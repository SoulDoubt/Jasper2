#pragma once


#include "Component.h"
#include "PhysicsWorld.h"
#include "Transform.h"
#include <memory>
#include <type_traits>



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

//enum class PHYSICS_COLLIDER_FLAGS : u_int32_t
//{
//    NONE = 0x00,
//    DRAW = 0x01
//};
//
//using PCF = std::underlying_type_t<PHYSICS_COLLIDER_FLAGS>;
//
//inline PHYSICS_COLLIDER_FLAGS operator|(PHYSICS_COLLIDER_FLAGS a, PHYSICS_COLLIDER_FLAGS b)
//{
//    return static_cast<PHYSICS_COLLIDER_FLAGS>(static_cast<PCF>(a) | static_cast<PCF>(b));
//}
//
//inline PHYSICS_COLLIDER_FLAGS operator&(PHYSICS_COLLIDER_FLAGS a, PHYSICS_COLLIDER_FLAGS b)
//{
//    return static_cast<PHYSICS_COLLIDER_FLAGS>(static_cast<PCF>(a) & static_cast<PCF>(b));
//}

class PhysicsCollider :	public Component
{
public:

    const static u_int32_t DRAW_COLLISION_SHAPE = 0x01;


    explicit PhysicsCollider(std::string name, Mesh* mesh, PhysicsWorld* world);
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

    u_int32_t ColliderFlags = 0;

protected:
    PhysicsWorld* m_world;
    std::unique_ptr<btCollisionShape> m_collisionShape;
    std::unique_ptr<btDefaultMotionState> m_defaultMotionState;
    std::unique_ptr<btRigidBody> m_rigidBody;
    Vector3 m_halfExtents;
    PHYSICS_COLLIDER_TYPE m_colliderType;
    Vector4 m_debugColor;
    Mesh* m_mesh = nullptr;


};

class CompoundCollider : public PhysicsCollider
{

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
