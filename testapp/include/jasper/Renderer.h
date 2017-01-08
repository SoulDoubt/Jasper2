#pragma once

#include "Common.h"

#include "GameObject.h"


namespace Jasper
{

class Shader;
class Material;
class MeshRenderer;
class Mesh;

class Renderer
{

public:

    explicit Renderer(Scene* scene);
    Renderer(const Renderer& o) = delete;
    Renderer& operator=(const Renderer& o) = delete;

    void Initialize();
    void Destroy();

    void RenderScene();

    void RegisterGameObject(GameObject* obj);
    void UnregisterGameObject(GameObject* obj);


    const size_t GetMeshRendererCount() const {
        return m_renderersToRender.size();
    }

private:

    void ProcessGameObject(const GameObject* go);
    void SortByMaterial();
	void SortByTransparancy();
    void ReleaseTextures();
    void SetFrameInvariants(Material* material);
    void SetMaterialUniforms(Material* material);
    void CullGameObjects();
    bool TestFrustum(const Frustum& frustum, const Vector3& position, const Vector3& halfExtents);

    void CreateShadowMapObjects();
    void RenderShadowMap();


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

};

}
