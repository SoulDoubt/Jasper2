#pragma once

#include "Common.h"
#include <btBulletDynamicsCommon.h>
#include "PhysicsDebugDraw.h"
#include <vector>
#include <memory>


namespace Jasper
{

class PhysicsCollider;
class RagdollCollider;

class PhysicsWorld
{
public:

    explicit PhysicsWorld(Scene* scene);
    ~PhysicsWorld();

    NON_COPYABLE(PhysicsWorld)

    Scene* scene;

    void Initialize();
    void Destroy();
    void Update(double dt);

    void AddRigidBody(btRigidBody* rb);
    void AddCollider(PhysicsCollider* collider);
	void AddRagdoll(RagdollCollider* ragdoll);
    void ConvexSweepTest(btConvexShape* shape, btTransform& from, btTransform& to, btCollisionWorld::ClosestConvexResultCallback& callback);
	void ContactTest(btCollisionObject* shape, btCollisionWorld::ContactResultCallback& cb);

    void DrawPhysicsShape(const btTransform& worldTransform, const btCollisionShape* shape, const btVector3& color) {
        m_world->debugDrawObject(worldTransform, shape, color);
    }
    
    void DebugDrawWorld(){
        m_world->debugDrawWorld();
    }

    void RemoveRigidBody(btRigidBody* rb);
    void DeleteRigidBody(btRigidBody* rb);
	void RemoveConstraint(btTypedConstraint* con);
	void AddConstraint(btTypedConstraint* con);

    std::unique_ptr<PhysicsDebugDrawer> debugDrawer;

    btDiscreteDynamicsWorld* GetBtWorld() const {
        return m_world.get();
    }

    private:

//    m_broadphase = new btDbvtBroadphase();
//    m_collisionConfig = new btDefaultCollisionConfiguration();
//    m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfig);
//    m_solver = new btSequentialImpulseConstraintSolver();
//    m_world = new btDiscreteDynamicsWorld(m_collisionDispatcher, m_broadphase, m_solver, m_

    std::unique_ptr<btBroadphaseInterface> m_broadphase;
    std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfig;
    std::unique_ptr<btCollisionDispatcher> m_collisionDispatcher;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::unique_ptr<btDiscreteDynamicsWorld> m_world;
    std::vector<btCollisionShape*> m_shapes;
    std::vector<btRigidBody*> m_bodies;

};

inline void PhysicsWorld::ConvexSweepTest(btConvexShape* shape, btTransform& from, btTransform& to, btCollisionWorld::ClosestConvexResultCallback& callback)
{
    m_world->convexSweepTest(shape, from, to, callback);
	
}

inline void PhysicsWorld::ContactTest(btCollisionObject* shape, btCollisionWorld::ContactResultCallback& cb) {
	m_world->contactTest(shape, cb);
}

}

