#include "CylinderCollider.h"
#include "GameObject.h"

namespace Jasper
{

CylinderCollider::CylinderCollider(const std::string& name, const Vector3& halfExtents, PhysicsWorld* world)
    : PhysicsCollider(name, halfExtents, world)
{

}

void CylinderCollider::Awake()
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

    m_collisionShape = new btCylinderShape(btVector3(halfX, halfY, halfZ));

    btVector3 inertia;
    m_collisionShape->calculateLocalInertia(Mass, inertia);

    m_defaultMotionState = new btDefaultMotionState(btTrans);
    btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState, m_collisionShape, inertia);
    m_rigidBody = new btRigidBody(rbci);
    m_rigidBody->setUserPointer(GetGameObject());
    m_world->AddCollider(this);
}

}
