#include "CharacterController.h"
#include <btBulletDynamicsCommon.h>

namespace Jasper
{

CharacterController::CharacterController(btPairCachingGhostObject * ghostObject, btConvexShape * convexShape, btScalar stepHeight, PhysicsWorld* world, btVector3 upAxis)
    : GameObject("charachter_controller"), btKinematicCharacterController(ghostObject, convexShape, stepHeight, upAxis)
{
    m_world = world;
}

CharacterController::~CharacterController()
{
}

void CharacterController::debugDraw(btIDebugDraw* debugDrawer)
{

}


}
