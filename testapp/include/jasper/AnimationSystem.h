#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>

#include <Transform.h>
#include <assimp/scene.h>
#include <memory>

#include <Component.h>

namespace Jasper
{
    
Matrix4 aiMatrix4x4ToMatrix4(const aiMatrix4x4& mm);
Transform aiMatrix4x4ToTransform(const aiMatrix4x4& mm);
Quaternion aiQuaternionToQuatenrion(const aiQuaternion& q);
Vector3 aiVector3ToVector3(const aiVector3D& v);

typedef unsigned uint;    
class Mesh;
class ImporterSceneNode;

struct ImporterSceneNode {
	std::string Name;
	Transform NodeTransform;
	ImporterSceneNode* Parent;
	std::vector<ImporterSceneNode> Children;
	bool isUsedBone = false;	
	ImporterSceneNode(aiNode* ainode);
	ImporterSceneNode() {}
};




class Skeleton;
class RagdollCapsuleShape;

struct BoneData {

	int Id;
	int ParentID;
	std::vector<int> Children;
	std::string Name;
	std::string MeshName;
	Skeleton* skeleton = nullptr;
	ImporterSceneNode* INode = nullptr;
	BoneData* Parent = nullptr;
	Mesh* mesh = nullptr;
	RagdollCapsuleShape* m_collisionShape = nullptr;
	// Node Transform is the mesh space transform of the bone relative to the 
	// mesh's origin.
	Transform NodeTransform;
	// The transform in model space
	Transform WorldTransform;
	// Offset Transform declares the transformation needed to transform from 
	// mesh space to the local space of this bone.
	Transform BoneOffsetTransform;	

	Transform BindTransform;

	int PhysicsControlled = 0;
	//Transform ToParentSpace();
	// The transform to actually send up to the shader
	//Transform RenderTransform() const;
	
	Transform GetSkinningTransform();

	bool PositionCorrected = false;

	Transform GetWorldTransform();
	void SetWorldTransform(const Transform& t);
	void UpdateWorldTransform();

private:
	BoneData* getParentBone() const;

};

struct ColladaSkin{
	std::vector<std::string> JointNames;
	std::vector<float> Weights;
};

struct ColladaMesh
{
	std::string Name;
	std::vector<Vector3> Positions;
	std::vector<Vector3> Normals;
	std::vector<Vector2> TexCoords;
	std::vector<uint32_t> PositionIndices;
	std::vector<uint32_t> NormalIndices;
	std::vector<uint32_t> TexCoordIndices;
	ColladaSkin Skin;
};

struct Bone {
	std::string Name;
	Transform BindLocalTransform;
	std::vector<Bone> Children;
	
};

class Skeleton
{
public:
	
	Skeleton(const std::string& name) : m_name(name){}
	std::string RootBoneName;
	Transform GlobalInverseTransform;
	std::string GetName() const {
		return m_name;
	}

	void SetName(const std::string& name) {
		m_name = name;
	}

	void SetPhysicsControl(bool p);
	bool IsPhysicsControlled() const;

	std::vector<std::unique_ptr<BoneData>> Bones;
	std::unordered_map<std::string, int> m_boneMap;

	BoneData* RootBone;

	//void EvaluateBoneSubtree(const BoneData& parent);
	//void TraverseSkeleton(const ImporterSceneNode* node);
	//void TransformBone(BoneData* bone);
	//void BuildIntoHierarchy();
	BoneData* GetBone(const std::string& name);
	ImporterSceneNode* GetRootBoneNode();

	void UpdateWorldTransforms();
	

private:
	std::string m_name;
	bool m_physicsControlled;
};

class SkeletonComponent : public Component {
public:
	SkeletonComponent(const std::string& name, Skeleton* skeleton) 
		: Component(name + "Skeleton"), m_skeleton(skeleton) {}

	void Update(double dt) override;
	bool ShowGui() override;

