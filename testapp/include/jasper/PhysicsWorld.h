#ifndef _JASPER_PHYSICS_WORLD_H_
#define _JASPER_PHYSICS_WORLD_H_

#include "Common.h"
#include <btBulletDynamicsCommon.h>
#include "PhysicsDebugDraw.h"
#include <vector>


namespace Jasper
{

class PhysicsCollider;

class PhysicsWorld
{
public:

    explicit PhysicsWorld(Scene* scene);
    ~PhysicsWorld();

    NON_COPYABLE(PhysicsWorld)

    Scene* scene;

    void Initialize();
    void Destroy();
    void Update(float dt);

    void AddRigidBody(btRigidBody* rb);
    void AddCollider(PhysicsCollider* collider);
    void ConvexSweepTest(btConvexShape* shape, btTransform& from, btTransform& to, btCollisionWorld::ClosestConvexResultCallback& callback);

    void DrawPhysicsShape(const btTransform& worldTransform, const btCollisionShape* shape, const btVector3& color) {
        m_world->debugDrawObject(worldTransform, shape, color);
    }

    void RemoveRigidBody(btRigidBody* rb);

    PhysicsDebugDrawer* debugDrawer;

    btDiscreteDynamicsWorld* GetBtWorld() const {
        return m_world;
    }

private:

    btBroadphaseInterface* m_broadphase;
    btDefaultCollisionConfiguration* m_collisionConfig;
    btCollisionDispatcher* m_collisionDispatcher;
    btSequentialImpulseConstraintSolver* m_solver;
    btDiscreteDynamicsWorld* m_world;
    std::vector<btCollisionShape*> m_shapes;
    std::vector<btRigidBody*> m_bodies;

};

inline void PhysicsWorld::ConvexSweepTest(btConvexShape* shape, btTransform& from, btTransform& to, btCollisionWorld::ClosestConvexResultCallback& callback)
{
    m_world->convexSweepTest(shape, from, to, callback);
}

}

#endif // _JASPER_PHYSICS_WORLD_H_
