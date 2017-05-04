#include "PhysicsCollider.h"
#include "GameObject.h"
#include "imgui.h"
#include "AssetSerializer.h"
#include "Mesh.h"
#include "AnimationSystem.h"
#include <StringFunctions.h>


namespace Jasper
{

using namespace std;

PhysicsCollider::PhysicsCollider(const std::string & name, PhysicsWorld * world)
	:Component(name), m_world(world)
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::None;
}

PhysicsCollider::PhysicsCollider(std::string name, const Mesh* mesh, PhysicsWorld* world)
	: Component(std::move(name)), m_world(world), m_mesh(mesh)
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::None;
}

PhysicsCollider::PhysicsCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world)
	: Component(std::move(name)), m_world(world), m_halfExtents(halfExtents)
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::None;
	m_debugColor = Vector4(1.f, 0.f, 0.f, 0.9f);
}

PhysicsCollider::PhysicsCollider(std::string name, std::unique_ptr<btCollisionShape> shape, PhysicsWorld* world)
	: Component(std::move(name)), m_world(world)//, m_halfExtents(halfExtents)
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::ConvexHull;
	m_collisionShape = std::move(shape);
}


PhysicsCollider::~PhysicsCollider()
{

}

void PhysicsCollider::Initialize()
{


}

void PhysicsCollider::Destroy()
{
	this->GetPhysicsWorld()->RemoveRigidBody(m_rigidBody.get());
	if (m_rigidBody != nullptr) {
		// delete m_defaultMotionState;
	}

}

void PhysicsCollider::Awake()
{
	// if constructed from an existing btCollision Shape...
	if (m_collisionShape.get() != nullptr) {
		const auto& trans = GetGameObject()->GetLocalTransform();
		auto btTrans = trans.AsBtTransform();
		btVector3 inertia;
		m_collisionShape->setLocalScaling(trans.Scale.AsBtVector3());
		m_collisionShape->calculateLocalInertia(Mass, inertia);
		m_defaultMotionState = make_unique<btDefaultMotionState>(btTrans);
		btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState.get(), m_collisionShape.get(), inertia);
		m_rigidBody = make_unique<btRigidBody>(rbci);
		m_rigidBody->setUserPointer(GetGameObject());
		m_world->AddCollider(this);
	}
}

void PhysicsCollider::Start()
{
	//Component::Start();
}

void PhysicsCollider::FixedUpdate()
{

}

void PhysicsCollider::Update(double dt)
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
		auto tr = GetGameObject()->GetWorldTransform().AsBtTransform();
		m_rigidBody->setWorldTransform(tr);
		m_defaultMotionState->setWorldTransform(tr);
		btVector3 inertia;
		m_collisionShape->calculateLocalInertia(Mass, inertia);
		m_rigidBody->setMassProps(Mass, inertia);
		m_rigidBody->setRestitution(Restitution);
		m_rigidBody->setFriction(Friction);
		m_world->AddRigidBody(m_rigidBody.get());
		//m_rigidBody->setActivationState(ACTIVE_TAG);
		m_rigidBody->activate();
	}
	else {
		m_world->RemoveRigidBody(m_rigidBody.get());
		//if (m_rigidBody) {
		//	m_rigidBody->setActivationState(DISABLE_SIMULATION);
			//m_rigidBody->
		//}
	}
	Component::ToggleEnabled(e);
}

void PhysicsCollider::SetScale(const Vector3 & scale)
{
	m_collisionShape->setLocalScaling(scale.AsBtVector3());
	m_world->GetBtWorld()->updateSingleAabb(m_rigidBody.get());
}

bool PhysicsCollider::ShowGui()
{
	Component::ShowGui();

	auto& tt = m_rigidBody->getWorldTransform();
	Vector3 pp(tt.getOrigin());
	ImGui::InputFloat3("Position", pp.AsFloatPtr());

	if (ImGui::InputFloat("Mass", &Mass)) {
		btVector3 inertia;
		m_collisionShape->calculateLocalInertia(Mass, inertia);
		this->m_rigidBody->setMassProps(Mass, inertia);
	}
	if (ImGui::InputFloat("Restitution", &Restitution)) {
		this->m_rigidBody->setRestitution(Restitution);
	}
	if (ImGui::InputFloat("Friction", &Friction)) {
		this->m_rigidBody->setFriction(Friction);
	}

	return false;
}

