

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
#include "Sphere.h"



namespace Jasper
{

using namespace std;

void SetGeometryPassGLState();
void SetPointLightStencilPassGLState();
void SetPointLightPassGLState();
void SetDirectionalLightGLState();
void SetDefaultGLState();

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


    CreateShadowMapObjects();


    m_lightingPassShader = (DirectionalLightPassShader*)(m_scene->GetShaderCache().GetResourceByName("lighting_pass_shader"s));
    m_pointLightPassShader = (PointLightPassShader*)(m_scene->GetShaderCache().GetResourceByName("pointlightpass_shader"s));
    m_pointLightPassShader->SetScreenSize(Vector2(m_windowWidth, m_windowHeight));
    auto fsquadmat = m_scene->GetMaterialCache().CreateInstance<Material>("fs_quad_mat");
    m_fullScreenQuad = make_unique<Quad>("dl_pass_quad"s, Vector2(1.0, 1.0), Quad::AxisAlignment::XY);
    m_fullScreenQuad->SetMaterial(fsquadmat);
    m_fullScreenQuad->SetVertexFormat(Mesh::VERTEX_FORMAT::Vertex_P);
    m_fullScreenQuad->Initialize();

    m_gBuffer = make_unique<GBuffer>();
    m_gBuffer->Initialize(m_windowWidth, m_windowHeight);

    m_geometryPassShader = m_scene->GetShaderCache().GetResourceByName("geometry_pass_shader"s);
    m_forwardLitShader = m_scene->GetShaderCache().GetResourceByName("lit_shader");
    m_skyboxShader = m_scene->GetShaderCache().GetResourceByName("skybox_shader");
    m_debugShader = m_scene->GetShaderCache().GetResourceByName("basic_shader");
    m_stencilPassShader = m_scene->GetShaderCache().GetResourceByName("deferred_stencil_pass");
    auto skyboxMesh = m_scene->GetMeshCache().GetResourceByName("skybox_cube_mesh");
    skyboxMesh->InitializeForRendering(m_skyboxShader);
    m_fullScreenQuad->InitializeForRendering(m_lightingPassShader);
    m_lightSphere = make_unique<Sphere>("point_light_sphere", 1.0f);
    m_lightSphere->SetMaterial(fsquadmat);
    m_lightSphere->Initialize();
    m_lightSphere->SetVertexFormat(Mesh::VERTEX_FORMAT::Vertex_P);
    m_lightSphere->InitializeForRendering(m_pointLightPassShader);
    m_forwardLitShader = m_scene->GetShaderCache().GetResourceByName("Lit_Shader");
    ProcessGameObject(root);
    SortByMaterial();
    SortByTransparancy();
   
    //int xx = 0;
    // create a framebuffer for shadow mapping...
    //CreateShadowMapObjects();
}

void Renderer::SetFrameInvariants(Material* material, Shader* shader)
{
    // invariants accross all models include the
    // projection matrix,
    // view matrix,
    // lights
    //Shader* shader = material->GetShader();
    //auto projectionMatrix = m_scene->ProjectionMatrix();
    const auto viewMatrix = m_scene->GetCamera().GetViewMatrix();
    //const auto pointLight = m_scene->GetGameObjectByName("p_light"s);
    //const auto directionalLight = m_scene->GetGameObjectByName("d_light"s);
    const auto cameraPosition = m_scene->GetCamera().GetWorldTransform().Position;

    shader->SetViewMatrix(viewMatrix);
    shader->SetCameraPosition(cameraPosition);

//    if (pointLight) {
//        shader->SetPointLightUniforms((PointLight*)pointLight);
//    }
//
//    if (directionalLight) {
//        shader->SetDirectionalLightUniforms((DirectionalLight*)directionalLight);
//    }
}

void SetGeometryPassFrameInvariants(Shader* shader)
{

}

void Renderer::SetMaterialUniforms(Material * material, Shader* shader)
{
    //Shader* shader = material->GetShader();
    shader->SetMaterialUniforms(material);
}

void Renderer::SetGeometryPassMaterialUniform(Material* material, Shader* shader)
{
    m_geometryPassShader->SetMaterialUniforms(material);
}

