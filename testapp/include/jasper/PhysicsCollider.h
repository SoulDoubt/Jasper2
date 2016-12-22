#ifndef _JASPER_PHYSICS_COLLIDER_H_
#define _JASPER_PHYSICS_COLLIDER_H_


#include "Component.h"
#include "PhysicsWorld.h"
#include "Transform.h"



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
    Sphere
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

    float Mass = 0.f;
    float Restitution = 0.5f;
    float Friction = 0.75f;

    PHYSICS_COLLIDER_TYPE GetColliderType() const {
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

};

} // namespace Jasper
#endif //_PHYSICS_COLLIDER_H_
