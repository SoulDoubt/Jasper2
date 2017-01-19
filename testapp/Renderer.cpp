

#include "Renderer.h"
#include "MeshRenderer.h"
#include "Scene.h"
#include "Material.h"
#include "GLError.h"
#include "GLWindow.h"
#include <algorithm>
#include "Shader.h"
#include "PhysicsCollider.h"
#include "GBuffer.h"



namespace Jasper
{

using namespace std;

Renderer::Renderer(Scene * scene) : m_scene(scene)
{
    m_windowWidth = m_scene->m_windowWidth;
    m_windowHeight = m_scene->m_windowHeight;
}

void Renderer::Initialize()
{
    //GBuffer gbuff = GBuffer(1024, 768);
    //gbuff.Initialize();
    const auto root = m_scene->GetRootNode();
    ProcessGameObject(root);
    SortByMaterial();
    SortByTransparancy();
    
    CreateShadowMapObjects();

    m_lightingPassShader = (DirectionalLightPassShader*)(m_scene->GetShaderCache().GetResourceByName("lighting_pass_shader"s));
    auto fsquadmat = m_scene->GetMaterialCache().CreateInstance<Material>(m_lightingPassShader, "fs_quad_mat");
    m_fullScreenQuad = make_unique<Quad>("dl_pass_quad"s, Vector2(1.0, 1.0), Quad::AxisAlignment::XY);
    m_fullScreenQuad->SetMaterial(fsquadmat);
    m_fullScreenQuad->Initialize();
    Quad* q = m_fullScreenQuad.get();
    m_gBuffer = make_unique<GBuffer>();
    m_gBuffer->Initialize(m_windowWidth, m_windowHeight);
    m_geometryPassShader = m_scene->GetShaderCache().GetResourceByName("geometry_pass_shader"s);
    m_forwardLitShader = m_scene->GetShaderCache().GetResourceByName("lit_shader");
   
    // create a framebuffer for shadow mapping...
    //CreateShadowMapObjects();
}

void Renderer::SetFrameInvariants(Material* material)
{
    // invariants accross all models include the
    // projection matrix,
    // view matrix,
    // lights
    Shader* shader = material->GetShader();
    //auto projectionMatrix = m_scene->ProjectionMatrix();
    const auto viewMatrix = m_scene->GetCamera().GetViewMatrix();
    const auto pointLight = m_scene->GetGameObjectByName("p_light"s);
    const auto directionalLight = m_scene->GetGameObjectByName("d_light"s);
    const auto cameraPosition = m_scene->GetCamera().GetWorldTransform().Position;

    shader->SetViewMatrix(viewMatrix);
    shader->SetCameraPosition(cameraPosition);

    if (pointLight) {
        shader->SetPointLightUniforms((PointLight*)pointLight, pointLight->GetWorldTransform().Position);
    }

    if (directionalLight) {
        shader->SetDirectionalLightUniforms((DirectionalLight*)directionalLight);
    }
}

void SetGeometryPassFrameInvariants(Shader* shader)
{

}

void Renderer::SetMaterialUniforms(Material * material)
{
    Shader* shader = material->GetShader();
    shader->SetMaterialUniforms(material);
}

void Renderer::SetGeometryPassMaterialUniform(Material* material)
{
    m_geometryPassShader->SetMaterialUniforms(material);
}

void Renderer::CullGameObjects()
{
    const Frustum& frustum = m_scene->GetCamera().GetFrustum();

    m_renderersToRender.clear();
    for (auto& mr : m_renderers) {
        auto go = mr->GetGameObject();
        Mesh* mesh = mr->GetMesh();
        //if (mesh->GetName() == "wall_mesh") {
        PhysicsCollider* c = go->GetComponentByType<PhysicsCollider>();
        if (c) {
            auto t = go->GetWorldTransform();
            auto bt = t.AsBtTransform();
            btVector3 min, max;
            c->GetCollisionShape()->getAabb(bt, min, max);
            Vector3 half = {((max - min) * 0.5f)};
            if (TestFrustum(frustum, t.Position, half)) {
                m_renderersToRender.push_back(mr);
            }
        } else {
            m_renderersToRender.push_back(mr);
        }
    }
}

bool Renderer::TestFrustum(const Frustum& frustum, const Vector3& position, const Vector3& half)
{
    uint ii;
    Vector3 absHalf = AbsVal(half);

    Vector3 planeToPoint = position - frustum.Vertices[0]; // Use near-clip-top-left point for point on first three planes
    for( ii=0; ii<3; ii++ ) {
        Vector3 normal      = frustum.Planes[ii].Normal;
        Vector3 absNormal   = AbsVal(normal);
        float  nDotC       = Dot( normal, planeToPoint );
        if( nDotC > Dot( absNormal, absHalf ) ) {
            return false;
        }
    }

    planeToPoint = position - frustum.Vertices[6]; // Use near-clip-top-left point for point on first three planes
    for( ii=3; ii<6; ii++ ) {
        Vector3 normal      = frustum.Planes[ii].Normal;
        Vector3 absNormal   = AbsVal(normal);
        float  nDotC       = Dot( normal, planeToPoint );
        if( nDotC > Dot( absNormal, absHalf ) ) {
            return false;
        }
    }

    // Tested all eight points against all six planes and none of the planes
    // had all eight points outside.
    return true;

}

void Renderer::CreateShadowMapObjects()
{
    glGenFramebuffers(1, &m_shadowMapBufferID);

    glGenTextures(1, &m_shadowMapTextureID);
    glBindTexture(GL_TEXTURE_2D, m_shadowMapTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,m_windowWidth, m_windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapBufferID);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMapTextureID, 0);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("Shadown Map FB error, status: 0x%x\n", Status);        
    } else {
        printf("Shadow Map Framebuffer OK!\n");
    }
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::RenderShadowMap()
{

}



