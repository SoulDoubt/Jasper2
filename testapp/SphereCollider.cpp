#include "SphereCollider.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Sphere.h"

namespace Jasper
{

//SphereCollider::SphereCollider(std::string name, Mesh * mesh, PhysicsWorld * world)
//    :PhysicsCollider(name, mesh, world)
//{
//    m_colliderType = PHYSICS_COLLIDER_TYPE::Sphere;
//}

using namespace std;

SphereCollider::SphereCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world)
    :PhysicsCollider(name, halfExtents, world)
{
    m_colliderType = PHYSICS_COLLIDER_TYPE::Sphere;
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
    m_collisionShape = make_unique<btSphereShape>(radius);

    btVector3 inertia;
    m_collisionShape->calculateLocalInertia(Mass, inertia);

    m_defaultMotionState = make_unique<btDefaultMotionState>(btTrans);
    btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState.get(), m_collisionShape.get(), inertia);
    m_rigidBody = make_unique<btRigidBody>(rbci);
    m_rigidBody->setRestitution(Restitution);
    m_rigidBody->setFriction(Friction);
    m_rigidBody->setUserPointer(GetGameObject());
    m_world->AddCollider(this);
}

} // namespace Jasper
