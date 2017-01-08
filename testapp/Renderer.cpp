

#include "Renderer.h"
#include "MeshRenderer.h"
#include "Scene.h"
#include "Material.h"
#include "GLError.h"
#include <algorithm>
#include "BasicShader.h"
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

    int i = 0;
    // create a framebuffer for shadow mapping...
    //CreateShadowMapObjects();
}

void Renderer::SetFrameInvariants(Material* material)
{
    // ingariants accross all models include the
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

void Renderer::SetMaterialUniforms(Material * material)
{
    Shader* shader = material->GetShader();
    shader->SetMaterialUniforms(material);
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
        }
    }
}

bool Renderer::TestFrustum(const Frustum& frustum, const Vector3& position, const Vector3& half)
{
    uint ii;
    Vector3 absHalf = AbsVal(half);

    Vector3 planeToPoint = position - frustum.Vertices[0]; // Use near-clip-top-left point for point on first three planes
    for( ii=0; ii<3; ii++ )
    {
        Vector3 normal      = frustum.Planes[ii].Normal;
        Vector3 absNormal   = AbsVal(normal);
        float  nDotC       = Dot( normal, planeToPoint );
        if( nDotC > Dot( absNormal, absHalf ) )
        {
            return false;
        }
    }

    planeToPoint = position - frustum.Vertices[6]; // Use near-clip-top-left point for point on first three planes
    for( ii=3; ii<6; ii++ )
    {
        Vector3 normal      = frustum.Planes[ii].Normal;
        Vector3 absNormal   = AbsVal(normal);
        float  nDotC       = Dot( normal, planeToPoint );
        if( nDotC > Dot( absNormal, absHalf ) )
        {
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