void PhysicsCollider::Serialize(std::ofstream& ofs) const
{
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

// ----------------- Compound Collider -----------------------//

CompoundCollider::CompoundCollider(std::string name, std::vector<std::unique_ptr<btCollisionShape>>& hulls, PhysicsWorld * world)
	: PhysicsCollider(name, { 0.f, 0.f, 0.f }, world), m_hulls()
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::Compound;
	for (auto& hull : hulls) {
		m_hulls.emplace_back(move(hull));
	}
}

void CompoundCollider::Awake()
{
	unique_ptr<btCompoundShape> compound = make_unique<btCompoundShape>(true, m_hulls.size());
	const auto& trans = GetGameObject()->GetLocalTransform();
	btTransform btTrans = btTransform::getIdentity();
	int i = 0;
	for (const auto& hull : m_hulls) {
		if (i == 0) {
			compound->addChildShape(btTrans, hull.get());
		}
		else {
			//btTrans = btTransform::getIdentity();
			compound->addChildShape(btTrans, hull.get());
		}
	}
	btTransform goTrans = GetGameObject()->GetLocalTransform().AsBtTransform();
	m_collisionShape = move(compound);
	btVector3 inertia;
	m_collisionShape->setLocalScaling(trans.Scale.AsBtVector3());
	m_collisionShape->calculateLocalInertia(Mass, inertia);
	m_defaultMotionState = make_unique<btDefaultMotionState>(goTrans);
	btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState.get(), m_collisionShape.get(), inertia);
	m_rigidBody = make_unique<btRigidBody>(rbci);
	m_rigidBody->setUserPointer(GetGameObject());
	m_world->AddCollider(this);

}

// ------------------------ Box Collider --------------------//
BoxCollider::BoxCollider(std::string name, const Mesh* mesh, PhysicsWorld* world)
	:PhysicsCollider(name, mesh, world)
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::Box;

}


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
	auto btTrans = trans.AsBtTransform();

	float halfX, halfY, halfZ;
	if (m_mesh) {
		Vector3 halfExtents = m_mesh->HalfExtents();
		halfX = halfExtents.x;
		halfY = halfExtents.y;
		halfZ = halfExtents.z;
	}
	else {
		halfX = m_halfExtents.x;
		halfY = m_halfExtents.y;
		halfZ = m_halfExtents.z;
	}

	m_collisionShape = make_unique<btBoxShape>(btVector3(halfX, halfY, halfZ));
	m_collisionShape->setLocalScaling(trans.Scale.AsBtVector3());

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


// ------------------- CapsuleCollider -----------------------//
CapsuleCollider::CapsuleCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world)
	: PhysicsCollider(name, halfExtents, world)
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::Capsule;
}

CapsuleCollider::CapsuleCollider(std::string name, const Mesh* mesh, PhysicsWorld* world)
	: PhysicsCollider(name, mesh, world)
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
		Vector3 minExtents = m_mesh->MinExtents();
		Vector3 maxExtents = m_mesh->MaxExtents();
		halfX = (maxExtents.x - minExtents.x) / 2.0f;
		halfY = (maxExtents.y - minExtents.y) / 2.0f;
		halfZ = (maxExtents.z - minExtents.z) / 2.0f;
	}
	else {
		halfX = m_halfExtents.x;
		halfY = m_halfExtents.y;
		halfZ = m_halfExtents.z;
	}

	float radius = std::max(halfX, halfZ);
	if (radius == halfX) {
		radius *= trans.Scale.x;
	}
	else {
		radius *= trans.Scale.y;
	}

	float height = (halfY)* trans.Scale.y * 2;
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

// ------------------- Cylinder Collider -------------------//

CylinderCollider::CylinderCollider(const std::string& name, const Vector3& halfExtents, PhysicsWorld* world)
	: PhysicsCollider(name, halfExtents, world)
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::Cylinder;
}

CylinderCollider::CylinderCollider(const std::string& name, const Mesh* mesh, PhysicsWorld* world)
	: PhysicsCollider(name, mesh, world)
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::Cylinder;
}

