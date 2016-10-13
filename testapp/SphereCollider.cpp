#include "SphereCollider.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Sphere.h"

namespace Jasper {

SphereCollider::SphereCollider(const std::string & name, Mesh * mesh, PhysicsWorld * world)
	:PhysicsCollider(name, mesh, world)
{

}

SphereCollider::SphereCollider(const std::string& name, const Vector3& halfExtents, PhysicsWorld* world)
	:PhysicsCollider(name, halfExtents, world)
{
	
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::Awake()
{
	auto go = GetGameObject();
	auto& trans = go->GetLocalTransform();
	auto btTrans = trans.GetBtTransform();
	float radius = 0.f;			
	if (m_mesh) {
		m_halfExtents = m_mesh->GetHalfExtents();
	}

	m_halfExtents.x = m_halfExtents.x * trans.Scale.x;
	m_halfExtents.y = m_halfExtents.y * trans.Scale.y;
	m_halfExtents.z = m_halfExtents.z * trans.Scale.z;

	auto extents = m_halfExtents.AsFloatPtr();
	for (int i = 0; i < 3; ++i) {
		float r = fabs(extents[i]);
		if (r > radius) {
			radius = r;
		}
	}				
	m_collisionShape = new btSphereShape(radius);

	btVector3 inertia;
	m_collisionShape->calculateLocalInertia(Mass, inertia);

	m_defaultMotionState = new btDefaultMotionState(btTrans);
	btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState, m_collisionShape, inertia);
	m_rigidBody = new btRigidBody(rbci);
	m_rigidBody->setRestitution(Restitution);
	m_rigidBody->setFriction(Friction);	
	m_world->AddCollider(this);
}

} // namespace Jasper