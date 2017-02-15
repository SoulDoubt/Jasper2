#pragma once
#include "Common.h"
#include "Component.h"
#include "PhysicsWorld.h"
#include "PhysicsCollider.h"
#include "ResourceManager.h"
#include "Mesh.h"
//#include "Scene.h"


class aiScene;
class aiNode;
class aiMesh;

namespace Jasper
{

class Shader;
class Material;
class Scene;

class ModelData
{
public:


    ModelData(std::string name) {
        m_name = std::move(name);

    }

    std::vector<Mesh*>& GetMeshes()  {
        return m_meshes;
    }
    std::vector<Material*>& GetMaterials()  {
        return m_materials;
    }

    std::string& GetName() {
        return m_name;
    }

    void AddMesh(Mesh* mesh) {
        m_meshes.push_back(mesh);
    }

    void AddMaterial(Material* mat) {
        m_materials.push_back(mat);
    }

    Skeleton* GetSkeleton() {
        return m_skeleton.get();
    }

    void CreateSkeleton() {
        m_skeleton = std::make_unique<Skeleton>();
    }

    void CreateRagdollCollider(Scene* scene, GameObject* go);

    std::unique_ptr<Skeleton> m_skeleton;

private:
    std::string            m_name;
    std::vector<Mesh*>     m_meshes;
    std::vector<Material*> m_materials;    

};

class ModelInstance : public Component
{

};

class ModelLoader //: public Component
{
public:
    ModelLoader(Scene* scene);
    ~ModelLoader();



    float Mass = 0.0f;
    float Restitution = 1.0f;
    float Friction = 1.0f;

    PHYSICS_COLLIDER_TYPE ColliderType = PHYSICS_COLLIDER_TYPE::Box;


    void Destroy();
//    virtual void Awake() override;
//    virtual void Update(float dt) override;

    Vector3 HalfExtents;
    Vector3 MinExtents;
    Vector3 MaxExtents;

    uint TriCount = 0;
    uint VertCount = 0;

    void SaveToAssetFile(const std::string& filename);
    void LoadModel(const std::string& filename, const std::string& name);
    void OutputMeshData(const std::string& filename);

    std::unique_ptr<GameObject> CreateModelInstance(const std::string& name, const std::string& modelName, bool generateCollider, bool splitColliders);

private:
    std::string m_name;
    Scene* m_scene;
    std::string m_filename;
    std::string m_directory;
    std::vector<Mesh*> m_model_meshes;
    std::vector<Material*> m_model_materials;
    PhysicsWorld* m_physicsWorld = nullptr;

    std::unique_ptr<Skeleton> m_modelSkeleton;
    int m_processedMeshCount = 0;
    bool m_enablePhysics = false;

    void ProcessAiSceneNode(const aiScene* aiscene, aiNode* ainode, const std::string& directory, ModelData* model_data);
    void ProcessAiMesh(const aiMesh* aimesh, const aiScene* aiscene, const std::string& directory, ModelData* model_data);
    void ConvexDecompose(Mesh* mesh, std::vector<std::unique_ptr<btConvexHullShape>>& shapes, Scene* scene);

    void BuildSkeleton(aiNode* ai_bone, BoneData* bone, bool isRoot = false);

    NON_COPYABLE(ModelLoader);


protected:

    void CalculateHalfExtents();
};




}
