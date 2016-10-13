#ifndef _JASPER_MODEL_H_
#define _JASPER_MODEL_H_

#include "Common.h"
#include "GameObject.h"
#include "PhysicsWorld.h"
#include "PhysicsCollider.h"
#include "ResourceManager.h"
//#include "Scene.h"


class aiScene;
class aiNode;
class aiMesh;

namespace Jasper {

class Shader;
class Material;

class Model : public GameObject
{
public:
	explicit Model(const std::string& name, const std::string& filename, Shader* shader, bool enablePhysics = false, PhysicsWorld* physicsWorld = nullptr);
	~Model();

	float Mass = 0.0f;
	float Restitution = 1.0f;
	float Friction = 1.0f;

	PHYSICS_COLLIDER_TYPE ColliderType = PHYSICS_COLLIDER_TYPE::Box;

	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Awake() override;
	virtual void Update(float dt) override;

	Vector3 HalfExtents;
	Vector3 MinExtents;
	Vector3 MaxExtents;

	uint TriCount = 0;
	uint VertCount = 0;

private:
	std::string m_filename;
	std::string m_directory;
	Shader* m_shader;
	bool m_enablePhysics = false;
	void ProcessAiSceneNode(const aiScene* aiscene, aiNode* ainode);
	void ProcessAiMesh(const aiMesh* aimesh, const aiScene* aiscene);
	PhysicsWorld* m_physicsWorld = nullptr;
	ResourceManager<Mesh> m_meshManager;
	ResourceManager<Material> m_materialManager;

	NON_COPYABLE(Model);

protected:

	void CalculateHalfExtents();
};
}
#endif // _MODEL_H_