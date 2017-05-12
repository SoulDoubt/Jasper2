#include <Ragdoll.h>

namespace Jasper {

using namespace std;

Ragdoll::Ragdoll(const string & name, Skeleton* skeleton, PhysicsWorld* world)
	: Component(name), m_skeleton(skeleton), m_world(world)
{

}

Ragdoll::~Ragdoll()
{
	Destroy();
}

void Ragdoll::Initialize()
{
	m_skeleton->UpdateWorldTransforms();
	{
		// collect up the bones
		BoneData* b = nullptr;

		b = m_skeleton->FindBone("upperleg", "r");
		m_bones[BODYPART_RIGHT_UPPER_LEG] = b;

		b = b = m_skeleton->FindBone("lowerleg", "r");
		m_bones[BODYPART_RIGHT_LOWER_LEG] = b;

		b = m_skeleton->FindBone("upperarm", "r");
		m_bones[BODYPART_RIGHT_UPPER_ARM] = b;

		b = m_skeleton->FindBone("lowerarm", "r");
		m_bones[BODYPART_RIGHT_LOWER_ARM] = b;


		b = m_skeleton->FindBone("upperleg", "l");
		m_bones[BODYPART_LEFT_UPPER_LEG] = b;

		b = b = m_skeleton->FindBone("lowerleg", "l");
		m_bones[BODYPART_LEFT_LOWER_LEG] = b;

		b = m_skeleton->FindBone("upperarm", "l");
		m_bones[BODYPART_LEFT_UPPER_ARM] = b;

		b = m_skeleton->FindBone("lowerarm", "l");
		m_bones[BODYPART_LEFT_LOWER_ARM] = b;


		b = m_skeleton->FindBone("hips");
		m_bones[BODYPART_HIPS] = b;

		b = m_skeleton->FindBone("spine");
		m_bones[BODYPART_SPINE] = b;

		b = m_skeleton->FindBone("neck_0");
		m_bones[BODYPART_HEAD] = b;
	}

	// set the initial positions of the bones
	for (int i = 0; i < BODYPART_COUNT; ++i)
	{
		const Transform& t = m_bones[i]->GetWorldTransform();
		m_boneInitialOrientation[i] = t.Orientation;
		m_boneInitialPosition[i] = t.Position;
	}



	// Setup the geometry
	m_shapes[BODYPART_HIPS] = new btCapsuleShape(btScalar(0.1), btScalar(0.05));
	m_shapes[BODYPART_SPINE] = new btCapsuleShape(btScalar(0.1), btScalar(0.05));
	m_shapes[BODYPART_HEAD] = new btCapsuleShape(btScalar(0.1), btScalar(0.1));
	m_shapes[BODYPART_LEFT_UPPER_LEG] = new btCapsuleShape(btScalar(0.1), btScalar(0.2));
	m_shapes[BODYPART_LEFT_LOWER_LEG] = new btCapsuleShape(btScalar(0.1), btScalar(0.35));
	m_shapes[BODYPART_RIGHT_UPPER_LEG] = new btCapsuleShape(btScalar(0.1), btScalar(0.2));
	m_shapes[BODYPART_RIGHT_LOWER_LEG] = new btCapsuleShape(btScalar(0.1), btScalar(0.35));
	m_shapes[BODYPART_LEFT_UPPER_ARM] = new btCapsuleShape(btScalar(0.1), btScalar(0.15));
	m_shapes[BODYPART_LEFT_LOWER_ARM] = new btCapsuleShape(btScalar(0.1), btScalar(0.12));
	m_shapes[BODYPART_RIGHT_UPPER_ARM] = new btCapsuleShape(btScalar(0.1), btScalar(0.15));
	m_shapes[BODYPART_RIGHT_LOWER_ARM] = new btCapsuleShape(btScalar(0.1), btScalar(0.12));
	//m_shapes[BODYPART_GUN] = new btCapsuleShape(btScalar(0.1), btScalar(0.6));

	// Setup all the rigid bodies
	btTransform offset; offset.setIdentity();
	offset.setOrigin(btVector3(0, 0, 0));

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.), btScalar(0.95f), btScalar(0.)));
	m_bodies[BODYPART_HIPS] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_HIPS]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.), btScalar(1.3), btScalar(0.)));
	m_bodies[BODYPART_SPINE] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_SPINE]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.), btScalar(1.7), btScalar(0.35)));
	m_bodies[BODYPART_HEAD] = localCreateRigidBody(btScalar(1), offset*transform, m_shapes[BODYPART_HEAD]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(-0.15), btScalar(0.65), btScalar(0.)));
	m_bodies[BODYPART_LEFT_UPPER_LEG] = localCreateRigidBody(btScalar(1), offset*transform, m_shapes[BODYPART_LEFT_UPPER_LEG]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(-0.16), btScalar(0.25), btScalar(0.)));
	m_bodies[BODYPART_LEFT_LOWER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_LEFT_LOWER_LEG]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.15), btScalar(0.65), btScalar(0.)));
	m_bodies[BODYPART_RIGHT_UPPER_LEG] = localCreateRigidBody(btScalar(1), offset*transform, m_shapes[BODYPART_RIGHT_UPPER_LEG]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.16), btScalar(0.25), btScalar(0.)));
	m_bodies[BODYPART_RIGHT_LOWER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_RIGHT_LOWER_LEG]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(-0.3), btScalar(1.25), btScalar(0.1)));
	transform.getBasis().setEulerZYX(0, 0, M_PI_2);
	m_bodies[BODYPART_LEFT_UPPER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_LEFT_UPPER_ARM]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(-0.6), btScalar(1.25), btScalar(0.1)));
	transform.getBasis().setEulerZYX(0, 0, M_PI_2);
	m_bodies[BODYPART_LEFT_LOWER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_LEFT_LOWER_ARM]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.3), btScalar(1.25), btScalar(0.1)));
	transform.getBasis().setEulerZYX(0, 0, -M_PI_2);
	m_bodies[BODYPART_RIGHT_UPPER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_RIGHT_UPPER_ARM]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.6), btScalar(1.25), btScalar(0.1)));
	transform.getBasis().setEulerZYX(0, 0, -M_PI_2);
	m_bodies[BODYPART_RIGHT_LOWER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_RIGHT_LOWER_ARM]);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(-1.2), btScalar(1.25), btScalar(0.1)));
	transform.getBasis().setEulerZYX(0, 0, M_PI_2);
	//m_bodies[BODYPART_GUN] = localCreateRigidBody(btScalar(1.), offset*transform, m_shapes[BODYPART_GUN]);

	// Setup some damping on the m_bodies
	for (int i = 0; i < BODYPART_COUNT; ++i)
	{
		m_bodies[i]->setDamping(0.05, 0.85);
		m_bodies[i]->setDeactivationTime(3.0);
		m_bodies[i]->setSleepingThresholds(5.6, 5.5);
		m_bodies[i]->setContactProcessingThreshold(0.25f);
		m_bodies[i]->setCcdMotionThreshold(0.05f);
		m_bodies[i]->setCcdSweptSphereRadius(0.06f);
		m_bodyInitialOrientation[i] = Quaternion(m_bodies[i]->getOrientation());
		m_bodyInitialPosition[i] = Vector3(m_bodies[i]->getCenterOfMassPosition());
	}

	// Now setup the constraints
	btHingeConstraint* hingeC;
	btConeTwistConstraint* coneC;

	btTransform localA, localB;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0, -M_PI_2, 0); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.15), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, -M_PI_2, 0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.15), btScalar(0.)));
	hingeC = new btHingeConstraint(*m_bodies[BODYPART_HIPS], *m_bodies[BODYPART_SPINE], localA, localB);
	hingeC->setLimit(btScalar(-M_PI_4), btScalar(M_PI_2));
	m_joints[JOINT_PELVIS_SPINE] = hingeC;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0, 0, -M_PI_2); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.2), btScalar(0.15)));
	localB.getBasis().setEulerZYX(0, 0, -M_PI_2); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.1), btScalar(0.0)));
	coneC = new btConeTwistConstraint(*m_bodies[BODYPART_SPINE], *m_bodies[BODYPART_HEAD], localA, localB);
	coneC->setLimit(M_PI_4, M_PI_4, M_PI_4);
	m_joints[JOINT_SPINE_HEAD] = coneC;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0, 0, M_PI_4 * 5); localA.setOrigin(btVector3(btScalar(-0.05), btScalar(-0.1), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, 0, M_PI_4 * 5); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.1), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_bodies[BODYPART_HIPS], *m_bodies[BODYPART_LEFT_UPPER_LEG], localA, localB);
	coneC->setLimit(M_PI_4 / 2.0f, M_PI_4 / 2.0f, 0);
	m_joints[JOINT_LEFT_HIP] = coneC;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0, -M_PI_2, 0); localA.setOrigin(btVector3(btScalar(0.), btScalar(-0.225), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, -M_PI_2, 0); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.185), btScalar(0.)));
	hingeC = new btHingeConstraint(*m_bodies[BODYPART_LEFT_UPPER_LEG], *m_bodies[BODYPART_LEFT_LOWER_LEG], localA, localB);
	hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
	m_joints[JOINT_LEFT_KNEE] = hingeC;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0, 0, -M_PI_4); localA.setOrigin(btVector3(btScalar(0.05), btScalar(-0.10), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, 0, -M_PI_4); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.1), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_bodies[BODYPART_HIPS], *m_bodies[BODYPART_RIGHT_UPPER_LEG], localA, localB);
	coneC->setLimit(M_PI_4 / 2.0f, M_PI_4 / 2.0f, 0);
	m_joints[JOINT_RIGHT_HIP] = coneC;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0, -M_PI_2, 0); localA.setOrigin(btVector3(btScalar(0.), btScalar(-0.225), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, -M_PI_2, 0); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.185), btScalar(0.)));
	hingeC = new btHingeConstraint(*m_bodies[BODYPART_RIGHT_UPPER_LEG], *m_bodies[BODYPART_RIGHT_LOWER_LEG], localA, localB);
	hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
	m_joints[JOINT_RIGHT_KNEE] = hingeC;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0, 0, M_PI); localA.setOrigin(btVector3(btScalar(-0.15), btScalar(-0.1), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, 0, M_PI_2); localB.setOrigin(btVector3(btScalar(-0.1), btScalar(0.1), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_bodies[BODYPART_SPINE], *m_bodies[BODYPART_LEFT_UPPER_ARM], localA, localB);
	coneC->setLimit(M_PI_4, M_PI_4, 0);
	m_joints[JOINT_LEFT_SHOULDER] = coneC;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0, -M_PI_2, 0); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.18), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, -M_PI_2, 0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
	hingeC = new btHingeConstraint(*m_bodies[BODYPART_LEFT_UPPER_ARM], *m_bodies[BODYPART_LEFT_LOWER_ARM], localA, localB);
	hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
	m_joints[JOINT_LEFT_ELBOW] = hingeC;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0, 0, 0); localA.setOrigin(btVector3(btScalar(0.15), btScalar(-0.1), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, 0, M_PI_2); localB.setOrigin(btVector3(btScalar(0.1), btScalar(0.1), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_bodies[BODYPART_SPINE], *m_bodies[BODYPART_RIGHT_UPPER_ARM], localA, localB);
	coneC->setLimit(M_PI_4, M_PI_4, 0);
	m_joints[JOINT_RIGHT_SHOULDER] = coneC;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0, -M_PI_2, 0); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.18), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, -M_PI_2, 0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
	hingeC = new btHingeConstraint(*m_bodies[BODYPART_RIGHT_UPPER_ARM], *m_bodies[BODYPART_RIGHT_LOWER_ARM], localA, localB);
	hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
	m_joints[JOINT_RIGHT_ELBOW] = hingeC;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0, -M_PI_2, 0); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.2), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, -M_PI_2, 0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.3), btScalar(-0.2)));
	//coneC = new btConeTwistConstraint(*m_bodies[BODYPART_LEFT_LOWER_ARM], *m_bodies[BODYPART_GUN], localA, localB);
	coneC->setLimit(M_PI_4, M_PI_4, 0);
	//m_joints[JOINT_GUN] = coneC;

	for (int i = 0; i < JOINT_COUNT; ++i) {
		m_world->AddConstraint(m_joints[i]);
	}

	

}

