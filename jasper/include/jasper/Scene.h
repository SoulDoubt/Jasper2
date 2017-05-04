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

#include <mutex>



namespace Jasper
{

class Skeleton;

class Scene
{
public:

    struct PickRay {
        Vector3 start;
        Vector3 end;
        Vector3 direction;
    };
    
    explicit Scene(int width, int height);
    ~Scene();

    void Resize(int width, int height);

//    Matrix4& ProjectionMatrix() {
//        return m_projectionMatrix;
//    }

    Matrix4& OrthographicMatrix() {
        return m_orthoMatrix;
    }

    GameObject* GetRootNode() const {
        return m_rootNode.get();
    }

    void Initialize();

    void InitializeManual();

    void AddGameObject(std::unique_ptr<GameObject> go);

    void DestroyGameObject(GameObject* go);

    GameObject* GetGameObjectByName(const std::string& name);

    void Update(double dt);
    void Awake();
    void Start();
    void Destroy();

    Camera& GetCamera() {
        if (m_camera)
            return *m_camera;
        m_camera = GetGameObjectByType<Camera>();
        return *m_camera;
    };

    /*CharacterController* GetPlayer() {
        return m_player.get();
    }*/

	void LoadModel(const std::string& filename, const std::string& name);
	void LoadAssetModel(const std::string& filename, const std::string& name);
	void DeleteGameObject(GameObject* go, GameObject* parent);
    
    void DrawPickRay();

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

    Shader* GetShaderByName(const std::string& name);
    Material* GetMaterialByName(const std::string& name);

    void Deserialize(const std::string& filepath);
    void Serialize(const std::string& filepath);

    Renderer* GetRenderer() const {
        return m_renderer.get();
    }

    template<typename T, typename... Args>
    T* CreateMesh(Args&&... args) {
        return m_meshManager.CreateInstance<T>(std::forward<Args>(args)...);
    }

//    template<typename T, typename... Args>
//    T* CreateModel(Args&&... args) {
//        return m_modelManager.CreateInstance<T>(std::forward<Args>(args)...);
//    }

    const btCollisionObject* MousePickGameObject(int x, int y, Vector3& hit_point, Vector3& hit_normal);
    void MouseSelectGameObject(int x, int y);
    void ShootMouse(int x, int y);
    void MouseMoveSelectedGameObject(int xrel, int yrel);

    //Material* GetMaterialByName(const std::string& name);
    //Mesh*     GetMeshByName(const std::string& name);

    void SerializeGameObject(const GameObject* go, std::ofstream& ofs);
    void DeserializeGameObject(std::ifstream& ifs, GameObject* parent);

    void DebugDrawPhysicsWorld();
	void DebugDrawSkeleton(Skeleton* skeleton, Transform t);

	Skeleton* skeleton_to_debug = nullptr;
	Transform skeleton_debug_game_object_transform;


    ResourceManager<Material>& GetMaterialCache() {
        return m_materialManager;
    }

    ResourceManager<Mesh>& GetMeshCache() {
        return m_meshManager;
    }

    ResourceManager<ModelData>& GetModelCache() {
        return m_modelManager;
    }

    ResourceManager<Texture>& GetTextureCache() {
        return m_textureManager;
    }

    ResourceManager<Shader>& GetShaderCache() {
        return m_shaderManager;
    }

	ResourceManager<Skeleton>& GetSkeletonCache() {
		return m_skeletons;
	}
    
    std::vector<PointLight*>& GetPointLights() {
        return m_pointLights;
    }

    Camera* GetPlayer() const {
        return m_player;
    }

    int m_windowWidth, m_windowHeight;

    double PhysicsFrameTime;
    double UpdateFrameTime;
    double RendererFrameTime;
    
    GameObject* GetSelectedGameObject() const {
        return m_selected_game_object;
    }
    
    void SetSelectedGameObject(GameObject* go){
        if (go) m_selected_game_object = go;
        else m_selected_game_object = nullptr;
    }
    
    DirectionalLight* GetDirectionalLight() const {
        return m_directionalLight;
    }
    
    void TogglePhysicsDebugDraw(){
        debug_draw_physics =  !debug_draw_physics;
    }

	Material* GetDefaultMaterial() {
		return m_defaultMaterial;
	}

private:

    std::unique_ptr<GameObject> m_rootNode;
    
    PickRay m_pickRay;

    //Matrix4 m_projectionMatrix;
    Matrix4 m_orthoMatrix;

    Camera* m_camera;
    Camera* m_player;

    std::unique_ptr<PhysicsWorld> m_physicsWorld;

    ResourceManager<Shader> m_shaderManager;
    ResourceManager<Mesh> m_meshManager;
    ResourceManager<Material> m_materialManager;
    ResourceManager<Texture> m_textureManager;
    ResourceManager<ModelData> m_modelManager;    
	ResourceManager<Skeleton> m_skeletons;
    
    std::vector<PointLight*> m_pointLights;
    
    DirectionalLight* m_directionalLight;
    
    std::unique_ptr<FontRenderer> m_fontRenderer;

    std::unique_ptr<Renderer> m_renderer;

    // std::unique_ptr<CharacterController> m_player;

    std::vector<ScriptComponent> m_scripts;
    std::unique_ptr<GameObject> m_skybox;

    bool debug_draw_physics = false;
    
    GameObject* m_selected_game_object = nullptr;

	Material* m_defaultMaterial = nullptr;

	std::mutex m_background_tasks_mutex;


};


}
#endif // _SCENE_H_
