#include "AnimationSystem.h"
#include "Model.h"
#include <vector>

namespace Jasper
{

using namespace std;
using namespace std::chrono;

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

Quaternion aiQuaternionToQuatenrion(const aiQuaternion & q)
{
	return Quaternion(q.x, q.y, q.z, q.w);
}

Vector3 aiVector3ToVector3(const aiVector3D & v)
{
	return Vector3(v.x, v.y, v.z);
}


//Transform BoneData::BuildParentTransform()
//{
//	vector<Transform> pts;
//	auto parent = INode->Parent;
//	while (parent != nullptr) {
//		pts.push_back(parent->NodeTransform);
//		parent = parent->Parent;
//	}
//	Transform pt;
//	for (int i = pts.size() - 1; i >= 0; --i) {
//		pt *= pts.at(i);
//	}
//	this->ParentTransform = pt;
//	return pt;
//}

void Skeleton::RecursiveBuild(ImporterSceneNode * parentNode)
{
	BoneData* parentBone = GetBone(parentNode->Name);
	parentBone->NodeTransform = parentNode->NodeTransform;
	assert(parentBone);
	for (auto& child : parentNode->Children) {
		if (child.isUsedBone) {
			auto childBone = GetBone(child.Name);
			assert(childBone);
			childBone->InverseBindTransform = (parentBone->NodeTransform * childBone->NodeTransform).Inverted();
			parentBone->Children.push_back(childBone->Id);
			RecursiveBuild(&child);
		}
		else {
			printf("skipping bone child node: %s\n", child.Name.data());
		}
	}
}


void Skeleton::BuildIntoHierarchy()
{
	auto rootBoneNode = GetRootBoneNode();
	auto p = rootBoneNode->Parent;
	Transform t = rootBoneNode->NodeTransform;
	while (p != nullptr) {
		t = p->NodeTransform * t;
		p = p->Parent;
	}
	GlobalInverseTransform = t.Inverted();
	rootBoneNode->NodeTransform = t;
	RecursiveBuild(rootBoneNode);


}

BoneData* Skeleton::GetBone(const std::string & name)
{
	auto it = m_boneMap.find(name);
	if (it != m_boneMap.end()) {
		return Bones[it->second].get();
	}
	return nullptr;
}

ImporterSceneNode* Skeleton::GetRootBoneNode()
{
	const auto bd = Bones[0].get();
	auto boneNode = bd->INode;
	assert(boneNode != nullptr);
	ImporterSceneNode* rootBoneNode = nullptr;
	while (boneNode->Parent != nullptr) {
		auto parentNode = boneNode->Parent;
		auto parentBoneIterator = m_boneMap.find(parentNode->Name);
		if (parentBoneIterator != m_boneMap.end()) {
			// this is not the root bone								
			boneNode = boneNode->Parent;
		}
		else {
			// boneNode is the root bone				
			rootBoneNode = boneNode;
			return rootBoneNode;
		}
	}
	return nullptr;
}


Transform BoneData::RenderTransform() const
{
	return InverseBindTransform * BoneOffsetTransform;
}

void BoneData::CalculateInverseBindTransform(Transform parentTransform)
{
	Transform bindTransform = parentTransform * NodeTransform;
	InverseBindTransform = bindTransform.Inverted();
	for (int child : Children) {
		auto& childBone = skeleton->Bones.at(child);
		childBone->CalculateInverseBindTransform(bindTransform);
	}
}

Transform BoneData::GetWorldTransform() const
{
	Transform result = NodeTransform;
	int pid = ParentID;
	while (pid > 1) {
		auto parentBone = skeleton->Bones.at(pid).get();
		result = parentBone->NodeTransform * result;
		pid = parentBone->ParentID;
	}
	return result;
}

void BoneData::EvaluateSubtree() {
	/*for (int i : Children) {
		auto& child = skeleton->Bones[i];
		child.BoneTransform = this->BoneTransform * child.BoneTransform;
		child.EvaluateSubtree();
	}*/
}



void Skeleton::TraverseSkeleton(const ImporterSceneNode* node)
{
	int bdidx = this->m_boneMap[node->Name];
	BoneData* parent = this->Bones[bdidx].get();
	int childCount = node->Children.size();
	printf("Traversing children of: %s \n", parent->Name.data());
	for (int i = 0; i < childCount; ++i) {
		auto& child = node->Children[i];
		int childbdidx = this->m_boneMap[child.Name];
		BoneData* childbd = this->Bones[childbdidx].get();
		//childbd.BoneTransform = parent.BoneTransform * child.NodeTransform;
		childbd->ParentID = parent->Id;
		childbd->InverseBindTransform = parent->InverseBindTransform * child.NodeTransform;
		//parent.Children.push_back(&childbd);		
		TraverseSkeleton(&child);
	}
}


void Skeleton::TransformBone(BoneData* bone) {
	/*if (bone->ParentID > -1) {
		bone->BoneTransform = Bones[bone->ParentID]->BoneTransform * bone->BoneTransform;
		TransformBone(Bones[bone->ParentID].get());
	}
	else {
		return;
	}*/
}

//Transform BoneData::BuildParentTransforms()
//{
//	std::vector<Transform> transforms;
//	int parentID = ParentID;
//	while (parentID > -1) {
//		BoneData& parentBone = skeleton->Bones[parentID];
//		transforms.push_back(parentBone.BoneTransform);
//		parentID = parentBone.ParentID;
//	}
//	Transform result;
//	for (int i = transforms.size() - 1; i > -1; --i) {
//		result *= transforms[i];
//	}
//	this->ParentTransform = result;
//	return result;
//}


// ----------------- AnimationComponent class ------------------------
AnimationComponent::AnimationComponent(const std::string & name)
	: Component(name)
{
}

AnimationComponent::AnimationComponent(const std::string& name, Skeleton * sk)
	: Component(name), m_skeleton(sk)
{

}

AnimationComponent::~AnimationComponent()
{
}

std::vector<Animation>& AnimationComponent::GetAnimations() {
	return m_animations;
}

void AnimationComponent::AddAnimation(const Animation&& anim) {
	m_animations.emplace_back(anim);
}

void AnimationComponent::Update(float dt)
{
	if (m_isPlaying) {
		auto& currentAnim = m_animations[m_currentAnimationIndex];
		high_resolution_clock::time_point currentTime = high_resolution_clock::now();
		auto animDtms = duration_cast<milliseconds>(currentTime - PlaybackStartTime);
		auto animDt = (float)animDtms.count();
		animDt /= 1000.f;
		float tps = currentAnim.TicksPerSecond != 0 ? currentAnim.TicksPerSecond : 24.0f;
		float animTotalDurationSeconds = currentAnim.Duration / currentAnim.TicksPerSecond;
		float dtTicks = animDt * tps;
		if (dtTicks >= currentAnim.Duration) {
			PlaybackStartTime = currentTime;
			return;
		}
		float animTime = fmod(dtTicks, currentAnim.Duration);
		int rootBoneID = m_skeleton->m_boneMap[m_skeleton->RootBoneName];
		auto rootBone = m_skeleton->Bones[rootBoneID].get();
		UpdateSkeleton(rootBone, animTime, Transform());
	}
	/*else {
		for (auto& bone : m_skeleton->Bones) {
			bone->RenderTransform = bone->GetWorldTransform().Inverted() * bone->BoneOffsetTransform;
		}
	}*/

}

void AnimationComponent::Awake()
{
	//PlayAnimation(0);
}

void AnimationComponent::UpdateSkeleton(BoneData* rootBone, float animTime, const Transform& parentTransform)
{
	auto skeleton = rootBone->skeleton;
	auto& boneAnim = m_animations[m_currentAnimationIndex].BoneAnimations[rootBone->Id];
	int rotIndex = boneAnim.FindRotationKeyframe(animTime, m_lastPlayedRotation);
	auto& animRotation = boneAnim.RotationKeyframes[rotIndex];

	Transform nodeTransform = rootBone->NodeTransform;

	// if we are not one keyframe into the animation, use the bone's transform
	// which is Bind.
	RotationKeyframe previousRotation;
	previousRotation.Value = nodeTransform.Orientation;
	previousRotation.Time = 0.f;
	// we now have the keyframes that are due up next
	if (rotIndex - 1 > 0) {
		previousRotation = boneAnim.RotationKeyframes[rotIndex - 1];
	}

	float rotDelta = animRotation.Time - previousRotation.Time;

	float rotPct = animTime / rotDelta;

	Transform animBoneTransform;// = rootBone->BoneTransform;

	Quaternion rotq = Slerp(previousRotation.Value, animRotation.Value, rotPct);
	animBoneTransform.Orientation = rotq;

	nodeTransform.Orientation *= rotq;

	Transform globalTransform = parentTransform * nodeTransform;

	//rootBone->RenderTransform =  m_skeleton->GlobalInverseTransform *  globalTransform * rootBone->BoneOffsetTransform;
	//Transform globalTransform = parentTransform * animBoneTransform;

	//rootBone->SkinningTransform = rootBone->InverseBindTransform * globalTransform * animBoneTransform;

	for (int i : rootBone->Children) {
		BoneData* childBone = m_skeleton->Bones[i].get();
		UpdateSkeleton(childBone, animTime, globalTransform);
	}
}

void AnimationComponent::PlayAnimation(int index)
{
	assert(index < m_animations.size());
	m_currentAnimationIndex = index;
	PlaybackStartTime = std::chrono::high_resolution_clock::now();
	m_isPlaying = true;
	m_lastPlayedRotation = 0;
	m_lastPlayedPosition = 0;
	m_lastPlayedScale = 0;
}




int BoneAnimation::FindRotationKeyframe(float time, int startIndex)
{
	for (int i = startIndex; i < RotationKeyframes.size(); ++i) {

		if (time < RotationKeyframes[i].Time) {
			return i;
		}
	}
}

int BoneAnimation::FindPositionKeyframe(float time, int startIndex)
{
	for (int i = startIndex; i < PositionKeyframes.size(); ++i) {
		if (time < PositionKeyframes[i].Time) {
			return i;
		}
	}
}

int BoneAnimation::FindScaleKeyframe(float time, int startIndex)
{
	for (int i = startIndex; i < ScaleKeyframes.size(); ++i) {
		if (time < ScaleKeyframes[i].Time) {
			return i;
		}
	}
}

} // Jasper
