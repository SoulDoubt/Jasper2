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
    using namespace std;
    m_broadphase = make_unique<btDbvtBroadphase>();
    m_collisionConfig = make_unique<btDefaultCollisionConfiguration>();
    m_collisionDispatcher = make_unique<btCollisionDispatcher>(m_collisionConfig.get());
    m_solver = make_unique<btSequentialImpulseConstraintSolver>();
    m_world = make_unique<btDiscreteDynamicsWorld>(m_collisionDispatcher.get(), m_broadphase.get(), m_solver.get(), m_collisionConfig.get());
    m_world->setGravity( { 0.0f, -9.81f, 0.0f });
    debugDrawer = make_unique<PhysicsDebugDrawer>(scene);
    debugDrawer->setDebugMode(btIDebugDraw::DBG_FastWireframe);
    m_world->setDebugDrawer(debugDrawer.get());
}

// removes the rigid body from the dynamics world, but
// leaves it in place
void PhysicsWorld::RemoveRigidBody(btRigidBody* rb)
{
    m_world->removeRigidBody(rb);
}

// Removes the rigid body from the world and 
// destroys it.
void PhysicsWorld::DeleteRigidBody(btRigidBody* rb){
    m_world->removeRigidBody(rb);
    auto bp = std::remove(begin(m_bodies), end(m_bodies), rb);
    if (bp != end(m_bodies)) {
        m_bodies.erase(bp);
    }
    delete rb->getMotionState();
    delete rb;    
}

void PhysicsWorld::RemoveConstraint(btTypedConstraint * con)
{
	m_world->removeConstraint(con);
}

void PhysicsWorld::AddConstraint(btTypedConstraint * con)
{
	m_world->addConstraint(con);
}

void PhysicsWorld::Destroy()
{
    /*if (debugDrawer)
    	delete debugDrawer;*/

    //delete m_world->getDebugDrawer();

//    for (uint i = 0; i < m_bodies.size(); i++) {
//        auto body = m_bodies[i];
//        m_world->removeRigidBody(body);
//        //delete body->getMotionState();
//        //delete body;
//    }
//
//    for (uint i = 0; i < m_shapes.size(); i++) {
//        auto cs = m_shapes[i];
//        //delete cs;
//    }
//
//    m_shapes.clear();

//    if (m_world)
//        delete m_world;
//    if (m_solver)
//        delete m_solver;
//    if (m_collisionConfig)
//        delete m_collisionConfig;
//    if (m_collisionDispatcher)
//        delete m_collisionDispatcher;
//    if (m_broadphase)
//        delete m_broadphase;

}

void PhysicsWorld::Update(double dt)
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

void PhysicsWorld::AddRagdoll(RagdollCollider * ragdoll)
{
	for (const auto& constraint : ragdoll->m_constraints) {
		m_world->addConstraint(constraint.second.get(), true);
	}
	for (const auto& body : ragdoll->m_bodies) {
		if (body.second) {
			m_world->addRigidBody(body.second.get());
			m_bodies.push_back(body.second.get());
		}
	}
	for (const auto& shape : ragdoll->m_hulls) {
		m_shapes.push_back(shape.second.get());
	}
	
}


}
