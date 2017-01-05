#include "CylinderCollider.h"
#include "GameObject.h"

namespace Jasper
{
using namespace std;

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

    m_collisionShape = make_unique<btCylinderShape>(btVector3(halfX, halfY, halfZ));

    btVector3 inertia;
    m_collisionShape->calculateLocalInertia(Mass, inertia);

    m_defaultMotionState = make_unique<btDefaultMotionState>(btTrans);
    btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState.get(), m_collisionShape.get(), inertia);
    m_rigidBody = make_unique<btRigidBody>(rbci);
    m_rigidBody->setUserPointer(GetGameObject());
    m_world->AddCollider(this);
}

}