void CylinderCollider::Awake()
{

	auto go = GetGameObject();
	auto& trans = go->GetLocalTransform();
	auto btTrans = trans.AsBtTransform();
	float halfX, halfY, halfZ;

	if (m_mesh != nullptr) {
		//Vector3 halfExtents = m_mesh->GetHalfExtents();
		Vector3 minExtents = m_mesh->MinExtents();
		Vector3 maxExtents = m_mesh->MaxExtents();
		halfX = (maxExtents.x - minExtents.x) / 2.0f;
		halfY = (maxExtents.y - minExtents.y) / 2.0f;
		halfZ = (maxExtents.z - minExtents.z) / 2.0f;
	}
	else {
		halfX = m_halfExtents.x;
		halfY = m_halfExtents.y;
		halfZ = m_halfExtents.z;
	}

	m_collisionShape = make_unique<btCylinderShape>(btVector3(halfX, halfY, halfZ));

	btVector3 inertia;
	m_collisionShape->calculateLocalInertia(Mass, inertia);

	m_defaultMotionState = make_unique<btDefaultMotionState>(btTrans);
	btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState.get(), m_collisionShape.get(), inertia);
	m_rigidBody = make_unique<btRigidBody>(rbci);
	m_rigidBody->setUserPointer(GetGameObject());
	m_world->AddCollider(this);
}

// -------------------- ConvexHull Collider --------------------//

ConvexHullCollider::ConvexHullCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world)
	: PhysicsCollider(name, halfExtents, world), m_meshes()
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::ConvexHull;
}

ConvexHullCollider::ConvexHullCollider(std::string name, const Mesh* mesh, PhysicsWorld* world)
	: PhysicsCollider(name, mesh, world), m_meshes()
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::ConvexHull;
}

void ConvexHullCollider::Awake()
{

	auto go = GetGameObject();
	auto& trans = go->GetLocalTransform();
	auto btTrans = trans.AsBtTransform();


	m_btm = std::make_unique<btTriangleMesh>(true, false);

	for (const auto mesh : m_meshes) {
		int sz = mesh->Indices.size();
		for (int i = 0; i < sz; i += 3) {
			const unsigned index0 = mesh->Indices[i];
			const unsigned index1 = mesh->Indices[i + 1];
			const unsigned index2 = mesh->Indices[i + 2];
			btVector3 p0 = mesh->Positions[index0].AsBtVector3();
			btVector3 p1 = mesh->Positions[index1].AsBtVector3();
			btVector3 p2 = mesh->Positions[index2].AsBtVector3();

			m_btm->addTriangle(p0, p1, p2, false);
		}
	}
	std::unique_ptr<btBvhTriangleMeshShape> bvh = make_unique<btBvhTriangleMeshShape>(m_btm.get(), true);
	bvh->buildOptimizedBvh();
	//bvh->usesQuantizedAabbCompression();
	//btShapeHull* shull = new btShapeHull(bcs);
	//shull->buildHull(bcs->getMargin());
	//btConvexHullShape* shape = new btConvexHullShape((btScalar*)shull->getVertexPointer(), shull->numVertices());
	m_collisionShape = move(bvh);

	btVector3 inertia;
	Mass = 0.0f;
	//m_collisionShape->calculateLocalInertia(Mass, inertia);

	m_defaultMotionState = make_unique<btDefaultMotionState>(btTrans);
	btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState.get(), m_collisionShape.get(), inertia);
	m_rigidBody = make_unique<btRigidBody>(rbci);
	m_rigidBody->setUserPointer(GetGameObject());
	m_collisionShape->setLocalScaling(trans.Scale.AsBtVector3());

	m_world->AddCollider(this);
	//delete shull;
	//delete bcs;
	//delete btm;
}

void ConvexHullCollider::Initialize()
{

}

void ConvexHullCollider::InitFromMeshes(const std::vector<Mesh*>& meshes)
{
	for (const auto m : meshes) {
		m_meshes.push_back(m);
	}
}

// ------------------- PlaneCollider ---------------------------------//


PlaneCollider::PlaneCollider(std::string name, Vector3 normal, float constant, PhysicsWorld * world) :
	PhysicsCollider(name, Vector3(), world), Normal(normal), Constant(constant)
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::Plane;
}

