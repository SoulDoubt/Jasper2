#include "AnimationSystem.h"

namespace Jasper
{

Matrix4 aiMatrix4x4ToMatrix4(const aiMatrix4x4& mm)
{
    return Matrix4(Vector4(mm.a1, mm.a2, mm.a3, mm.a4), Vector4(mm.b1, mm.b2, mm.b3, mm.b4), Vector4(mm.c1, mm.c2, mm.c3, mm.c4), Vector4(mm.d1, mm.d2, mm.d3, mm.d4));
}

Transform aiMatrix4x4ToTransform(const aiMatrix4x4& mm)
{
    Transform t;
    aiQuaternion brot;
    aiVector3D bpos;
    aiVector3D bscale;
    mm.Decompose(bscale, brot, bpos);
    t.Position    = Vector3(bpos.x, bpos.y, bpos.z);
    t.Orientation = Quaternion(brot.x, brot.y, brot.z, brot.w);
    t.Scale       = Vector3(bscale.x, bscale.y, bscale.z);

    if (t.Orientation.Length() > 1.f) {
        printf("Non unit length quaternion produced by assimp. Normalizing...\n");
        t.Orientation = Normalize(t.Orientation);
    }

    return t;
}



void Skeleton::TraverseSkeleton(const aiNode* node)
{
    int bdidx = this->m_boneMap[node->mName.data];
    BoneData& parent = this->Bones[bdidx];
    int childCount = node->mNumChildren;
    printf("Traversing children of: %s \n", parent.Name.data());
    for (int i = 0; i < childCount; ++i) {
        aiNode* child = node->mChildren[i];
        int childbdidx = this->m_boneMap[child->mName.data];
        BoneData& childbd = this->Bones[childbdidx];
        childbd.BoneTransform = aiMatrix4x4ToTransform(child->mTransformation);
        childbd.Parent = &parent;
        Transform nodeTransform = parent.BoneTransform * aiMatrix4x4ToTransform(child->mTransformation);

        childbd.InverseBindTransform = parent.InverseBindTransform * aiMatrix4x4ToTransform(child->mTransformation);        
        parent.Children.push_back(&childbd);
        if (node != child) {
            TraverseSkeleton(child);
        }
    }
}

void Skeleton::EvaluateBoneSubtree(const BoneData& parentBone)
{
    for (size_t i = 0; i < parentBone.Children.size(); ++i) {
        BoneData& childBone = *(parentBone.Children[i]);
        
        Transform parentTransform = parentBone.BoneOffsetTransform;
        Transform childTransform = childBone.BoneOffsetTransform;
        
        childBone.BoneOffsetTransform *= parentTransform;// * childTransform;
        
        EvaluateBoneSubtree(childBone);
    }
}


// ----------------- AnimationSyatem class ------------------------

AnimationSystem::AnimationSystem()
{
}

AnimationSystem::~AnimationSystem()
{
}

} // Jasper
