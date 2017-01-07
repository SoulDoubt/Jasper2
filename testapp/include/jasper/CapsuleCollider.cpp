#include "CapsuleCollider.h"
#include "GameObject.h"
#include "Mesh.h"

//Jasper::CapsuleCollider::CapsuleCollider(std::string name, Mesh * mesh, PhysicsWorld * world)
//    : PhysicsCollider(name, mesh, world)
//{
//    m_colliderType = PHYSICS_COLLIDER_TYPE::Capsule;
//}

using namespace std;

namespace Jasper {

CapsuleCollider::CapsuleCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world)
    : PhysicsCollider(name, halfExtents, world)
{
    m_colliderType = PHYSICS_COLLIDER_TYPE::Capsule;
}

CapsuleCollider::~CapsuleCollider()
{
}

void CapsuleCollider::Awake()
{
    auto go = GetGameObject();
    auto& trans = go->GetLocalTransform();
    auto btTrans = trans.AsBtTransform();

    float halfX, halfY, halfZ;
    if (m_mesh != nullptr) {
        //Vector3 halfExtents = m_mesh->GetHalfExtents();
        Vector3 minExtents = m_mesh->GetMinExtents();
        Vector3 maxExtents = m_mesh->GetMaxExtents();
        halfX = (maxExtents.x - minExtents.x) / 2.0f;
        halfY = (maxExtents.y - minExtents.y) / 2.0f;
        halfZ = (maxExtents.z - minExtents.z) / 2.0f;
    } else {
        halfX = m_halfExtents.x;
        halfY = m_halfExtents.y;
        halfZ = m_halfExtents.z;
    }

    float radius = std::max(halfX, halfZ);
    if (radius == halfX) {
        radius *= trans.Scale.x;
    } else {
        radius *= trans.Scale.y;
    }

    float height = (halfY) * trans.Scale.y * 2;
    height = height - (2 * radius);

    m_collisionShape = make_unique<btCapsuleShape>(radius, height);

    btVector3 inertia;
    m_collisionShape->calculateLocalInertia(Mass, inertia);

    m_defaultMotionState = make_unique<btDefaultMotionState>(btTrans);
    btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState.get(), m_collisionShape.get(), inertia);
    m_rigidBody = make_unique<btRigidBody>(rbci);
    m_rigidBody->setUserPointer(GetGameObject());
    m_world->AddCollider(this);
}


}