PlaneCollider::~PlaneCollider()
{
}

void PlaneCollider::Serialize(std::ofstream & ofs) const
{
	using namespace AssetSerializer;
	PhysicsCollider::Serialize(ofs);
	ofs.write(ConstCharPtr(Normal.AsFloatPtr()), sizeof(Normal));
	ofs.write(ConstCharPtr(&Constant), sizeof(Constant));
}

void PlaneCollider::Awake()
{
	auto go = GetGameObject();
	auto& trans = go->GetLocalTransform();
	auto btTrans = trans.AsBtTransform();
	m_collisionShape = make_unique<btStaticPlaneShape>(Normal.AsBtVector3(), Constant);
	m_defaultMotionState = make_unique<btDefaultMotionState>(btTrans);
	btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState.get(), m_collisionShape.get(), trans.Position.AsBtVector3());
	m_rigidBody = make_unique<btRigidBody>(rbci);
	m_rigidBody->setRestitution(Restitution);
	m_rigidBody->setFriction(Friction);
	m_rigidBody->setUserPointer(GetGameObject());
	m_world->AddCollider(this);
}

// -------------------- SphereCollider ----------------------//

SphereCollider::SphereCollider(std::string name, Mesh * mesh, PhysicsWorld * world)
	:PhysicsCollider(name, mesh, world)
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::Sphere;
}


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
	auto btTrans = trans.AsBtTransform();
	float radius = 0.f;
	if (m_mesh) {
		m_halfExtents = m_mesh->HalfExtents();
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


// --------------- Static Triangle Mesh ------------------//

StaticTriangleMeshCollider::StaticTriangleMeshCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world)
	: PhysicsCollider(name, halfExtents, world), m_meshes()
{
	m_colliderType = PHYSICS_COLLIDER_TYPE::StaticTriangleMesh;
}

void StaticTriangleMeshCollider::Awake()
{

	auto go = GetGameObject();
	auto& trans = go->GetLocalTransform();
	auto btTrans = trans.AsBtTransform();


	m_btm = std::make_unique<btTriangleMesh>(true, false);

	for (const auto mesh : m_meshes) {
		int sz = mesh->Indices.size();
		for (int i = 0; i < sz; i += 3) {
			const unsigned index0 = mesh->Indices[i];
			const unsigned index1 = mesh->Indices[i + 1];
			const unsigned index2 = mesh->Indices[i + 2];
			btVector3 p0 = mesh->Positions[index0].AsBtVector3();
			btVector3 p1 = mesh->Positions[index1].AsBtVector3();
			btVector3 p2 = mesh->Positions[index2].AsBtVector3();

			m_btm->addTriangle(p0, p1, p2, false);
		}
	}
	std::unique_ptr<btBvhTriangleMeshShape> bvh = make_unique<btBvhTriangleMeshShape>(m_btm.get(), true);
	bvh->buildOptimizedBvh();
	//bvh->usesQuantizedAabbCompression();
	//btShapeHull* shull = new btShapeHull(bcs);
	//shull->buildHull(bcs->getMargin());
	//btConvexHullShape* shape = new btConvexHullShape((btScalar*)shull->getVertexPointer(), shull->numVertices());
	m_collisionShape = move(bvh);

	btVector3 inertia;
	Mass = 0.0f;
	//m_collisionShape->calculateLocalInertia(Mass, inertia);

	m_defaultMotionState = make_unique<btDefaultMotionState>(btTrans);
	btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState.get(), m_collisionShape.get(), inertia);
	m_rigidBody = make_unique<btRigidBody>(rbci);
	m_rigidBody->setUserPointer(GetGameObject());
	m_collisionShape->setLocalScaling(trans.Scale.AsBtVector3());

	m_world->AddCollider(this);
	//delete shull;
	//delete bcs;
	//delete btm;
}

void StaticTriangleMeshCollider::Initialize()
{

}

void StaticTriangleMeshCollider::InitFromMeshes(const std::vector<Mesh*>& meshes)
{
	for (const auto m : meshes) {
		m_meshes.push_back(m);
	}
}


