#include "PhysicsCollider.h"
#include "GameObject.h"
#include "imgui.h"
#include "AssetSerializer.h"

namespace Jasper
{

using namespace std;

//PhysicsCollider::PhysicsCollider(std::string name, Mesh* mesh, PhysicsWorld* world)
//    :Component(std::move(name)), m_world(world), m_mesh(mesh)
//{
//    m_colliderType = PHYSICS_COLLIDER_TYPE::None;
//}

PhysicsCollider::PhysicsCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world)
    : Component(std::move(name)), m_world(world), m_halfExtents(halfExtents)
{
    m_colliderType = PHYSICS_COLLIDER_TYPE::None;
}


PhysicsCollider::~PhysicsCollider()
{

}

void PhysicsCollider::Initialize()
{
    

}

void PhysicsCollider::Destroy()
{
    this->GetPhysicsWorld()->RemoveRigidBody(m_rigidBody);
    if (m_rigidBody != nullptr) {
        // delete m_defaultMotionState;
    }

}

void PhysicsCollider::Awake()
{

}

void PhysicsCollider::Start()
{
    //Component::Start();
}

void PhysicsCollider::FixedUpdate()
{

}

void PhysicsCollider::Update(float dt)
{
    if (m_isEnabled) {
        auto go = this->GetGameObject();
        Transform t = this->GetCurrentWorldTransform();
        go->SetLocalTransform(t);
    }
}

void PhysicsCollider::LateUpdate()
{
}

Transform PhysicsCollider::GetCurrentWorldTransform()
{
    Transform t = GetGameObject()->GetWorldTransform();
    btTransform trans;
    m_rigidBody->getMotionState()->getWorldTransform(trans);
    Transform physTransform = Transform(trans);
    physTransform.Scale = t.Scale;
    return physTransform;
}

void PhysicsCollider::ToggleEnabled(bool e)
{    
    if (e) {
        auto tr = GetGameObject()->GetLocalTransform().GetBtTransform();
        m_rigidBody->setWorldTransform(tr);
        m_defaultMotionState->setWorldTransform(tr);
        m_rigidBody->setRestitution(Restitution);
        m_rigidBody->setFriction(Friction);
        m_rigidBody->activate();
    }
    Component::ToggleEnabled(e);
}

void PhysicsCollider::SetScale(const Vector3 & scale)
{
	m_collisionShape->setLocalScaling(scale.AsBtVector3());
	m_world->GetBtWorld()->updateSingleAabb(m_rigidBody);
}

bool PhysicsCollider::ShowGui()
{
    Component::ShowGui();
    
    if (ImGui::InputFloat("Mass", &Mass)) {
        this->m_rigidBody->setMassProps(Mass, btVector3(0,0,0));
    }
    if (ImGui::InputFloat("Restitution", &Restitution)) {
        this->m_rigidBody->setRestitution(Restitution);
    }
    if (ImGui::InputFloat("Friction", &Friction)){
        this->m_rigidBody->setFriction(Friction);
    }
    
    return false;
}

void PhysicsCollider::Serialize(std::ofstream& ofs) const{
    // CompontntType    -> int
    // collider type    -> enum
    // Mass             -> float
    // Restitution      -> float
    // Friction         -> float
    // HalfExtents      -> Vector3
    
    using namespace AssetSerializer;
    
	Component::Serialize(ofs);
    PHYSICS_COLLIDER_TYPE colliderType = GetColliderType();
    ofs.write(ConstCharPtr(&colliderType), sizeof(colliderType));
    ofs.write(ConstCharPtr(&Mass), sizeof(Mass));
    ofs.write(ConstCharPtr(&Restitution), sizeof(Restitution));
    ofs.write(ConstCharPtr(&Friction), sizeof(Friction));
    ofs.write(ConstCharPtr(m_halfExtents.AsFloatPtr()), sizeof(Vector3));
}



} // namespace Jasper