void Renderer::RenderScene()
{
    static Material* previousMaterial = nullptr;

    const auto projMatrix = m_scene->GetCamera().GetProjectionMatrix();
    const auto viewMatrix = m_scene->GetCamera().GetViewMatrix();
    CullGameObjects();
    previousMaterial = nullptr;
    for (auto& mr : m_renderersToRender) {

        if (!mr->IsEnabled()) continue;
        if (!mr->IsVisible()) continue;

        auto material = mr->GetMaterial();

        if (material != previousMaterial) {
            previousMaterial = material;   
            material->SetShader(m_forwardLitShader);
            material->Bind();
            SetFrameInvariants(material);
            SetMaterialUniforms(material);
        }

        Shader* shader = material->GetShader();
        const auto transform = mr->GetGameObject()->GetWorldTransform();
        const auto modelMatrix = transform.TransformMatrix();
        const auto mvpMatrix = projMatrix * viewMatrix * modelMatrix;
        const auto normMatrix = modelMatrix.NormalMatrix();
        shader->SetModelMatrix(modelMatrix);
        shader->SetModelViewProjectionMatrix(mvpMatrix);
        shader->SetNormalMatrix(normMatrix);
        mr->Render();
        if (material != previousMaterial) {
            material->Release();
        }
    }
}

void Renderer::RenderGeometryPass()
{
    static Material* previousMaterial = nullptr;

    const auto projMatrix = m_scene->GetCamera().GetProjectionMatrix();
    const auto viewMatrix = m_scene->GetCamera().GetViewMatrix();
    CullGameObjects();
    previousMaterial = nullptr;
    m_gBuffer->BindForWriting();
    glDepthMask(GL_TRUE);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    for (auto& mr : m_renderersToRender) {

        if (!mr->IsEnabled()) continue;
        if (!mr->IsVisible()) continue;

        auto material = mr->GetMaterial();

        if (material != previousMaterial) {
            previousMaterial = material;
            material->SetShader(m_geometryPassShader);
            material->Bind();
            SetFrameInvariants(material);
            SetMaterialUniforms(material);
        }


        Shader* shader = material->GetShader();
                        
        const auto transform = mr->GetGameObject()->GetWorldTransform();
        const auto modelMatrix = transform.TransformMatrix();
        const auto mvpMatrix = projMatrix * viewMatrix * modelMatrix;
        const auto normMatrix = modelMatrix.NormalMatrix();
        shader->SetModelMatrix(modelMatrix);
        shader->SetModelViewProjectionMatrix(mvpMatrix);
        shader->SetNormalMatrix(normMatrix);
        mr->Render();
        if (material != previousMaterial) {
            material->Release();
        }
    }    
    
   

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    
    

    //GLint HalfWidth = (GLint)(m_windowWidth / 2.0f);
    //GLint HalfHeight = (GLint)(m_windowHeight / 2.0f);
    /*
    m_gBuffer->BindForReading();
    m_gBuffer->SetReadBuffer(GBUFFER_TEXTURE_TYPE_NORMAL);
    glBlitFramebuffer(0, 0,
                      m_windowWidth,
                      m_windowHeight,
                      0, 0,
                      m_windowWidth,
                      m_windowHeight,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    GLERRORCHECK;
    */
}

