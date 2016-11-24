#ifndef _ALENGINE_SCENE_H_
#define _ALENGINE_SCENE_H_

#include "Common.h"
#include "GameObject.h"
#include "Lights.h"
#include "Shader.h"
#include "Camera.h"
#include "PhysicsWorld.h"
#include "Texture.h"
#include <vector>
#include <typeinfo>
#include "ResourceManager.h"
#include "Mesh.h"
#include "Model.h"
#include "FontRenderer.h"
#include "Renderer.h"
#include "Material.h"
#include "CharacterController.h"

namespace Jasper
{

class Scene
{
public:
    explicit Scene(int width, int height);
    ~Scene();

    void Resize(int width, int height);    

    Matrix4& ProjectionMatrix() {
        return m_projectionMatrix;
    }

    Matrix4& OrthographicMatrix() {
        return m_orthoMatrix;
    }

    GameObject* GetRootNode() const {
        return m_rootNode.get();
    }

    void AddGameObject(std::unique_ptr<GameObject> go);

    void DestroyGameObject(GameObject* go);

    GameObject* GetGameObjectByName(std::string name);

    void Update(float dt);
    void Awake();
    void Start();
    void Destroy();

    Camera& GetCamera() {
        if (m_camera)
            return *m_camera;
        m_camera = GetGameObjectByType<Camera>();
        return *m_camera;
    };

    CharacterController* GetPlayer() {
        return m_player.get();
    }

    void DoLeftClick(double x, double y);

    void DestroyGameObject(std::unique_ptr<GameObject> object);

    GameObject* CreateEmptyGameObject(std::string name, GameObject* parent);

    template<typename T> T* GetGameObjectByType() {
        for (auto& go : m_rootNode->Children()) {
            if (auto found = dynamic_cast<T*>(go.get())) {
                return found;
            }
        }
        return nullptr;
    }

    PhysicsWorld* GetPhysicsWorld() const {
        return m_physicsWorld.get();
    }

    Shader* GetShaderByName(std::string name);
    Material* GetMaterialByName(std::string name);
    
    void Deserialize(const std::string& filepath);
    void Serialize(const std::string& filepath);

    Renderer* GetRenderer() const {
        return m_renderer.get();
    }

    template<typename T, typename... Args>
    T* CreateMesh(Args&&... args) {
        return m_meshManager.CreateInstance<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    T* CreateModel(Args&&... args) {
        return m_modelManager.CreateInstance<T>(std::forward<Args>(args)...);
    }

    //Material* GetMaterialByName(const std::string& name);
    //Mesh*     GetMeshByName(const std::string& name);

    ResourceManager<Material>& GetMaterialCache() {
        return m_materialManager;
    }

    ResourceManager<Mesh>& GetMeshCache() {
        return m_meshManager;
    }

    ResourceManager<Model>& GetModelCache() {
        return m_modelManager;
    }

    ResourceManager<Texture>& GetTextureCache() {
        return m_textureManager;
    }
    
    ResourceManager<Shader>& GetShaderCache(){
        return m_shaderManager;
    }

    int m_windowWidth, m_windowHeight;
    
    double PhysicsFrameTime;
    double UpdateFrameTime;
    double RendererFrameTime;

private:

    std::unique_ptr<GameObject> m_rootNode;

    Matrix4 m_projectionMatrix;
    Matrix4 m_orthoMatrix;

    Camera* m_camera;

    std::unique_ptr<PhysicsWorld> m_physicsWorld;

    ResourceManager<Shader> m_shaderManager;
    ResourceManager<Mesh> m_meshManager;
    ResourceManager<Material> m_materialManager;
    ResourceManager<Texture> m_textureManager;
    ResourceManager<Model> m_modelManager;

    std::unique_ptr<FontRenderer> m_fontRenderer;

    std::unique_ptr<Renderer> m_renderer;

    std::unique_ptr<CharacterController> m_player;

    std::vector<ScriptComponent> m_scripts;

    void Initialize();


};


}
#endif // _SCENE_H_
