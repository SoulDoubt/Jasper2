#include "BoxCollider.h"
#include "GameObject.h"
#include "Mesh.h"

namespace Jasper
{

//BoxCollider::BoxCollider(std::string name, Mesh* mesh, PhysicsWorld* world)
//    :PhysicsCollider(name, mesh, world)
//{
//    m_colliderType = PHYSICS_COLLIDER_TYPE::Box;
//
//}

BoxCollider::BoxCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world)
    :PhysicsCollider(name, halfExtents, world)
{
    m_colliderType = PHYSICS_COLLIDER_TYPE::Box;
}


BoxCollider::~BoxCollider()
{
}

void BoxCollider::Awake()
{
    auto go = GetGameObject();
    auto& trans = go->GetLocalTransform();
    auto btTrans = trans.GetBtTransform();

    float halfX, halfY, halfZ;
    if (m_mesh) {
        Vector3 halfExtents = m_mesh->GetHalfExtents();
        halfX = halfExtents.x;
        halfY = halfExtents.y;
        halfZ = halfExtents.z;
    } else {
        halfX = m_halfExtents.x;
        halfY = m_halfExtents.y;
        halfZ = m_halfExtents.z;
    }
    //halfX *= trans.Scale.x;
    //halfY *= trans.Scale.y;
    //halfZ *= trans.Scale.z;

    m_collisionShape = new btBoxShape(btVector3(halfX, halfY, halfZ));
    m_collisionShape->setLocalScaling(trans.Scale.AsBtVector3());

    btVector3 inertia;
    m_collisionShape->calculateLocalInertia(Mass, inertia);

    m_defaultMotionState = new btDefaultMotionState(btTrans);
    btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState, m_collisionShape, inertia);
    m_rigidBody = new btRigidBody(rbci);
    m_rigidBody->setRestitution(Restitution);
    m_rigidBody->setFriction(Friction);
    m_rigidBody->setUserPointer(GetGameObject());
    m_world->AddCollider(this);
}

} // namespace Jasper
