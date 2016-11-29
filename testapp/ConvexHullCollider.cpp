#include "ConvexHullCollider.h"
#include "GameObject.h"

namespace Jasper
{

ConvexHullCollider::ConvexHullCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world)
    : PhysicsCollider(name, halfExtents, world)
{
    m_colliderType = PHYSICS_COLLIDER_TYPE::Cylinder;
}

void ConvexHullCollider::Awake()
{

    auto go = GetGameObject();
    auto& trans = go->GetLocalTransform();
    auto btTrans = trans.GetBtTransform();

    float halfX = m_halfExtents.x;
    float halfY = m_halfExtents.y;
    float halfZ = m_halfExtents.z;

    halfX *= trans.Scale.x;
    halfY *= trans.Scale.y;
    halfZ *= trans.Scale.z;

    m_collisionShape = new btConvexHullShape(btVector3(halfX, halfY, halfZ));

    btVector3 inertia;
    m_collisionShape->calculateLocalInertia(Mass, inertia);

    m_defaultMotionState = new btDefaultMotionState(btTrans);
    btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState, m_collisionShape, inertia);
    m_rigidBody = new btRigidBody(rbci);

    m_world->AddCollider(this);
}

}