void Ragdoll::Awake()
{
}

void Ragdoll::Destroy()
{
	for (int i = 0; i < JOINT_COUNT; i++) {
		m_world->RemoveConstraint(m_joints[i]);
		delete m_joints[i];
	}
	for (int i = 0; i < BODYPART_COUNT; i++) {
		m_world->RemoveRigidBody(m_bodies[i]);
		delete m_bodies[i];		
		delete m_shapes[i];
	}
	
}

void Ragdoll::CopyModelState()
{
	/*for (int i = 0; i < BODYPART_COUNT; ++i)
	{
		btTransform &t = m_bodies[i]->getWorldTransform();

		t.setRotation((Quaternion() *
			m_bones[i]->GetWorldTransform().Orientation *
			Inverse(m_boneInitialOrientation[i]) * Quaternion(t.getRotation())).AsBtQuaternion());

		t.setOrigin(BtOgre::Convert::toBullet(m_pModelNode->_getDerivedPosition() +
			(m_pModelNode->_getDerivedOrientation() *
				m_Bones[i]->_getDerivedPosition()) * m_Scale));
	}*/
}

btRigidBody * Ragdoll::localCreateRigidBody(btScalar mass, const btTransform & startTransform, btCollisionShape * shape)
{
	mass = 0.f;
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	rbInfo.m_additionalDamping = true;
	btRigidBody* body = new btRigidBody(rbInfo);

	m_world->AddRigidBody(body);

	return body;
}

}