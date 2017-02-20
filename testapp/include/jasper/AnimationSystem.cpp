#include "AnimationSystem.h"
#include "Model.h"
#include <vector>

namespace Jasper
{

using namespace std;

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
	t.Position = Vector3(bpos.x, bpos.y, bpos.z);
	t.Orientation = Quaternion(brot.x, brot.y, brot.z, brot.w);
	t.Scale = Vector3(bscale.x, bscale.y, bscale.z);

	if (t.Orientation.Length() > 1.f) {
		printf("Non unit length quaternion produced by assimp. Normalizing...\n");
		t.Orientation = Normalize(t.Orientation);
	}

	return t;
}



void Skeleton::TraverseSkeleton(const ImporterSceneNode* node)
{
	int bdidx = this->m_boneMap[node->Name];
	BoneData& parent = this->Bones[bdidx];
	int childCount = node->Children.size();
	printf("Traversing children of: %s \n", parent.Name.data());
	for (int i = 0; i < childCount; ++i) {
		auto& child = node->Children[i];
		int childbdidx = this->m_boneMap[child.Name];
		BoneData& childbd = this->Bones[childbdidx];
		childbd.BoneTransform = parent.BoneTransform * child.NodeTransform;
		childbd.ParentID = parent.Id;
		childbd.InverseBindTransform = parent.InverseBindTransform * child.NodeTransform;
		//parent.Children.push_back(&childbd);		
		TraverseSkeleton(&child);		
	}
}


void Skeleton::EvaluateBoneSubtree(const BoneData& parentBone)
{
	//for (size_t i = 0; i < parentBone.Children.size(); ++i) {
	//	BoneData& childBone = *(parentBone.Children[i]);

	//	Transform parentTransform = parentBone.BoneTransform;
	//	Transform childTransform = childBone.BoneTransform;

	//	childBone.BoneTransform = childTransform * parentTransform;// * childTransform;

	//	EvaluateBoneSubtree(childBone);
	//}
}

void Skeleton::TransformBone(BoneData& bone) {
	if (bone.ParentID > -1) {
		bone.BoneTransform = Bones[bone.ParentID].BoneTransform * bone.BoneTransform;
		TransformBone(Bones[bone.ParentID]);
	}
	else {
		return;
	}
}


// ----------------- AnimationSyatem class ------------------------

AnimationSystem::AnimationSystem()
{
}

AnimationSystem::~AnimationSystem()
{
}

Transform BoneData::BuildParentTransforms()
{
	std::vector<Transform> transforms;
	int parentID = ParentID;
	while (parentID > -1) {
		BoneData& parentBone = skeleton->Bones[parentID];
		transforms.push_back(parentBone.BoneTransform);
		parentID = parentBone.ParentID;
	}
	Transform result;
	for (int i = transforms.size() - 1; i > -1; --i) {
		result *= transforms[i];
	}
	this->ParentTransform = result;
	return result;
}

} // Jasper
