#pragma once


#include "Component.h"
#include "PhysicsWorld.h"
#include "Transform.h"
#include <memory>



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
	Compound
};

class PhysicsCollider :	public Component
{
public:
    //explicit PhysicsCollider(std::string name, Mesh* mesh, PhysicsWorld* world);
    explicit PhysicsCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world);
    virtual ~PhysicsCollider();

    ComponentType GetComponentType() const override final {
        return ComponentType::PhysicsCollider;
    }

    void Initialize() override;
    void Destroy() override;
    void Awake() override;
    void Start() override;
    void FixedUpdate() override;
    void Update(float dt) override;
    void LateUpdate() override;
    bool ShowGui() override;
    void Serialize(std::ofstream& ofs) const;

    Transform GetCurrentWorldTransform();

    void ToggleEnabled(bool enabled) override;

    PhysicsWorld* GetPhysicsWorld() const {
        return m_world;
    }
    btRigidBody* GetRigidBody() const {
        return m_rigidBody;
    }
    btCollisionShape* GetCollisionShape() const {
        return m_collisionShape;
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
		return debugColor;
	}

    float Mass = 0.f;
    float Restitution = 0.5f;
    float Friction = 0.75f;

    virtual PHYSICS_COLLIDER_TYPE GetColliderType() const {
        return m_colliderType;
    }

protected:
    PhysicsWorld* m_world;
    Mesh* m_mesh = nullptr;
    btCollisionShape* m_collisionShape;
    btDefaultMotionState* m_defaultMotionState;
    btRigidBody* m_rigidBody;
    Vector3 m_halfExtents;
    PHYSICS_COLLIDER_TYPE m_colliderType;
	Vector4 debugColor;

};

class CompoundCollider : public PhysicsCollider {

public:
	CompoundCollider(std::string name, std::vector<std::unique_ptr<btConvexHullShape>>& hulls, PhysicsWorld* world);
	void Awake() override;

	PHYSICS_COLLIDER_TYPE GetColliderType() const override {
		return PHYSICS_COLLIDER_TYPE::Compound;
	}
	

private:
	std::vector<std::unique_ptr<btConvexHullShape>> m_hulls;
};

} // namespace Jasper

