#pragma once

#include "PhysicsCollider.h"

namespace Jasper
{
class ConvexHullCollider : public PhysicsCollider
{
public:
    explicit ConvexHullCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world);
    void Awake() override;
	void Initialize() override;
	void InitFromMeshes(const std::vector<Mesh*>& meshes);

private:
	std::vector<Mesh*> m_meshes;
	btTriangleMesh* m_btm;

};

}
