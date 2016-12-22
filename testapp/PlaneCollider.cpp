#include "PlaneCollider.h"
#include "GameObject.h"
#include "AssetSerializer.h"

namespace Jasper
{


PlaneCollider::PlaneCollider(std::string name, Vector3 normal, float constant, PhysicsWorld * world) :
	PhysicsCollider(name, Vector3(), world), Normal(normal), Constant(constant)
{	
	m_colliderType = PHYSICS_COLLIDER_TYPE::Plane;
}

PlaneCollider::~PlaneCollider()
{
}

void PlaneCollider::Serialize(std::ofstream & ofs) const
{	
	using namespace AssetSerializer;
	PhysicsCollider::Serialize(ofs);
	ofs.write(ConstCharPtr(Normal.AsFloatPtr()), sizeof(Normal));
	ofs.write(ConstCharPtr(&Constant), sizeof(Constant));
}

void PlaneCollider::Awake()
{
    auto go = GetGameObject();
    auto& trans = go->GetLocalTransform();
    auto btTrans = trans.GetBtTransform();
    m_collisionShape = new btStaticPlaneShape(Normal.AsBtVector3(), Constant);
    m_defaultMotionState = new btDefaultMotionState(btTrans);
    btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState, m_collisionShape, btVector3(0.f, -1.f, 0.f));
    m_rigidBody = new btRigidBody(rbci);
    m_rigidBody->setRestitution(Restitution);
    m_rigidBody->setFriction(Friction);
    m_world->AddCollider(this);

}
}
