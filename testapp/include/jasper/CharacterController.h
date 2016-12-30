#ifndef _JASPER_CHARACTER_CONTROLLER_H_
#define _JASPER_CHARACTER_CONTROLLER_H_

#include "GameObject.h"
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "PhysicsWorld.h"


namespace Jasper
{

class PhysicsWorld;


class CharacterController :  public GameObject
{
public:
	CharacterController() : GameObject("player") {

	}

	enum class CameraType
	{
		FIRST_PERSON,
		FLYING
	};

	void Rotate(float pitch, float roll, float yaw);
	void Translate(float x, float y, float z);
	void Translate(const Vector3& vec);

	void Awake() override;

	Vector3 GetPosition() const {
		return m_transform.Position;
	}

	

	
    ~CharacterController();

private : 

	PhysicsCollider* m_collider;
	PhysicsWorld* m_physicsWorld;

	void UpdateVectors();

	CameraType m_type;

	static Vector3 WORLD_X_AXIS;
	static Vector3 WORLD_Y_AXIS;
	static Vector3 WORLD_Z_AXIS;

	Vector3 m_localXAxis;
	Vector3 m_localYAxis;
	Vector3 m_localZAxis;

	Vector3 m_viewVector;
	Vector3 m_upVector;
	Vector3 m_rightVector;

	float m_accumPitch = 0.f;


};

} // namespace Jasper

#endif // _JASPER_CHARACTER_CONTROLLER_H_
