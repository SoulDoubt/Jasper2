#include "ConvexHullCollider.h"
#include "GameObject.h"
#include "Mesh.h"
#include <BulletCollision/CollisionShapes/btShapeHull.h>

namespace Jasper
{

ConvexHullCollider::ConvexHullCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world)
    : PhysicsCollider(name, halfExtents, world), m_meshes()
{
    m_colliderType = PHYSICS_COLLIDER_TYPE::ConvexHull;
}

void ConvexHullCollider::Awake()
{

    auto go = GetGameObject();
    auto& trans = go->GetLocalTransform();
    auto btTrans = trans.GetBtTransform();

    //float halfX = m_halfExtents.x;
    //float halfY = m_halfExtents.y;
    //float halfZ = m_halfExtents.z;

    //halfX *= trans.Scale.x;
    //halfY *= trans.Scale.y;
    //halfZ *= trans.Scale.z;

	
	m_btm = new btTriangleMesh(true, false);
	
	for (const auto mesh : m_meshes) {
		int sz = mesh->Indices.size();
		for (int i = 0; i < sz; i += 3) {
			const unsigned index0 = mesh->Indices[i];
			const unsigned index1 = mesh->Indices[i + 1];
			const unsigned index2 = mesh->Indices[i + 2];
			btVector3 p0 = mesh->Positions[index0].AsBtVector3();
			btVector3 p1 = mesh->Positions[index1].AsBtVector3();
			btVector3 p2 = mesh->Positions[index2].AsBtVector3();

			m_btm->addTriangle(p0, p1, p2, false);
		}
	}
	btBvhTriangleMeshShape* bvh = new btBvhTriangleMeshShape(m_btm, true);
	bvh->buildOptimizedBvh();
	//bvh->usesQuantizedAabbCompression();
	//btShapeHull* shull = new btShapeHull(bcs);
	//shull->buildHull(bcs->getMargin());
	//btConvexHullShape* shape = new btConvexHullShape((btScalar*)shull->getVertexPointer(), shull->numVertices());
	m_collisionShape = bvh;

    btVector3 inertia;
	Mass = 0.0f;
    //m_collisionShape->calculateLocalInertia(Mass, inertia);

    m_defaultMotionState = new btDefaultMotionState(btTrans);
    btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState, m_collisionShape, inertia);
    m_rigidBody = new btRigidBody(rbci);
    m_rigidBody->setUserPointer(GetGameObject());
	m_collisionShape->setLocalScaling(trans.Scale.AsBtVector3());

    m_world->AddCollider(this);
	//delete shull;
	//delete bcs;
	//delete btm;
}

void ConvexHullCollider::Initialize()
{
	
}

void ConvexHullCollider::InitFromMeshes(const std::vector<Mesh*>& meshes)
{
	for (const auto m : meshes) {
		m_meshes.push_back(m);
	}
}

}
