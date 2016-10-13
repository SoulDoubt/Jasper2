#ifndef _JASPER_CAMERA_H_
#define _JASPER_CAMERA_H_

#include "Common.h"
#include "Transform.h"
#include "matrix.h"
#include "GameObject.h"
#include <memory>
#include "CapsuleCollider.h"

namespace Jasper {

class PhysicsWorld;

class Camera : public GameObject
{
public:

	enum class CameraType {
		FIRST_PERSON,
		FLYING
	};

	explicit Camera(CameraType type);
	virtual ~Camera();

	void Awake();

	void SetPhysicsWorld(PhysicsWorld* world);

	Matrix4 GetViewMatrix();
	Matrix4 GetCubemapViewMatrix();
	void Rotate(float pitch, float roll, float yaw);
	void Translate(float x, float y, float z);
	void Translate(const Vector3& vec);

	Vector3 GetPosition() const {		
		return m_transform.Position;
	}

	Vector3 GetViewDirection() const {
		return m_viewVector;
	}	

	Quaternion m_orientation;
	Vector3 m_position;

private:
	
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

	PhysicsWorld* m_physicsWorld = nullptr;
	CapsuleCollider* m_collider = nullptr;

};

inline void Camera::SetPhysicsWorld(PhysicsWorld* world) {
	m_physicsWorld = world;
}

}
#endif
