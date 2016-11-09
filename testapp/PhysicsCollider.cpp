#include "PhysicsCollider.h"
#include "GameObject.h"
#include "imgui.h"

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
//Component::Update();
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
    Component::ToggleEnabled(e);
    if (e) {
        //m_rigidBody->clearForces();
        auto tr = GetGameObject()->GetLocalTransform().GetBtTransform();
        m_rigidBody->setWorldTransform(tr);
        //m_rigidBody->getMotionState()->setWorldTransform(tr);
        m_defaultMotionState->setWorldTransform(tr);
        //btVector3 inertia;
        //m_collisionShape->calculateLocalInertia(this->Mass, inertia);
        m_rigidBody->setRestitution(Restitution);
        m_rigidBody->setFriction(Friction);
        m_rigidBody->activate();
    }
}

bool PhysicsCollider::ShowGui()
{
    using namespace ImGui;
    Component::ShowGui();
    if (InputFloat("Mass", &Mass)) {
        this->m_rigidBody->setMassProps(Mass, btVector3(0,0,0));
    }
    if (InputFloat("Restitution", &Restitution)) {
        this->m_rigidBody->setRestitution(Restitution);
    }
    if (InputFloat("Friction", &Friction)){
        this->m_rigidBody->setFriction(Friction);
    }
    return false;
}



} // namespace Jasper
