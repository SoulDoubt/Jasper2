#ifndef _JASPER_CHARACTER_CONTROLLER_H_
#define _JASPER_CHARACTER_CONTROLLER_H_

#include "GameObject.h"
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include "PhysicsWorld.h"

class btConvexShape;

namespace Jasper {

class PhysicsWorld;


class CharacterController : public btKinematicCharacterController, public GameObject
{
public:	
	CharacterController(btPairCachingGhostObject* ghostObject, btConvexShape* convexShape, btScalar stepHeight, PhysicsWorld* world, const btVector3 up = btVector3(0.f, 1.f, 0.f));
	~CharacterController();

	void debugDraw(btIDebugDraw* debugDrawer) override;

	btConvexShape* GetCollisionShape() {
		return m_convexShape;		
	}

	Transform GetGhostWorldTransform() {
		btTransform btt = m_ghostObject->getWorldTransform();
		return Transform(btt);
	}

	void StepPlayer(float dt) {
		playerStep(m_world->GetBtWorld(), dt);
	}

private:
	PhysicsWorld* m_world;


};

} // namespace Jasper

#endif // _JASPER_CHARACTER_CONTROLLER_H_