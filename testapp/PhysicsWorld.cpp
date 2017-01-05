#include "PhysicsWorld.h"
#include "PhysicsCollider.h"
#include <algorithm>

namespace Jasper
{

PhysicsWorld::PhysicsWorld(Scene* scene) : scene(scene)
{

}


PhysicsWorld::~PhysicsWorld()
{
    Destroy();
}

void PhysicsWorld::Initialize()
{
    m_broadphase = new btDbvtBroadphase();
    m_collisionConfig = new btDefaultCollisionConfiguration();
    m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfig);
    m_solver = new btSequentialImpulseConstraintSolver();
    m_world = new btDiscreteDynamicsWorld(m_collisionDispatcher, m_broadphase, m_solver, m_collisionConfig);
    m_world->setGravity( { 0.0f, -12.81f, 0.0f });
    debugDrawer = new PhysicsDebugDrawer(scene);
    debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    m_world->setDebugDrawer(debugDrawer);
}

// call when you want to remove a collider from the world
void PhysicsWorld::RemoveRigidBody(btRigidBody* rb)
{
    m_world->removeRigidBody(rb);
//    auto bp = std::find(begin(m_bodies), end(m_bodies), rb);
//    if (bp != end(m_bodies)) {
//        m_bodies.erase(bp);
//    }
//    delete rb->getMotionState();
//    delete rb;

}

void PhysicsWorld::Destroy()
{
    /*if (debugDrawer)
    	delete debugDrawer;*/

    delete m_world->getDebugDrawer();

    for (int i = 0; i < m_bodies.size(); i++) {
        auto body = m_bodies[i];
        m_world->removeRigidBody(body);
        delete body->getMotionState();
        delete body;
    }

    for (int i = 0; i < m_shapes.size(); i++) {
        auto cs = m_shapes[i];
        delete cs;
    }

    m_shapes.clear();

    if (m_world)
        delete m_world;
    if (m_solver)
        delete m_solver;
    if (m_collisionConfig)
        delete m_collisionConfig;
    if (m_collisionDispatcher)
        delete m_collisionDispatcher;
    if (m_broadphase)
        delete m_broadphase;

}

void PhysicsWorld::Update(float dt)
{
    m_world->stepSimulation(dt, 10);
}

void PhysicsWorld::AddRigidBody(btRigidBody * rb)
{    
    m_world->addRigidBody(rb);
}

void PhysicsWorld::AddCollider(PhysicsCollider* collider)
{
    auto rb = collider->GetRigidBody();
    m_world->addRigidBody(rb);
    m_shapes.push_back(collider->GetCollisionShape());
    m_bodies.push_back(collider->GetRigidBody());
}


}