void Renderer::CullGameObjects()
{
    const Frustum& frustum = m_scene->GetCamera().GetFrustum();

    m_renderersToRender.clear();
    for (auto& mr : m_renderers) {
        auto go = mr->GetGameObject();
        //Mesh* mesh = mr->GetMesh();
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
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("Shadown Map FB error, status: 0x%x\n", Status);
    } else {
        printf("Shadow Map Framebuffer OK!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::RenderShadowMap()
{

}



void Renderer::RenderScene()
{
    NewFrame();
    //glCullFace(GL_BACK);
    //glEnable(GL_DEPTH_TEST);
    //glDepthMask(GL_TRUE);
    static Material* previousMaterial = nullptr;

    //const auto projMatrix = m_scene->GetCamera().GetProjectionMatrix();
    //const auto viewMatrix = m_scene->GetCamera().GetViewMatrix();
    CullGameObjects();
    previousMaterial = nullptr;
    m_forwardLitShader->Bind();
    for (auto& mr : m_renderersToRender) {

        if (!mr->IsEnabled()) continue;
        if (!mr->IsVisible()) continue;

        auto material = mr->GetMaterial();

        if (material != previousMaterial) {
            previousMaterial = material;
            //material->SetShader(m_forwardLitShader);
            material->Bind(m_forwardLitShader);
            SetFrameInvariants(material, m_forwardLitShader);
            SetMaterialUniforms(material, m_forwardLitShader);
        }
        //m_forwardLitShader->SetDirectionalLightUniforms(m_scene->GetDirectionalLight());
        
        const auto transform = mr->GetGameObject()->GetWorldTransform();
        const auto modelMatrix = transform.TransformMatrix();
        const auto normMatrix = modelMatrix.NormalMatrix();
        m_forwardLitShader->SetMatrixUniforms(modelMatrix, m_viewMatrix, m_projectionMatrix, normMatrix);
        const auto pl = m_scene->GetPointLights()[1];
        //for (const auto pl : m_scene->GetPointLights()){
            m_forwardLitShader->SetPointLightUniforms(pl);
            mr->Render();
        
        //}
        if (material != previousMaterial) {
            material->Release();
        }
    }
    m_forwardLitShader->Release();
}

void Renderer::NewFrame()
{
    m_projectionMatrix = m_scene->GetCamera().GetProjectionMatrix();
    m_viewMatrix = m_scene->GetCamera().GetViewMatrix();
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_gBuffer->ID());
    //glDrawBuffer(GL_COLOR_ATTACHMENT4);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::RenderDeferred()
{
    NewFrame();
    RenderGeometryPass();
    //BeginLightingPasses();
    
    glEnable(GL_STENCIL_TEST);

    for (const auto pl : m_scene->GetPointLights()) {
        RenderStencilPass(pl);
        RenderPointLightPass(pl);
    }

    glDisable(GL_STENCIL_TEST);

    SetDirectionalLightGLState();
    RenderDirectionalLightPass();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    RenderFinalPass();
    // copy the fbo depth buffer to the main
    BlitDepthBufferToScreen();


}

void Renderer::RenderGeometryPass()
{
    m_gBuffer->BindForGeometryPass();

    glDepthMask(GL_TRUE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    static Material* previousMaterial = nullptr;

    CullGameObjects();
    previousMaterial = nullptr;
    m_gBuffer->BindForWriting();
    //SetGeometryPassGLState();
    m_geometryPassShader->Bind();
    for (auto& mr : m_renderersToRender) {

        if (!mr->IsEnabled()) continue;
        if (!mr->IsVisible()) continue;

        auto material = mr->GetMaterial();

        if (material != previousMaterial) {
            previousMaterial = material;
            //material->SetShader(m_geometryPassShader);
            material->Bind(m_geometryPassShader);
            //SetFrameInvariants(material, m_geometryPassShader);
            SetMaterialUniforms(material, m_geometryPassShader);
        }

        const auto transform = mr->GetGameObject()->GetWorldTransform();
        const auto modelMatrix = transform.TransformMatrix();
        const auto normMatrix = modelMatrix.NormalMatrix();
        m_geometryPassShader->SetMatrixUniforms(modelMatrix, m_viewMatrix, m_projectionMatrix, normMatrix);
        mr->Render();
        if (material != previousMaterial) {
            material->Release();
        }
    }

    m_geometryPassShader->Release();


    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





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

void Renderer::BeginLightingPasses()
{
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    glDrawBuffer(GL_COLOR_ATTACHMENT4);

    m_gBuffer->BindForReading();
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::RenderStencilPass(PointLight* pl)
{
    //SetPointLightStencilPassGLState();
    m_gBuffer->BindForStencilPass();
    m_stencilPassShader->Bind();
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_CULL_FACE);

    glClear(GL_STENCIL_BUFFER_BIT);

    // We need the stencil test to be enabled but we want it
    // to succeed always. Only the depth test matters.
    glStencilFunc(GL_ALWAYS, 0, 0);

    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    pl->UniformScale(pl->Radius);

    m_stencilPassShader->SetMatrixUniforms(pl->GetWorldTransform().TransformMatrix(), m_viewMatrix, m_projectionMatrix, Matrix3());
    glBindVertexArray(m_lightSphere->VaoID());
    glDrawElements(GL_TRIANGLES, m_lightSphere->ElementCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_stencilPassShader->Release();

}

void Renderer::RenderPointLightPass(PointLight* pl)
{

    //SetPointLightPassGLState();

    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    m_gBuffer->BindForPointLightPass();
    m_pointLightPassShader->Bind();

    glActiveTexture(GL_TEXTURE0);
    m_pointLightPassShader->SetActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer->gPosition);

    glActiveTexture(GL_TEXTURE1);
    m_pointLightPassShader->SetActiveTexture(1);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer->gDiffuse);

    glActiveTexture(GL_TEXTURE2);
    m_pointLightPassShader->SetActiveTexture(2);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer->gNormal);

    glActiveTexture(GL_TEXTURE3);
    m_pointLightPassShader->SetActiveTexture(3);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer->gSpecular);

    glActiveTexture(GL_TEXTURE4);
    m_pointLightPassShader->SetActiveTexture(4);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer->gFinal);



    //sphere->GetLocalTransform().Scale = {10.0f, 10.0f, 10.0f};
    //glDisable(GL_CULL_FACE);
    //const auto projMatrix = m_scene->GetCamera().GetProjectionMatrix();
    //const auto viewMatrix = m_scene->GetCamera().GetViewMatrix();

    pl->GetLocalTransform().UniformScale(pl->Radius);
    //Vector3 lightPosition = pl->GetWorldTransform().Position;
    Vector3 cameraPosition = m_scene->GetCamera().GetPosition();
    m_pointLightPassShader->SetCameraPosition(cameraPosition);
    m_pointLightPassShader->SetPointLightUniforms(pl);
    m_pointLightPassShader->SetMatrixUniforms(pl->GetWorldTransform().TransformMatrix(), m_viewMatrix, m_projectionMatrix, Matrix3());
    glPolygonMode(GL_FRONT, GL_FILL);
    glBindVertexArray(m_lightSphere->VaoID());
    glDrawElements(GL_TRIANGLES, m_lightSphere->ElementCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    m_pointLightPassShader->Release();

    glCullFace(GL_BACK);

    glDisable(GL_BLEND);
//m_gBuffer->BindForReading();
// now blit the gbuffer's depth buffer into the default FB for any forward rendering to come
//glBlitFramebuffer(0, 0, m_windowWidth, m_windowHeight, 0, 0, m_windowWidth, m_windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

}

void Renderer::RenderDirectionalLightPass()
{
    // When we get here the depth buffer is already populated and the stencil pass
    // depends on it, but it does not write to it.
    //glDrawBuffer(GL_FRONT);
    //SetDirectionalLightGLState();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    m_gBuffer->BindForDirectionalLightPass();

    m_lightingPassShader->Bind();

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
    glBindTexture(GL_TEXTURE_2D, m_gBuffer->gSpecular);

    glActiveTexture(GL_TEXTURE4);
    m_lightingPassShader->SetActiveTexture(4);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer->gFinal);

    int poly_mode[2];
    glGetIntegerv(GL_POLYGON_MODE, poly_mode);
    glBindVertexArray(m_fullScreenQuad->VaoID());

    glPolygonMode(GL_FRONT, GL_FILL);
    glDrawElements(GL_TRIANGLES, m_fullScreenQuad->ElementCount(), GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT, poly_mode[0]);
    GLERRORCHECK;
    glBindVertexArray(0);

    //material->Release();
    m_lightingPassShader->Release();
    glDisable(GL_BLEND);


}



void Renderer::RenderFinalPass()
{
    //SetDefaultGLState();
    m_gBuffer->BindForFinalPass();
    // just blit the read buffer into the current fb
    glBlitFramebuffer(0, 0, m_windowWidth, m_windowHeight,
                      0, 0, m_windowWidth, m_windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void Renderer::RenderSkybox()
{
    m_skyboxShader->Bind();
    auto& camera = m_scene->GetCamera();

    //shader->Bind();

    // we need the current view matrix and projection matrix of the scene
    const auto projection = camera.GetProjectionMatrix();
    const auto view = camera.GetCubemapViewMatrix();

    //trix4 view;
    //view.SetToIdentity();
    //const auto pv = projection * view;
    m_skyboxShader->SetProjectionMatrix(projection);
    m_skyboxShader->SetViewMatrix(view);
    auto mat = m_skyboxRenderer->GetMaterial();
    m_skyboxRenderer->Render();
    mat->Release();
    m_skyboxShader->Release();
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
            const auto mrs = child->GetComponentsByType<MeshRenderer>();
            for (const auto mr : mrs) {
                auto mesh = mr->GetMesh();
                if (0) {
                    mesh->InitializeForRendering(m_geometryPassShader);
                } else{
                    mesh->InitializeForRendering(m_forwardLitShader);
                }
                RegisterGameObject(child.get());
            }
        }
        ProcessGameObject(child.get());
    }
}

void SetGeometryPassGLState()
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void SetPointLightStencilPassGLState()
{
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);
    // We need the stencil test to be enabled but we want it
    // to succeed always. Only the depth test matters.
    glStencilFunc(GL_ALWAYS, 0, 0);

    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
}

void SetPointLightPassGLState()
{
    glDisable(GL_DEPTH_TEST);

    //glEnable(GL_BLEND);
    //glBlendEquation(GL_FUNC_ADD);
    //glBlendFunc(GL_ONE, GL_ONE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
}

void SetDirectionalLightGLState()
{
    glDisable(GL_STENCIL_TEST);
    //glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void SetDefaultGLState()
{
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

}

void Renderer::BlitDepthBufferToScreen()
{
    m_gBuffer->BindForReading();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, m_windowWidth, m_windowHeight, 0, 0, m_windowWidth, m_windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

}
