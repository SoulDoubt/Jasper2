#include "AnimationSystem.h"
#include "Model.h"
#include <vector>
#include <Scene.h>

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
			//childBone->InverseBindTransform = (parentBone->NodeTransform * childBone->NodeTransform).Inverted();
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

BoneData* BoneData::getParentBone() const {
	if (ParentID == -1) return nullptr;
	return skeleton->Bones[ParentID].get();
}


ImporterSceneNode* Skeleton::GetRootBoneNode()
{
	const auto bd = Bones[0].get();
	auto boneNode = bd->INode;
	assert(boneNode != nullptr);

	auto parentNode = boneNode->Parent;
	ImporterSceneNode* rootBoneNode = parentNode;
	while (parentNode != nullptr) {
		auto parentNode = boneNode->Parent;
		const auto parentBoneIterator = m_boneMap.find(parentNode->Name);
		if (parentBoneIterator != m_boneMap.end()) {
			// this is not the root bone								
			boneNode = boneNode->Parent;
		}
		else {
			// boneNode is the root bone							
			rootBoneNode = boneNode;
			BoneData* rootBone = Bones[m_boneMap[boneNode->Name]].get();
			this->RootBone = rootBone;
			assert(rootBone != nullptr);
			if (rootBoneNode->Parent != nullptr) {
				// need ro add any parent transforms to the rootBone
				Transform t = rootBone->NodeTransform;
				auto rbp = rootBoneNode->Parent;
				while (rbp != nullptr) {
					t = rbp->NodeTransform * t;
					rbp = rbp->Parent;
				}
				//rootBone->WorldTransform = t;
				this->GlobalInverseTransform = t;
			}
			else {
				this->GlobalInverseTransform = rootBoneNode->NodeTransform;
			}
			return rootBoneNode;
		}
	}
	return nullptr;
}


Transform BoneData::ToParentSpace()
{
	auto p = getParentBone();
	if (p) {
		return p->NodeTransform * this->NodeTransform;
	}
	return NodeTransform;
}

Transform BoneData::RenderTransform() const
{
	//return InverseBindTransform * BoneOffsetTransform;
	return Transform();
}

void BoneData::CalculateInverseBindTransforms(Transform parentTransform)
{
	/*Transform bindTransform = parentTransform * NodeTransform;
	InverseBindTransform = bindTransform.Inverted();
	for (int child : Children) {
		auto& childBone = skeleton->Bones.at(child);
		childBone->CalculateInverseBindTransforms(bindTransform);
	}*/
}

Transform BoneData::GetWorldTransform() {
	Transform result = NodeTransform;
	auto pb = getParentBone();
	while (pb != nullptr) {
		result = pb->NodeTransform * result;
		pb = pb->getParentBone();
	}
	return result;
}