GhostCollider::GhostCollider(std::string name, const Vector3 & halfExtents, PhysicsWorld * world)
	: PhysicsCollider(name, halfExtents, world)
{

}

void GhostCollider::Awake()
{
	auto go = GetGameObject();
	auto& trans = go->GetLocalTransform();
	auto btTrans = trans.AsBtTransform();

	float halfX, halfY, halfZ;
	if (m_mesh != nullptr) {
		//Vector3 halfExtents = m_mesh->GetHalfExtents();
		Vector3 minExtents = m_mesh->MinExtents();
		Vector3 maxExtents = m_mesh->MaxExtents();
		halfX = (maxExtents.x - minExtents.x) / 2.0f;
		halfY = (maxExtents.y - minExtents.y) / 2.0f;
		halfZ = (maxExtents.z - minExtents.z) / 2.0f;
	}
	else {
		halfX = m_halfExtents.x;
		halfY = m_halfExtents.y;
		halfZ = m_halfExtents.z;
	}

	float radius = std::max(halfX, halfZ);
	if (radius == halfX) {
		radius *= trans.Scale.x;
	}
	else {
		radius *= trans.Scale.y;
	}

	float height = (halfY)* trans.Scale.y * 2;
	height = height - (2 * radius);

	m_collisionShape = make_unique<btCapsuleShape>(radius, height);

	m_ghostObject = make_unique<btPairCachingGhostObject>();

}

//RagdollCollider::RagdollCollider(const std::string & name, ShapeList & hulls, PhysicsWorld* world)
//	:PhysicsCollider(name, Vector3(0.f, 0.f, 0.f), world)
//{
//
//	for (auto& hull : hulls) {
//		m_hulls.pu(move(hull));
//	}
//
//}




RagdollCollider::RagdollCollider(const std::string & name, Skeleton* skeleton, PhysicsWorld * world)
	:PhysicsCollider(name, world), m_ragdoll(name, skeleton, world)
{
}

