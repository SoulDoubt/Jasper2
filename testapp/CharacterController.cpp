#include "CharacterController.h"
#include <btBulletDynamicsCommon.h>
#include "CapsuleCollider.h"

namespace Jasper
{

//CharacterController::CharacterController(btPairCachingGhostObject * ghostObject, btConvexShape * convexShape, btScalar stepHeight, PhysicsWorld* world, btVector3 upAxis)
//    : GameObject("charachter_controller"), btKinematicCharacterController(ghostObject, convexShape, stepHeight, upAxis)
//{
//    m_world = world;
//}

struct ContactCallback : public btCollisionWorld::ContactResultCallback {

	btRigidBody* body;
	btTransform from, to;
	bool hit;

	bool HasHit() {
		return hit;
	}

	btVector3 ContactPoint;

	ContactCallback(btRigidBody* body, btTransform from, btTransform to) : body(body), from(from), to(to) {
		hit = false;
	}

	virtual btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0, int partId0, int index0,
		const btCollisionObjectWrapper* colObj1, int partId1, int index1)
	{
		btVector3 pt; // will be set to point of collision relative to body
		if (colObj0->m_collisionObject == body) {
			pt = cp.m_localPointA;
			
			ContactPoint = pt;
			hit = true;
		}
		else {
			assert(colObj1->m_collisionObject == body && "body does not match either collision object");
			pt = cp.m_localPointB;
		}
		// do stuff with the collision point
		return 0; // There was a planned purpose for the return value of addSingleResult, but it is not used so you can ignore it.
	}

};

Vector3 CharacterController::WORLD_X_AXIS = Vector3(1.0f, 0.0f, 0.0f);
Vector3 CharacterController::WORLD_Y_AXIS = Vector3(0.0f, 1.0f, 0.0f);
Vector3 CharacterController::WORLD_Z_AXIS = Vector3(0.0f, 0.0f, 1.0f);

void CharacterController::Rotate(float pitch, float roll, float yaw)
{
	if (pitch != 0.f) {

	}
	m_accumPitch += pitch;
	//btQuaternion orientation = btt.getRotation();
	Quaternion orientation = m_transform.Orientation;
	//Quaternion qo = Quaternion(orientation.x(), orientation.y(), orientation.z(), orientation.w());


	if (m_accumPitch > 90.f) {
		pitch = 0.f;
		m_accumPitch = 90.f;
	}
	if (m_accumPitch < -90.f) {
		pitch = 0.f;
		m_accumPitch = -90.f;
	}

	//btQuaternion xrot, yrot;
	//Quaternion xr, yr;
	Quaternion xrot, yrot;
	if (pitch != 0.f) {
		//xrot = btQuaternion(WORLD_X_AXIS.AsBtVector3(), DEG_TO_RAD(pitch));
		xrot = Quaternion::FromAxisAndAngle(WORLD_X_AXIS, DEG_TO_RAD(pitch));
		orientation = orientation * xrot;
		//qo = qo * xr;
	}

	if (yaw != 0.f) {
		
		yrot = Quaternion::FromAxisAndAngle(WORLD_Y_AXIS, DEG_TO_RAD(yaw));
		orientation = yrot * orientation;		
	}
	//btt.setRotation(orientation);
	m_transform.Orientation = orientation;
	if (m_collider) {
		btTransform btt;
		btt.setRotation(m_transform.Orientation.AsBtQuaternion());
		btt.setOrigin(m_transform.Position.AsBtVector3());
		m_collider->SetWorldTransform(btt);
	}

	UpdateVectors();
}

void CharacterController::Translate(float x, float y, float z)
{
	Translate({ x, y, z });
}