void BoneData::UpdateWorldTransform()
{
	
	//this->WorldTransform = result;
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
		//childbd->InverseBindTransform = parent->InverseBindTransform * child.NodeTransform;
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
	auto go = this->GetGameObject();
	go->GetScene()->skeleton_to_debug = m_skeleton;
	go->GetScene()->skeleton_debug_game_object_transform = go->GetWorldTransform();
	if (m_isPlaying) {
		auto& currentAnim = m_animations[m_currentAnimationIndex];
		high_resolution_clock::time_point currentTime = high_resolution_clock::now();
		auto animDtms = duration_cast<milliseconds>(currentTime - PlaybackStartTime);
		auto animDt = (float)animDtms.count();
		animDt /= 1000.f;
		float tps = currentAnim.TicksPerSecond != 0 ? currentAnim.TicksPerSecond : 24.0f;
		tps = tps; // slowing down for debugging
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
	PlayAnimation(0);
}

void AnimationComponent::UpdateSkeleton(BoneData* rootBone, float animTime, const Transform& parentTransform)
{
	auto skeleton = rootBone->skeleton;
	//auto& boneAnim  =  m_animations[m_currentAnimationIndex].BoneAnimations.at) {
	auto& boneAnim = m_animations[m_currentAnimationIndex].BoneAnimations[rootBone->Id];

	auto& bone = skeleton->Bones[boneAnim.BoneIndex];
	int rotIndex = boneAnim.FindRotationKeyframe(animTime, m_lastPlayedRotation);
	int posIndex = boneAnim.FindPositionKeyframe(animTime, m_lastPlayedPosition);
	int scaleIndex = boneAnim.FindScaleKeyframe(animTime, m_lastPlayedScale);
	auto& animRotation = boneAnim.RotationKeyframes[rotIndex];
	auto& animPosition = boneAnim.PositionKeyframes[posIndex];
	auto& animScale = boneAnim.ScaleKeyframes[scaleIndex];

	//bone->UpdateWorldTransform();
	//Transform worldTransform = bone->;
	Transform nodeTransform = bone->NodeTransform;
	//Transform nodeTransform = bone->ToParentSpace();


	// if we are not one keyframe into the animation, use the bone's transform
	// which is Bind.
	Quaternion previousRotation;
	float previousRotationTime = 0.f;
	//previousRotation.Value = nodeTransform.Orientation;
	//previousRotation.Time = 0.f;
	// we now have the keyframes that are due up next
	if (rotIndex - 1 > 0) {
		previousRotation = boneAnim.RotationKeyframes[rotIndex - 1].Value;
		previousRotationTime = boneAnim.RotationKeyframes[rotIndex - 1].Time;
	}
	else {
		previousRotation = boneAnim.RotationKeyframes[0].Value;
		previousRotationTime = boneAnim.RotationKeyframes[0].Time;
	}

	Vector3 previousPosition;
	float previousPositionTime = 0.f;
	if (posIndex - 1 > 0) {
		previousPosition = boneAnim.PositionKeyframes[posIndex - 1].Value;
		previousPositionTime = boneAnim.PositionKeyframes[posIndex - 1].Time;
	}
	else {
		previousPosition = boneAnim.PositionKeyframes[0].Value;
		previousPositionTime = boneAnim.PositionKeyframes[0].Time;
	}

	Vector3 previousScale;
	float previousScaleTime = 0.f;
	if (scaleIndex - 1 > 0) {
		previousScale = boneAnim.ScaleKeyframes[scaleIndex - 1].Value;
		previousScaleTime = boneAnim.ScaleKeyframes[scaleIndex - 1].Time;
	}
	else {
		previousScale = boneAnim.ScaleKeyframes[0].Value;
		previousScaleTime = boneAnim.ScaleKeyframes[0].Time;
	}



	float rotDelta = animRotation.Time - previousRotationTime;
	float posDelta = animPosition.Time - previousPositionTime;
	float scaleDelta = animScale.Time - previousScaleTime;
	float rotPct = rotDelta > 0.f ? animTime / rotDelta : 0.f;
	float posPct = posDelta > 0.f ? animTime / posDelta : 0.f;
	float sclPct = scaleDelta > 0.f ?animTime / scaleDelta: 0.f;

	//Transform animBoneTransform;// = rootBone->BoneTransform;
	//Quaternion rotq = animRotation.Value;
	Quaternion rotq = Interpolate(previousRotation, animRotation.Value, rotPct);
	Vector3    posq = Lerp(previousPosition, animPosition.Value, posPct);
	Vector3    sclq = Lerp(previousScale, animScale.Value, sclPct);
	
	//animBoneTransform.Orientation = rotq;

	//nodeTransform.Orientation = animRotation.Value;
	//nodeTransform.Position = animPosition.Value;
	//nodeTransform.Scale = animScale.Value;
	nodeTransform.Orientation = rotq;
	nodeTransform.Position = posq;
	nodeTransform.Scale = sclq;
	//bone->TransformationTransform = parentTransform * nodeTransform;
	Transform nt = Transform(animPosition.Value, animRotation.Value, animScale.Value);
	bone->NodeTransform = nt;// nodeTransform;
	bone->UpdateWorldTransform();





	//Transform globalTransform = parentTransform * nodeTransform;

	//rootBone->RenderTransform =  m_skeleton->GlobalInverseTransform *  globalTransform * rootBone->BoneOffsetTransform;
	//Transform globalTransform = parentTransform * animBoneTransform;

	//rootBone->SkinningTransform = rootBone->InverseBindTransform * globalTransform * animBoneTransform;

	for (int i : rootBone->Children) {
		BoneData* childBone = m_skeleton->Bones[i].get();
		UpdateSkeleton(childBone, animTime, Transform());
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
	return 0;
}

int BoneAnimation::FindPositionKeyframe(float time, int startIndex)
{
	for (int i = startIndex; i < PositionKeyframes.size(); ++i) {
		if (time < PositionKeyframes[i].Time) {
			return i;
		}
	}
	return 0;
}

int BoneAnimation::FindScaleKeyframe(float time, int startIndex)
{
	for (int i = startIndex; i < ScaleKeyframes.size(); ++i) {
		if (time < ScaleKeyframes[i].Time) {
			return i;
		}
	}
	return 0;
}


} // Jasper