void RagdollCollider::Awake()
{
	m_ragdoll.Initialize();

	//auto worldTransform = GetGameObject()->GetWorldTransform();

	////vector<unique_ptr<RagdollCapsuleShape>> tempShapes;
	//
	//for (const auto& hullpair : m_hulls) {
	//	auto& name = hullpair.first;
	//	auto& hull = hullpair.second;		

	//	auto rcap = static_cast<RagdollCapsuleShape*>(hull.get());
	//	auto bone = static_cast<BoneData*>(hull->getUserPointer());
	//	if (bone) {		
	//		// activate physics on the sksleton
	//		if (!bone->skeleton->IsPhysicsControlled()) {
	//			//bone->skeleton->SetPhysicsControl(true);
	//		}

	//		auto names = split(name, ':');

	//		if (rcap->ParentBone() == nullptr) {
	//			Transform trans = rcap->ChildBone()->GetWorldTransform();
	//			trans = worldTransform * trans;

	//			unique_ptr<btDefaultMotionState> ms = make_unique<btDefaultMotionState>(trans.AsBtTransform());
	//			float mass = 10.f;
	//			m_masses[name] = mass;
	//			btVector3 inertia;
	//			hull->calculateLocalInertia(mass, inertia);
	//			btRigidBody::btRigidBodyConstructionInfo rbci(mass, ms.get(), hull.get(), inertia);
	//			unique_ptr<btRigidBody> rb = make_unique<btRigidBody>(rbci);
	//			rcap->SetRigidBody(rb.get());
	//			rcap->SetMotionState(ms.get());
	//			rb->setUserPointer(GetGameObject());				
	//			rb->setDamping(0.05, 0.85);
	//			rb->setDeactivationTime(0.8);
	//			rb->setSleepingThresholds(1.6, 2.5);
	//			m_bodies[name] = move(rb);
	//			m_motionStates[name] = move(ms);

	//		}
	//		else {
	//			auto parentBone = rcap->ParentBone();
	//			auto childBone = rcap->ChildBone();
	//			Transform parentBoneTransform = parentBone->GetWorldTransform();
	//			Transform childBoneTransform = childBone->GetWorldTransform();
	//			Vector3 midpoint = (parentBoneTransform.Position + childBoneTransform.Position) / 2.f;
	//			
	//			Transform trans;
	//			//trans = childBone->GetSkinningTransform().Inverted();
	//			trans.Position = midpoint;

	//			// the default btcapsuleshape is oriented in the world y axis
	//			// we  need to ratate it to line up with the bone
	//			Vector3 world_y = { 0.f, 1.f, 0.f };
	//			
	//			Vector3 bone_axis = Normalize(childBoneTransform.Position - parentBoneTransform.Position);
	//			rcap->SetAxisOfAlignment(bone_axis);
	//			// get's the axis of rotation from world_y to the bone vector
	//			Vector3 cro = Cross(world_y, bone_axis);
	//			// get the magnitude of the rotation
	//			float dt = Dot(world_y, bone_axis);
	//			dt = acos(dt);
	//			Quaternion q = Quaternion::FromAxisAndAngle(cro, dt);
	//			trans.Orientation = q;
	//			rcap->SetOrientation(q);
	//			trans = GetGameObject()->GetWorldTransform() * trans;
	//			
	//			unique_ptr<btDefaultMotionState> ms = make_unique<btDefaultMotionState>(trans.AsBtTransform());
	//			
	//			float mass = 5.f;
	//			m_masses[name] = mass;
	//			btVector3 inertia;
	//			hull->calculateLocalInertia(mass, inertia);
	//			btRigidBody::btRigidBodyConstructionInfo rbci(mass, ms.get(), hull.get(), inertia);
	//			unique_ptr<btRigidBody> rb = make_unique<btRigidBody>(rbci);
	//			rb->setUserPointer(GetGameObject());				
	//			rcap->SetRigidBody(rb.get());
	//			rcap->SetMotionState(ms.get());
	//			m_bodies[name] = move(rb);
	//			m_motionStates[name] = move(ms);
	//			
	//			int x = 0;
	//		}
	//	}

	//}

	//for (auto& hp : m_hulls) {
	//	if (!FindInString("joint_test", hp.first)) {
	//		const string name = hp.first;
	//		auto& shape = hp.second;

	//		auto sh = static_cast<RagdollCapsuleShape*>(shape.get());
	//		if (sh->ParentBone() == nullptr) {
	//			// this is the root bone in the skeleton
	//			continue;
	//		}

	//		if (sh->ParentBone()->m_collisionShape == nullptr) {
	//			// this is the root node in the skeleton
	//			// basically do nothing....
	//			continue;

	//		}

	//		auto pbone = sh->ParentBone();
	//		auto cbone = sh->ChildBone();

	//		assert(pbone && cbone);

	//		auto& prb = m_bodies[pbone->Name];
	//		auto& crb = m_bodies[cbone->Name];

	//		assert(prb && crb);

	//		auto& pshape = m_hulls[pbone->Name];
	//	
	//		assert(pshape);
	//		// the pivot point of the joint is the position of the parent bone
	//		auto hingeTransform = pbone->GetWorldTransform();
	//		auto hingeTransformWorld = worldTransform * hingeTransform;

	//		auto childBoneTransform = cbone->GetWorldTransform();
	//		auto childBoneTransformWorld = worldTransform * childBoneTransform;

	//		btTransform pt, ct;
	//		prb->getMotionState()->getWorldTransform(pt);
	//		crb->getMotionState()->getWorldTransform(ct);
	//		
	//		auto ppo = Vector3(pt.inverse() * hingeTransformWorld.Position.AsBtVector3());
	//		pbone->ShapeOffset = ppo;
	//		auto cco = Vector3(ct.inverse() * hingeTransformWorld.Position.AsBtVector3());

	//		auto prot = Quaternion(pt.inverse() * hingeTransformWorld.Orientation.AsBtQuaternion());
	//		auto crot = Quaternion(ct.inverse() * childBoneTransformWorld.Orientation.AsBtQuaternion());

	//		auto pshaperc = static_cast<RagdollCapsuleShape*>(pshape.get());
	//		auto cshaperc = static_cast<RagdollCapsuleShape*>(shape.get());

	//		assert(pshaperc && cshaperc);

	//		auto paxis = pshaperc->AxisOfAlignment();
	//		auto caxis = cshaperc->AxisOfAlignment();
	//		auto phingeAxis = pshaperc->GetOrientation();
	//		auto chingeAxis = cshaperc->GetOrientation();

	//	//	Vector3 world_x = { 0, 1, 0 };
	//	//	Vector3 pcro = Cross(world_x, paxis);
	//	//	float pdt = Dot(world_x, paxis);
	//	//	pdt = acos(pdt);

	//	//	Quaternion pq = Quaternion::FromAxisAndAngle(pcro, pdt);
	//	//	auto bpq = pq.AsBtQuaternion();

	//	//	Vector3 ccro = Cross(world_x, caxis);
	//	//	float cdt = Dot(world_x, caxis);
	//	//	cdt = acos(cdt);

	//	//	Quaternion cq = Quaternion::FromAxisAndAngle(ccro, cdt);
	//	//	auto bcq = cq.AsBtQuaternion();

	//		pt.setOrigin(ppo.AsBtVector3());
	//	//	//pt.setRotation(prot.AsBtQuaternion());

	//	//	pt.getBasis().setEulerZYX(pcro.x, pcro.y, pcro.z);
	//	//	//pt.getBasis().setEulerZYX(ccro.z, ccro.y, ccro.x);
	//		ct.setOrigin(cco.AsBtVector3());
	//	//	//ct.setRotation(crot.AsBtQuaternion());
	//	//	ct.getBasis().setEulerZYX(ccro.x, ccro.y, ccro.z);




	//	//	/* -------------------------------
	//	//		Some Test bodies
	//	//	*/
	//	///*	auto cbody = make_unique<btSphereShape>(0.025f);
	//	//	Transform cbodyt;
	//	//	cbodyt.Position = hingeTransformWorld.Position;
	//	//	unique_ptr<btDefaultMotionState> cbodyms = make_unique<btDefaultMotionState>(cbodyt.AsBtTransform());
	//	//	float mass = 0.f;
	//	//	btVector3 inertia;
	//	//	cbody->calculateLocalInertia(mass, inertia);
	//	//	btRigidBody::btRigidBodyConstructionInfo rbci(mass, cbodyms.get(), cbody.get(), inertia);
	//	//	unique_ptr<btRigidBody> cbodyrb = make_unique<btRigidBody>(rbci);
	//	//	cbodyrb->setUserPointer(GetGameObject());


	//	//	m_bodies[name + "joint_test"] = move(cbodyrb);
	//	//	m_motionStates[name + "joint_test"] = move(cbodyms);
	//	//	m_hulls[name + "joint_test"] = move(cbody);*/
	//	//	//m_world->AddRigidBody(cbodyrb.get());
	//	//	//auto cbody = make_unique<btSphereShape>()
	//	//	//-----------------------------------------------------
	//	//	//-----------------------------------------------------

	//	//	//auto& prb = m_bodies[pbone->Name];
	//	//	//auto& crb = m_bodies[cbone->Name];

	//	//	//btTransform pti, cti;
	//	//	//prb->getMotionState()->getWorldTransform(pti);// .inverse();
	//	//	//crb->getMotionState()->getWorldTransform(cti);// .inverse();

	//	//	//pti = pti.inverse();
	//	//	//cti = cti.inverse();

	//	//	// now we need to multiply the world space position of the joint (parent)
	//	//	//Vector3 worldHingePoint = worldTransform * pbone->GetWorldTransform().Position;


	//	//	//worldHingePoint = pti * worldHingePoint;

	//	//	//Vector3 poffset, coffset;
	//	//	//poffset = Transform(pt);// *worldHingePoint;// *Transform(pti);
	//	//	//coffset = Transform(ct);// *worldHingePoint;
	//	//	//pti.setOrigin(poffset.AsBtVector3());
	//	//	//cti.setOrigin(coffset.AsBtVector3());
	//	//	//pti.setOrigin(hingePosition.AsBtVector3());
	//		pt.getBasis().setEulerZYX(0, -1, 0);
	//		ct.getBasis().setEulerYPR(0, -1, 0);
	//		auto constraint = make_unique<btConeTwistConstraint>(*(prb.get()), *(crb.get()), pt, ct);
	//		constraint->setLimit(M_PI_4, M_PI_4, M_PI_4);
	//		constraint->setDbgDrawSize(0.1);
	//		
	//		m_constraints[name] = move(constraint);
	//	}
	//}
	//m_world->AddRagdoll(this);
	ToggleEnabled(false);
}

