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


struct BoneData {
    aiNode* ainode;
    std::string Name;
    std::string ParentName;
    std::vector<VertexBoneWeight> Weights;
    std::vector<BoneData*> Children;    
    Transform BoneTransform;
    BoneData* Parent;
    Transform BoneOffsetTransform;    
    Transform InverseBindTransform;    
    int Index;
    int Depth;
};


class Skeleton
{
public:
    std::string RootBoneName;
    Transform GlobalInverseTransform;
    //Matrix4 GlobalInverseMatrix;
    std::vector<BoneData> Bones;
    std::unordered_map<std::string, int> m_boneMap;
    
    void EvaluateBoneSubtree(const BoneData& parent);
    void TraverseSkeleton(const aiNode* node);
    void TransformBone(BoneData& bone);
};

class AnimationSystem
{
public:
    AnimationSystem();
    ~AnimationSystem();

};

}