void CharacterController::Translate(const Vector3 & vec)
{
	

	Vector3 forwards;
	float z = vec.z * -1.0f;

	if (m_type == CameraType::FIRST_PERSON) {
		forwards = WORLD_Y_AXIS.Cross(m_localXAxis);
		forwards.Normalize();
	}
	else {
		forwards = m_viewVector;
	}
	auto& current = m_transform.Position;	

	if (m_collider) {
		//btVector3 after = current;
		Vector3 after = current;
		after += m_localXAxis.AsBtVector3() * vec.x;
		after += WORLD_Y_AXIS.AsBtVector3() * vec.y;
		after += forwards.AsBtVector3() * z;
		btTransform from, to;
		from.setIdentity();
		to.setIdentity();

		from.setOrigin(current.AsBtVector3());
		to.setOrigin(after.AsBtVector3());

		auto me = m_collider->GetRigidBody();

		auto myshape = static_cast<btConvexShape*>(m_collider->GetCollisionShape());
		auto cb = btCollisionWorld::ClosestConvexResultCallback(from.getOrigin(), to.getOrigin());
		m_collider->GetPhysicsWorld()->ConvexSweepTest(myshape, from, to, cb);

		if (cb.hasHit()) {
			if (cb.m_hitCollisionObject != me) {
				auto frac = cb.m_closestHitFraction;
				auto point = cb.m_hitPointWorld;
				auto norm = cb.m_hitNormalWorld;
				int i = 0;
				
			}
			
		}
		ContactCallback conb = ContactCallback(me, from, to);

		m_collider->GetPhysicsWorld()->ContactTest(me, conb);

		if (conb.HasHit()) {
			btVector3 dir = to.getOrigin() - from.getOrigin();
			btVector3 condir = conb.ContactPoint - from.getOrigin();
			
			const float epsilon = 0.0001;
			//const float min = btMax(epsilon, conb.ContactPoint);
			//current += -forwards * 0.1;
			btVector3 newPos = current.AsBtVector3();
			newPos.setInterpolate3(current.AsBtVector3(), after.AsBtVector3(), epsilon);
			m_transform.Position = Vector3(newPos);
		}
		else {
			m_transform.Position = after;
		}
		/*btCollisionWorld::ClosestConvexResultCallback cb(current.AsBtVector3(), after.AsBtVector3());
		cb.m_collisionFilterMask = btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter;*/

		//btConvexShape* cs = static_cast<btConvexShape*>(me);

		//m_physicsWorld->ConvexSweepTest(cs, from, to, cb);
		//if (cb.hasHit() && cb.m_hitCollisionObject != me) {
		//    /*if () {
		//    	printf("\nHit Self");
		//    }*/
		//    const float epsilon = 0.0001;
		//    const float min = btMax(epsilon, cb.m_closestHitFraction);
		//    //current += -forwards * 0.1;
		//    btVector3 newPos = current.AsBtVector3();
		//    newPos.setInterpolate3(current.AsBtVector3(), after.AsBtVector3(), min);
		//    m_transform.Position = Vector3(newPos);
		//} else {
		//    m_transform.Position = after;
		//}

	}
	else {
		current += m_localXAxis.AsBtVector3() * vec.x;
		current += WORLD_Y_AXIS.AsBtVector3() * vec.y;
		current += forwards.AsBtVector3() * z;
		m_transform.Position = current;
	}
	if (m_collider) {
		btTransform btt;
		btt.setRotation(m_transform.Orientation.AsBtQuaternion());
		btt.setOrigin(m_transform.Position.AsBtVector3());
		m_collider->SetWorldTransform(btt);
	}

	UpdateVectors();
}

void CharacterController::Awake()
{
	GameObject::Awake();
	m_collider = GetComponentByType<CapsuleCollider>();
	//m_physicsWorld = m_collider->GetPhysicsWorld();
	/*if (!m_collider) {
	m_collider = new CapsuleCollider("camera_collider", Vector3(1.0f, 2.0f, 1.0f), m_physicsWorld);
	}*/
	//btDefaultMotionState* ms = new btDefaultMotionState(m_transform.GetBtTransform());
	//btVector3 inertia;
	//m_collisionShape->calculateLocalInertia(mass, inertia);
	//btRigidBody::btRigidBodyConstructionInfo rbci(mass, ms, m_collisionShape.get(), inertia);
	//m_rigidBody = std::make_unique<btRigidBody>(rbci);
	//m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	//m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
	//m_physicsWorld->AddRigidBody(m_rigidBody.get());
	if (m_collider) {
		m_collider->GetRigidBody()->setCollisionFlags(m_collider->GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		m_collider->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);


	}
}

CharacterController::~CharacterController()
{
}

void CharacterController::UpdateVectors()
{
	const Matrix4 vm = GetWorldTransform().TransformMatrix();
	m_localXAxis = Vector3(vm.mat[0].x, vm.mat[1].x, vm.mat[2].x);
	m_localYAxis = Vector3(vm.mat[0].y, vm.mat[1].y, vm.mat[2].y);
	m_localZAxis = Vector3(vm.mat[0].z, vm.mat[1].z, vm.mat[2].z);
	m_viewVector = -m_localZAxis;

	if (m_collider)
		m_collider->SetWorldTransform(m_transform.GetBtTransform());
}



}