void RagdollCollider::Update(double dt)
{
	//for (auto& body : m_bodies) {
	//	if (body.second->getUserPointer()) {
	//		auto gameObjectWorldTransform = GetGameObject()->GetWorldTransform();
	//		auto bone = static_cast<BoneData*>(body.second->getCollisionShape()->getUserPointer());
	//		if (bone) {
	//			btTransform wt;
	//			body.second->getMotionState()->getWorldTransform(wt);
	//			Transform bt = wt;
	//			Vector3 position = bt * bone->ShapeOffset;
	//			Quaternion invBind = Inverse(bone->BindTransform.Orientation);
	//			Quaternion diff =invBind * bt.Orientation;
	//			Quaternion rot = bone->BindTransform.Orientation * diff;

	//			Transform ft = Transform(position, Quaternion(), bone->BindTransform.Scale);
	//			bone->SetWorldTransform(ft);
	//			//Vector3 pos = bt * 
	//			//bone->SetWorldTransform(bt);
	//			
	//			
	//		}
	//	}		//		btTransform bt;
	//	//		body->getMotionState()->getWorldTransform(bt);
	//	//		auto trans = Transform(bt);
	//	//		Vector3 xp = trans.Position;// -bone->skeleton->RootBone->NodeTransform.Position;
	//	//		bone->NodeTransform.Position = xp;
	//	//	}
	//	//}
	//}
}

