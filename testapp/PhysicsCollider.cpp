#include "PhysicsCollider.h"
#include "GameObject.h"
#include "imgui.h"
#include "AssetSerializer.h"
#include "Mesh.h"


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
	float dr = ((float)rand() / (RAND_MAX));
	float dg = ((float)rand() / (RAND_MAX));
	float db = ((float)rand() / (RAND_MAX));
	debugColor = Vector4(dr, dg, db, 1.f);        
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
	else {
		//m_rigidBody->setAc();
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


CompoundCollider::CompoundCollider(std::string name, std::vector<std::unique_ptr<btConvexHullShape>>& hulls, PhysicsWorld * world)
	: PhysicsCollider(name, { 0.f, 0.f, 0.f }, world), m_hulls()
{
	for (auto& hull : hulls) {
		m_hulls.emplace_back(move(hull));
	}
}

void CompoundCollider::Awake()
{
	btCompoundShape* compound = new btCompoundShape(true, m_hulls.size());
	const auto& trans = GetGameObject()->GetLocalTransform();
	auto btTrans = trans.GetBtTransform();
	
	for (const auto& hull : m_hulls) {		
		compound->addChildShape(btTrans, hull.get());
	}

	m_collisionShape = compound;
	btVector3 inertia;
	m_collisionShape->setLocalScaling(trans.Scale.AsBtVector3());
	m_collisionShape->calculateLocalInertia(Mass, inertia);
	m_defaultMotionState = new btDefaultMotionState(btTrans);
	btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState, m_collisionShape, inertia);
	m_rigidBody = new btRigidBody(rbci);

	m_world->AddCollider(this);

}

} // namespace Jasper
