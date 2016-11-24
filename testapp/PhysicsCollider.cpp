#include "PhysicsCollider.h"
#include "GameObject.h"
#include "imgui.h"
#include "AssetSerializer.h"

namespace Jasper
{

using namespace std;

PhysicsCollider::PhysicsCollider(const std::string& name, Mesh* mesh, PhysicsWorld* world)
    :Component(name), m_world(world), m_mesh(mesh)
{

}

PhysicsCollider::PhysicsCollider(const std::string& name, const Vector3& halfExtents, PhysicsWorld* world)
    : Component(name), m_world(world), m_halfExtents(halfExtents)
{
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

void PhysicsCollider::Serialize(std::ofstream& ofs){
    // CompontntType    -> int
    // Mass             -> float
    // Restitution      -> float
    // Friction         -> float
    // HalfExtents      -> Vector3
    
    using namespace AssetSerializer;
    
    ComponentType ty = GetComponentType();
    ofs.write(CharPtr(&ty), sizeof(ty));
    ofs.write(CharPtr(&Mass), sizeof(Mass));
    ofs.write(CharPtr(&Restitution), sizeof(Restitution));
    ofs.write(CharPtr(&Friction), sizeof(Friction));
    ofs.write(CharPtr(m_halfExtents.AsFloatPtr()), sizeof(Vector3));
}



} // namespace Jasper
