#pragma once


#include "Component.h"
#include "PhysicsWorld.h"
#include "Transform.h"
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>



namespace Jasper
{

class Mesh;

enum class PHYSICS_COLLIDER_TYPE
{
    None,
    Box,
    Capsule,
    ConvexHull,
    Cylinder,
    Plane,
    Sphere,
    Compound,
    StaticTriangleMesh,
	Ghost,
	Ragdoll
};


class PhysicsCollider :	public Component
{
public:

    const static unsigned DRAW_COLLISION_SHAPE = 0x01;

	explicit PhysicsCollider(const std::string& name, PhysicsWorld* world);
    explicit PhysicsCollider(std::string name, const Mesh* mesh, PhysicsWorld* world);
    explicit PhysicsCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world);
    explicit PhysicsCollider(std::string name, std::unique_ptr<btCollisionShape> shape, PhysicsWorld* world);
    virtual ~PhysicsCollider();

    ComponentType GetComponentType() const override final {
        return ComponentType::PhysicsCollider;
    }

    void Initialize() override;
    void Destroy() override;
    void Awake() override;
    void Start() override;
    void FixedUpdate() override;
    void Update(double dt) override;
    void LateUpdate() override;
    bool ShowGui() override;
    void Serialize(std::ofstream& ofs) const;

    Transform GetCurrentWorldTransform();

    void ToggleEnabled(bool enabled) override;

    PhysicsWorld* GetPhysicsWorld() const {
        return m_world;
    }
    btRigidBody* GetRigidBody() const {
        return m_rigidBody.get();
    }

    btCollisionShape* GetCollisionShape() const {
        return m_collisionShape.get();
    }

    btTransform GetDebugTransform() const {
        btTransform trans;
        m_rigidBody->getMotionState()->getWorldTransform(trans);
        return trans;
    }

    void SetScale(const Vector3& scale);

    void SetWorldTransform(const btTransform& trans) {
        m_rigidBody->getMotionState()->setWorldTransform(trans);
    }

    Vector4 GetDebugColor() const {
        return m_debugColor;
    }

    void SetDebugColor(const Vector4& c) {
        m_debugColor = c;
    }


    float Mass = 0.f;
    float Restitution = 0.5f;
    float Friction = 0.75f;

    virtual PHYSICS_COLLIDER_TYPE GetColliderType() const {
        return m_colliderType;
    }

    uint32_t ColliderFlags = 0;

protected:
    PhysicsWorld* m_world;
    std::unique_ptr<btCollisionShape> m_collisionShape;
    std::unique_ptr<btDefaultMotionState> m_defaultMotionState;
    std::unique_ptr<btRigidBody> m_rigidBody;
    Vector3 m_halfExtents;
    PHYSICS_COLLIDER_TYPE m_colliderType;
    Vector4 m_debugColor;
    const Mesh* m_mesh = nullptr;


};

//class Skeleton;
//
//class RagdollCollider : public PhysicsCollider {
//
//public:
//	PHYSICS_COLLIDER_TYPE GetColliderType() const override {
//		return PHYSICS_COLLIDER_TYPE::Ragdoll;
//	}
//
//	RagdollCollider(const std::string& name, const Skeleton* skeleton);
//
//};

class GhostCollider : public PhysicsCollider {
public: 
	GhostCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* wprld);
	void Awake() override;

	PHYSICS_COLLIDER_TYPE GetColliderType() const override {
		return PHYSICS_COLLIDER_TYPE::Ghost;
	}

private:
	std::unique_ptr<btPairCachingGhostObject> m_ghostObject;
	
};

class CompoundCollider : public PhysicsCollider
{

public:
    CompoundCollider(std::string name, std::vector<std::unique_ptr<btCollisionShape>>& hulls, PhysicsWorld* world);
    void Awake() override;

    PHYSICS_COLLIDER_TYPE GetColliderType() const override {
        return PHYSICS_COLLIDER_TYPE::Compound;
    }


private:
    std::vector<std::unique_ptr<btCollisionShape>> m_hulls;
};

class BoxCollider :
    public PhysicsCollider
{
public:
    explicit BoxCollider(std::string name, const Mesh* mesh, PhysicsWorld* world);
    explicit BoxCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world);
    ~BoxCollider();

    void Awake() override;


    void Serialize(std::ofstream& ofs) {
        PhysicsCollider::Serialize(ofs);
    }
};


class CapsuleCollider : public PhysicsCollider
{

public:

    CapsuleCollider(std::string name, const Mesh* mesh, PhysicsWorld* world);
    CapsuleCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world);
    ~CapsuleCollider();

    void Awake() override;


};


class ConvexHullCollider : public PhysicsCollider
{
public:
    explicit ConvexHullCollider(std::string name, const Mesh* mesh, PhysicsWorld* world);
    explicit ConvexHullCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world);
    void Awake() override;
    void Initialize() override;
    void InitFromMeshes(const std::vector<Mesh*>& meshes);

private:
    std::vector<Mesh*> m_meshes;
    std::unique_ptr<btTriangleMesh> m_btm;

};

class StaticTriangleMeshCollider : public PhysicsCollider
{
public:
    explicit StaticTriangleMeshCollider(std::string name, const Mesh* mesh, PhysicsWorld* world);
    explicit StaticTriangleMeshCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world);
    void Awake() override;
    void Initialize() override;
    void InitFromMeshes(const std::vector<Mesh*>& meshes);

private:
    std::vector<Mesh*> m_meshes;
    std::unique_ptr<btTriangleMesh> m_btm;
};


class CylinderCollider : public PhysicsCollider
{
public:
    explicit CylinderCollider(const std::string& name, const Mesh* mesh, PhysicsWorld* world);
    explicit CylinderCollider(const std::string& name, const Vector3& halfExtents, PhysicsWorld* world);
    void Awake() override;

};


class PlaneCollider :
    public PhysicsCollider
{
public:
    PlaneCollider(std::string name, Vector3 normal, float constant, PhysicsWorld* world);
    ~PlaneCollider();
    void Serialize(std::ofstream& ofs) const override;

    void Awake() override;

    Vector3 Normal;
    float Constant;

};


class SphereCollider :
    public PhysicsCollider
{
public:
    SphereCollider(std::string name, Mesh* mesh, PhysicsWorld* world);
    SphereCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world);
    ~SphereCollider();

    void Awake() override;

};

class BoneData;

class RagdollCapsuleShape : public btCapsuleShape {
public:
	RagdollCapsuleShape(float radius, float height, BoneData* parentBone, BoneData* childBone);
	
	BoneData* ParentBone() const {
		return m_parentBone;
	}

	BoneData* ChildBone() const {
		return m_childBone;
	}

	Vector3 AxisOfAlignment() const {
		return m_alignedAxis;
	}

	void SetAxisOfAlignment(const Vector3& axis) {
		m_alignedAxis = axis;
	}

	void SetOrientation(const Quaternion& q) {
		m_orientation = q;
	}

	Quaternion GetOrientation() const {
		return m_orientation;
	}

	Quaternion& GetOrientation() {
		return m_orientation;
	}

	void SetMotionState(btDefaultMotionState* ms) {
		m_motionState = ms;
	}

	void SetRigidBody(btRigidBody* rb) {
		m_rigidBody = rb;
	}

	btDefaultMotionState* GetMotionState() const {
		return m_motionState;
	}

	btRigidBody* GetRigidBody() const {
		return m_rigidBody;
	}

private:
	BoneData* m_parentBone = nullptr;
	BoneData* m_childBone = nullptr;
	Vector3 m_alignedAxis;
	Quaternion m_orientation;
	RagdollCapsuleShape* m_parentShape = nullptr;
	btRigidBody* m_rigidBody = nullptr;
	btDefaultMotionState* m_motionState = nullptr;
};

class RagdollCollider: public PhysicsCollider {

	using ShapeList = std::unordered_map<std::string, std::unique_ptr<btCollisionShape>>;
	using BodyList = std::unordered_map<std::string, std::unique_ptr<btRigidBody>>;
	using MotionStateList = std::unordered_map<std::string, std::unique_ptr<btDefaultMotionState>>;
	using ConstraintList = std::unordered_map<std::string, std::unique_ptr<btTypedConstraint>>;
	using MassList = std::unordered_map<std::string, float>;
public:

	enum BodyParts {

	};
	
	//RagdollCollider(const std::string& name, ShapeList& hulls, PhysicsWorld* world);
	RagdollCollider(const std::string& name, PhysicsWorld* world);

	void Awake() override;
	void Update(double dt) override;
	void Destroy() override;
	void ToggleEnabled(bool enabled) override;
	bool ShowGui() override;

	ShapeList m_hulls;
	BodyList m_bodies;
	MotionStateList m_motionStates;
	ConstraintList m_constraints;

	ShapeList m_temp_shapes;
	BodyList m_temp_bodies;
	MotionStateList m_temp_motionStates;
	MassList m_masses;


};

} // namespace Jasper
