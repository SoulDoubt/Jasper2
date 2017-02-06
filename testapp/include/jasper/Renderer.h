#pragma once

#include "Common.h"

#include "GameObject.h"
#include "GBuffer.h"
#include "Shader.h"
#include "Quad.h"


namespace Jasper
{

class Shader;
class Material;
class MeshRenderer;
class Mesh;
class Sphere;

class Renderer
{

public:

    explicit Renderer(Scene* scene);
    Renderer(const Renderer& o) = delete;
    Renderer& operator=(const Renderer& o) = delete;

    void Initialize();
    void Destroy();

    void RenderScene();
    void RenderGeometryPass();
    void RenderStencilPass(PointLight* pl);
    void RenderDirectionalLightPass();
    void RenderPointLightPass(PointLight* pl);
    void RenderFinalPass();
    void RenderSkybox();
    void BlitDepthBufferToScreen();

    void RegisterGameObject(GameObject* obj);
    void UnregisterGameObject(GameObject* obj);


    const size_t GetMeshRendererCount() const {
        return m_renderersToRender.size();
    }

    void SetSkyboxRenderer(MeshRenderer* sbr) {
        m_skyboxRenderer = sbr;
    }

    void RenderDeferred();

private:

    void NewFrame();

    void BeginLightingPasses();

    void ProcessGameObject(const GameObject* go);
    void SortByMaterial();
    void SortByTransparancy();
    void ReleaseTextures();
    void SetFrameInvariants(Material* material, Shader* shader);
    void SetGeometryPassFrameInvariants(Shader* shader);
    void SetMaterialUniforms(Material* material, Shader* shader);
    void SetGeometryPassMaterialUniform(Material* material, Shader* shader);
    void CullGameObjects();
    bool TestFrustum(const Frustum& frustum, const Vector3& position, const Vector3& halfExtents);

    void CreateShadowMapObjects();
    void RenderShadowMap();

    std::unique_ptr<GBuffer> m_gBuffer;
    std::unique_ptr<Quad> m_fullScreenQuad;
    Shader* m_geometryPassShader = nullptr;
    Shader* m_forwardLitShader = nullptr;
    Shader* m_debugShader = nullptr;
    Shader* m_skyboxShader = nullptr;
    Shader* m_stencilPassShader = nullptr;
    DirectionalLightPassShader* m_lightingPassShader;
    PointLightPassShader* m_pointLightPassShader;
    MeshRenderer* m_skyboxRenderer;

    std::unique_ptr<Sphere> m_lightSphere;

    Scene* m_scene;

    std::vector<GameObject*> m_renderObjects;
    std::vector<MeshRenderer*> m_renderers;
    std::vector<MeshRenderer*> m_renderersToRender;

    Shader* m_currentShader = nullptr;
    Material* m_currentMaterial = nullptr;

    uint m_shadowMapBufferID;
    uint m_shadowMapTextureID;

    int m_windowWidth;
    int m_windowHeight;

    Matrix4 m_viewMatrix;
    Matrix4 m_projectionMatrix;

};

}