	void Serialize(std::ofstream& ofs) const override;

	Skeleton* GetSkeleton() const {
		return m_skeleton;
	}

	ComponentType GetComponentType() const override {
		return ComponentType::SkeletonComponent;
	}

private:
	Skeleton* m_skeleton;

	void SaveAnimationFrame(int frame);

	void MirrorPose();
};

struct RotationKeyframe {
	float Time;
	Quaternion Value;
	RotationKeyframe(float time, Quaternion val)
		: Time(time), Value(val) {}
	RotationKeyframe() : Time(0.f), Value(Quaternion()) {}
};

struct PositionKeyframe {
	float Time;
	Vector3 Value;
	PositionKeyframe(float time, Vector3 val)
		: Time(time), Value(val) {}
	PositionKeyframe() : Time(0.f), Value(Vector3()) {}
};

struct ScaleKeyframe {
	float Time;
	Vector3 Value;
	ScaleKeyframe(float time, Vector3 val)
		: Time(time), Value(val) {}
	ScaleKeyframe() : Time(0.f), Value(Vector3(1.f, 1.f, 1.f)) {}
};

struct BoneAnimation {

	std::string Name;	
	int BoneIndex;
	std::vector<RotationKeyframe> RotationKeyframes;
	std::vector<PositionKeyframe> PositionKeyframes;
	std::vector<ScaleKeyframe>    ScaleKeyframes;

	int FindRotationKeyframe(float time, int startIndex);
	int FindPositionKeyframe(float time, int startIndex);
	int FindScaleKeyframe(float time, int startIndex);

	int GetRotationKeyframeByTime(float time);
	int GetPositionKeyframeByTime(float time);
	int GetScaleKeyframeByTime(float time);


};

class Animation {

public:

	std::string Name;
	float Ticks;
	float TicksPerSecond = 24.f;
	float Duration = 0.f;
	int Index;
	std::vector<BoneAnimation> BoneAnimations;
	std::vector<int> Keyframes;
	BoneAnimation* GetBoneAnimationByBoneIndex(int idx);

};


class AnimationComponent : public Component
{
public:

	AnimationComponent(const std::string& name);
	AnimationComponent(const std::string& name, Skeleton* sk);
	~AnimationComponent();

	std::vector<Animation>& GetAnimations();

	const std::vector<Animation>& GetAnimations() const {
		return m_animations;
	}
	void AddAnimation(const Animation&& anim);

	void Update(double dt) override;
	void UpdateBoneAnimation(Jasper::BoneAnimation & boneAnim, int prevFrame, int nextFrame, double dtFrames);
	void Awake() override;
	void UpdateSkeleton(BoneData* rootBone, double animTime, const Transform& parentTransform);
	bool ShowGui() override;

	void SetSkeleton(Skeleton* s) {
		m_skeleton = s;
	}

	Skeleton* GetSkeleton() {
		return m_skeleton;
	}

	const Skeleton* GetSkeleton() const{
		return m_skeleton;
	}

	void PoseSkeleton(int animIndex, int framenumber);

	void AddKeyframe(int framenumber);
	void UpdateKeyframe(int framenumber);
	void DeleteKeyframe(int framenumber);

	std::chrono::high_resolution_clock::time_point PlaybackStartTime;

	void PlayAnimation(int index);
	void StopPlayback();

	void SetLooping(bool loop) {
		m_loop = loop;
	}

	ComponentType GetComponentType() const override {
		return ComponentType::AnimationComponent;
	}

private:

	int m_currentAnimationIndex = -1;
	std::vector<Animation> m_animations;
	Skeleton* m_skeleton;
	bool m_loop = true;
	bool m_isPlaying = false;
	int m_lastPlayedRotation;
	int m_lastPlayedPosition;
	int m_lastPlayedScale;
	int m_edit_animation = -1;

	std::vector<int> GetPreviousAndNextKeyframes(double dt);



    

};



}
