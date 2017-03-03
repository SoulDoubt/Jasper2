#include "PhysicsCollider.h"
#include "GameObject.h"
#include "imgui.h"
#include "AssetSerializer.h"
#include "Mesh.h"


namespace Jasper
{

using namespace std;

PhysicsCollider::PhysicsCollider(std::string name, const Mesh* mesh, PhysicsWorld* world)
    :Component(std::move(name)), m_world(world), m_mesh(mesh)
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
        auto tr = GetGameObject()->GetLocalTransform().AsBtTransform();
        m_rigidBody->setWorldTransform(tr);
        m_defaultMotionState->setWorldTransform(tr);
        btVector3 inertia;
        m_collisionShape->calculateLocalInertia(Mass, inertia);
        m_rigidBody->setMassProps(Mass, inertia);
        m_rigidBody->setRestitution(Restitution);
        m_rigidBody->setFriction(Friction);
        m_world->AddRigidBody(m_rigidBody.get());
        m_rigidBody->activate();
    } else {
        m_world->RemoveRigidBody(m_rigidBody.get());
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

    if (ImGui::InputFloat("Mass", &Mass)) {
        this->m_rigidBody->setMassProps(Mass, btVector3(0,0,0));
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
        } else {
            btTrans = btTransform::getIdentity();
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
    } else {
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
    } else {
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
    btRigidBody::btRigidBodyConstructionInfo rbci(Mass, m_defaultMotionState.get(), m_collisionShape.get(), trans.Position.AsBtVector3() );
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


} // namespace Jasper
