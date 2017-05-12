#pragma once

#include <PhysicsWorld.h>
#include <AnimationSystem.h>
#include <StringFunctions.h>

namespace Jasper {

class Ragdoll : public Component {
public:

	enum
	{
		BODYPART_HIPS = 0,
		BODYPART_SPINE,
		BODYPART_HEAD,

		BODYPART_LEFT_UPPER_LEG,
		BODYPART_LEFT_LOWER_LEG,

		BODYPART_RIGHT_UPPER_LEG,
		BODYPART_RIGHT_LOWER_LEG,

		BODYPART_LEFT_UPPER_ARM,
		BODYPART_LEFT_LOWER_ARM,

		BODYPART_RIGHT_UPPER_ARM,
		BODYPART_RIGHT_LOWER_ARM,

		//BODYPART_GUN,

		BODYPART_COUNT
	};

	enum
	{
		JOINT_PELVIS_SPINE = 0,		
		JOINT_SPINE_HEAD,

		JOINT_LEFT_HIP,
		JOINT_LEFT_KNEE,

		JOINT_RIGHT_HIP,
		JOINT_RIGHT_KNEE,

		JOINT_LEFT_SHOULDER,
		JOINT_LEFT_ELBOW,

		JOINT_RIGHT_SHOULDER,
		JOINT_RIGHT_ELBOW,

		//JOINT_GUN,

		JOINT_COUNT
	};

	ComponentType GetComponentType() const override {
		return ComponentType::LauncherScript;
	}

	Ragdoll(const std::string& name, Skeleton* skeleton, PhysicsWorld* world);
	virtual ~Ragdoll();
	void Initialize() override;
	void Awake() override;
	void Destroy() override;

	void CopyModelState();

private:
	Skeleton* m_skeleton;

	btCollisionShape* m_shapes[BODYPART_COUNT];
	btRigidBody* m_bodies[BODYPART_COUNT];
	btTypedConstraint* m_joints[JOINT_COUNT];

	BoneData *m_bones[BODYPART_COUNT];
	Quaternion m_boneInitialOrientation[BODYPART_COUNT];
	Vector3 m_boneInitialPosition[BODYPART_COUNT];

	Vector3 m_bodyInitialPosition[BODYPART_COUNT];
	Quaternion m_bodyInitialOrientation[BODYPART_COUNT];

	bool m_bIsRagdolling;
	Vector3 m_ragdollOffset;

	btRigidBody* localCreateRigidBody(btScalar mass, const btTransform& startTransform, btCollisionShape* shape);

	PhysicsWorld* m_world;
};

} // Jasper