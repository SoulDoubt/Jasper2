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

struct BoneData {

	int Id;
	int ParentID;
	std::vector<int> Children;
	std::string Name;
	Skeleton* skeleton = nullptr;
	ImporterSceneNode* INode = nullptr;
	BoneData* Parent = nullptr;
	Mesh* mesh = nullptr;
		
	// Node Transform is the mesh space transform of the bone relative to the 
	// mesh's origin.
	Transform NodeTransform;
	// The transform in model space
	Transform WorldTransform;
	// Offset Transform declares the transformation needed to transform from 
	// mesh space to the local space of this bone.
	Transform BoneOffsetTransform;	
	//Transform ToParentSpace();
	// The transform to actually send up to the shader
	//Transform RenderTransform() const;
	
	Transform GetSkinningTransform();

	bool PositionCorrected = false;


	Transform GetWorldTransform();
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

private:
	Skeleton* m_skeleton;

	void SaveAnimationFrame(int frame);
};

struct RotationKeyframe {
	float Time;
	Quaternion Value;
};

struct PositionKeyframe {
	float Time;
	Vector3 Value;
};

struct ScaleKeyframe {
	float Time;
	Vector3 Value;
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


};

class Animation {

public:

	std::string Name;
	float Ticks;
	float TicksPerSecond;
	float Duration;
	int Index;
	std::vector<BoneAnimation> BoneAnimations;

};


class AnimationComponent : public Component
{
public:

	AnimationComponent(const std::string& name);
	AnimationComponent(const std::string& name, Skeleton* sk);
	~AnimationComponent();

	std::vector<Animation>& GetAnimations();
	void AddAnimation(const Animation&& anim);

	void Update(double dt) override;
	void Awake() override;
	void UpdateSkeleton(BoneData* rootBone, double animTime, const Transform& parentTransform);
	bool ShowGui() override;

	void SetSkeleton(Skeleton* s) {
		m_skeleton = s;
	}

	Skeleton* GetSkeleton() {
		return m_skeleton;
	}

	std::chrono::high_resolution_clock::time_point PlaybackStartTime;

	void PlayAnimation(int index);
	void StopPlayback();

private:

	int m_currentAnimationIndex = -1;
	std::vector<Animation> m_animations;
	Skeleton* m_skeleton;
	bool m_loop = true;
	bool m_isPlaying = false;
	int m_lastPlayedRotation;
	int m_lastPlayedPosition;
	int m_lastPlayedScale;


    

};



}