void RagdollCollider::Destroy()
{
	/*for (auto& body : m_bodies) {
		m_world->RemoveRigidBody(body.second.get());
	}
	for (auto& c : m_constraints) {
		m_world->RemoveConstraint(c.second.get());
	}*/
}

void RagdollCollider::ToggleEnabled(bool e)
{
	if (e) {
		/*for (auto& conPair : m_constraints) {
			auto& con = conPair.second;
			m_world->AddConstraint(con.get());
		}*/
		//for (auto& rbodyPair : m_bodies) {
		//	const string& name = rbodyPair.first;
		//	auto& rb = rbodyPair.second;
		//	auto& ms = m_motionStates[name];
		//	auto& sh = m_hulls[name];

		//	float mass = m_masses[name];
		//	btVector3 inertia;
		//	sh->calculateLocalInertia(mass, inertia);
		//	rb->setMassProps(mass, inertia);
		//	
		//	
		//	//btTransform tr;
		//	//ms->getWorldTransform(tr);;
		//	//rb->setWorldTransform(tr);
		//	//ms->setWorldTransform(tr);
		//	//btVector3 inertia;
		//	////sh->calculateLocalInertia(Mass, inertia);
		//	////rb->setMassProps(Mass, inertia);
		//	////rb->setRestitution(Restitution);
		//	////rb->setFriction(Friction);
		//	//m_world->AddRigidBody(rb.get());
		//	//m_rigidBody->setActivationState(ACTIVE_TAG);
		//	rb->activate();
		//}
	}
	else {
		/*for (auto& conPair : m_constraints) {
			auto& con = conPair.second;
			m_world->RemoveConstraint(con.get());
		}*/
		//for (auto& rbodyPair : m_bodies) {
		//	auto& rb = rbodyPair.second;
		//	rb->setMassProps(0, btVector3());
		//	//m_world->RemoveRigidBody(rb.get());
		//}
		//if (m_rigidBody) {
		//	m_rigidBody->setActivationState(DISABLE_SIMULATION);
		//m_rigidBody->
		//}
	}
	Component::ToggleEnabled(e);
}

bool RagdollCollider::ShowGui()
{
	Component::ShowGui();
	return false;
}

RagdollCapsuleShape::RagdollCapsuleShape(float radius, float height, BoneData * parentBone, BoneData * childBone)
	: btCapsuleShape(radius, height), m_parentBone(parentBone), m_childBone(childBone)
{
	if (parentBone != nullptr) {
		parentBone->UpdateWorldTransform();
	}
	if (childBone != nullptr)
		childBone->UpdateWorldTransform();
}

} // namespace Jasper