void Renderer::RenderDirectionalLightPass(){
    // When we get here the depth buffer is already populated and the stencil pass
    // depends on it, but it does not write to it.
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    // create a full screen quad to render
    glEnable(GL_BLEND);
   	glBlendEquation(GL_FUNC_ADD);
   	glBlendFunc(GL_ONE, GL_ONE);

    m_gBuffer->BindForReading();
    glClear(GL_COLOR_BUFFER_BIT);
    
    Material* material = m_fullScreenQuad->GetMaterial();
    material->SetShader(m_lightingPassShader);
    material->Bind();
    SetMaterialUniforms(material);
   
    const auto directionalLight = m_scene->GetDirectionalLight();
    m_lightingPassShader->SetDirectionalLightUniforms((DirectionalLight*)directionalLight);
    m_lightingPassShader->SetCameraPosition(m_scene->GetCamera().GetPosition());
    
    glActiveTexture(GL_TEXTURE0);
    m_lightingPassShader->SetActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer->gPosition);
    
    glActiveTexture(GL_TEXTURE1);
    m_lightingPassShader->SetActiveTexture(1);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer->gDiffuse);
    
    glActiveTexture(GL_TEXTURE2);
    m_lightingPassShader->SetActiveTexture(2);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer->gNormal);
    
    glActiveTexture(GL_TEXTURE3);
    m_lightingPassShader->SetActiveTexture(3);    
    glBindTexture(GL_TEXTURE_2D, m_gBuffer->gTexCoords);
    
    glActiveTexture(GL_TEXTURE4);
    m_lightingPassShader->SetActiveTexture(4);    
    glBindTexture(GL_TEXTURE_2D, m_gBuffer->gSpecular);
    
    int poly_mode[2];
    glGetIntegerv(GL_POLYGON_MODE, poly_mode);
    glBindVertexArray(m_fullScreenQuad->VaoID());
    
    glPolygonMode(GL_FRONT, GL_FILL);
    glDrawElements(GL_TRIANGLES, m_fullScreenQuad->ElementCount(), GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT, poly_mode[0]);
    GLERRORCHECK;
    glBindVertexArray(0);
    
    material->Release();
    
    m_gBuffer->BindForReading();
    // now blit the gbuffer's depth buffer into the default FB for any forward rendering to come
    glBlitFramebuffer(0, 0, m_windowWidth, m_windowHeight, 0, 0, m_windowWidth, m_windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
   	//glBlendEquation(GL_FUNC_ADD);
   	//glBlendFunc(GL_ONE, GL_ONE);
        
}

void Renderer::RenderPointLightPass(){
    
    // GL Setup stuff.
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    // create a full screen quad to render
    glEnable(GL_BLEND);
   	glBlendEquation(GL_FUNC_ADD);
   	glBlendFunc(GL_ONE, GL_ONE);

    m_gBuffer->BindForReading();
    
    auto pointLights = m_scene->GetPointLights();
    for (const auto pl : pointLights){
        Vector3 lightPosition = pl->GetWorldTransform().Position;
        Vector3 cameraPosition = m_scene->GetCamera().GetPosition();
        
    }
    
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::RenderSkybox(){
    auto mat = m_skyboxRenderer->GetMaterial();
    mat->Bind();
    m_skyboxRenderer->Render();
    mat->Release();
}


void Renderer::RegisterGameObject(GameObject* obj)
{

    if (find(begin(m_renderObjects), end(m_renderObjects), obj) == end(m_renderObjects)) {
        m_renderObjects.push_back(obj);
        const auto& renderers = obj->GetComponentsByType<MeshRenderer>();
        if (renderers.size() > 0) {
            m_renderers.insert(end(m_renderers), begin(renderers), end(renderers));
        }
    }
}

void Renderer::UnregisterGameObject(GameObject* obj)
{
    auto go = find(begin(m_renderObjects), end(m_renderObjects), obj);
    auto mrs = obj->GetComponentsByType<MeshRenderer>();
    if (go != end(m_renderObjects)) {
        m_renderObjects.erase(go);
    }
    for (auto mr : mrs) {
        auto mrf = find(begin(m_renderers), end(m_renderers), mr);
        if (mrf != end(m_renderers)) {
            m_renderers.erase(mrf);
        }
    }

}

void Renderer::SortByMaterial()
{
    sort(begin(m_renderers), end(m_renderers), [&](const MeshRenderer* a, const MeshRenderer* b) {
        const Material* mata = a->GetMaterial();
        const Material* matb = b->GetMaterial();

        return (mata > matb);
    });
}

void Renderer::SortByTransparancy()
{
    sort(begin(m_renderers), end(m_renderers), [&](const MeshRenderer* a, const MeshRenderer* b) {
        const Material* mata = a->GetMaterial();
        const Material* matb = b->GetMaterial();

        return mata->IsTransparent > matb->IsTransparent;
    });
}

void Renderer::ReleaseTextures()
{

}

void Renderer::ProcessGameObject(const GameObject* root)
{
    for (const auto& child : root->Children()) {
        if (child->GetName() != "skybox") {
            const auto mr = child->GetComponentByType<MeshRenderer>();
            if (mr != nullptr) {
                RegisterGameObject(child.get());
            }
        }
        ProcessGameObject(child.get());
    }
}

}
