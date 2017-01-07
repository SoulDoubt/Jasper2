#include "PlaneCollider.h"
#include "GameObject.h"
#include "AssetSerializer.h"

namespace Jasper
{
using namespace std;

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
    auto btTrans = trans.AsBtTransform();
    m_collisionShape = make_unique<btStaticPlaneShape>(Normal.AsBtVector3(), Constant);
    m_defaultMotionState = make_unique<btDefaultMotionState>(btTrans);
    btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState.get(), m_collisionShape.get(), btVector3(0.f, -1.f, 0.f));
    m_rigidBody = make_unique<btRigidBody>(rbci);
    m_rigidBody->setRestitution(Restitution);
    m_rigidBody->setFriction(Friction);
    m_rigidBody->setUserPointer(GetGameObject());
    m_world->AddCollider(this);

}
}
