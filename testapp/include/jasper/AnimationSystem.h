#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <Transform.h>
#include <assimp/scene.h>

namespace Jasper
{
    
Matrix4 aiMatrix4x4ToMatrix4(const aiMatrix4x4& mm);
Transform aiMatrix4x4ToTransform(const aiMatrix4x4& mm);

typedef unsigned uint;    
class Mesh;
class ImporterSceneNode;

struct VertexBoneWeight {
    uint Index;
    float Weight;
    Mesh* mesh;
};

inline bool operator<(const VertexBoneWeight& a, const VertexBoneWeight& b)
{
    return (size_t)a.mesh + a.Index < (size_t)b.mesh + b.Index;
}

inline bool operator==(const VertexBoneWeight& a, const VertexBoneWeight& b)
{
    return (a.mesh == b.mesh) && (a.Index == b.Index);
}

class Skeleton;

struct BoneData {
	int Id;
	int ParentID;
	std::vector<int> Children;
	Mesh* mesh;
	Skeleton* skeleton;
	std::string Name;
	std::string ParentName;
	std::vector<VertexBoneWeight> Weights;
	Transform ParentTransform; // concatenation of all parent transforms up until this bone
	Transform BoneTransform;
	Transform BoneOffsetTransform;
	Transform InverseBindTransform;

	Transform BuildParentTransforms();

};

class Skeleton
{
public:
	std::string RootBoneName;
	Transform GlobalInverseTransform;
	std::vector<BoneData> Bones;
	std::unordered_map<std::string, int> m_boneMap;


	void EvaluateBoneSubtree(const BoneData& parent);
	void TraverseSkeleton(const ImporterSceneNode* node);
	void TransformBone(BoneData& bone);
};







class AnimationSystem
{
public:
    AnimationSystem();
    ~AnimationSystem();

};

}
