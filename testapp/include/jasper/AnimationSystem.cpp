#include <AnimationSystem.h>
#include <Model.h>
#include <vector>
#include <Scene.h>
#include <imgui.h>
#include <AssetSerializer.h>

namespace Jasper
{

using namespace std;
using namespace std::chrono;

Matrix4 aiMatrix4x4ToMatrix4(const aiMatrix4x4& mm)
{
	return Matrix4(
		Vector4(mm.a1, mm.a2, mm.a3, mm.a4), 
		Vector4(mm.b1, mm.b2, mm.b3, mm.b4), 
		Vector4(mm.c1, mm.c2, mm.c3, mm.c4), 
		Vector4(mm.d1, mm.d2, mm.d3, mm.d4)
	);
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



BoneData* Skeleton::GetBone(const std::string & name)
{
	auto it = m_boneMap.find(name);
	if (it != m_boneMap.end()) {
		return Bones[it->second].get();
	}
	return nullptr;
}

BoneData* BoneData::getParentBone() const {	
	return Parent;
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
			rootBone->Parent = nullptr;
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
				this->GlobalInverseTransform = t.Inverted();
				rootBone->NodeTransform = t;				
			}
			else {
				this->GlobalInverseTransform = rootBoneNode->NodeTransform.Inverted();
				rootBone->NodeTransform = rootBoneNode->NodeTransform;
			}
			return rootBoneNode;
		}
	}
	return nullptr;
}

void Skeleton::UpdateWorldTransforms()
{
	for (auto& bone : Bones) {
		bone->UpdateWorldTransform();
	}
}

void SkeletonComponent::Update(double dt)
{
	auto go = this->GetGameObject();
	go->GetScene()->skeleton_to_debug = this->m_skeleton;
	go->GetScene()->skeleton_debug_game_object_transform = go->GetWorldTransform();
}

void SetBoneTransformGui(BoneData* bd, Skeleton* skeleton) {
	using namespace ImGui;
	//if (TreeNode(bd->Name.c_str())) {

	Transform& t = bd->NodeTransform;
	Vector3 pos = t.Position;
	Quaternion rot = t.Orientation;
	Vector3 scale = t.Scale;
	float roll = t.Orientation.Roll();
	float pitch = t.Orientation.Pitch();
	float yaw = t.Orientation.Yaw();
	bool rotation_updates = false;
	bool position_updates = false;
	bool scale_updates = false;
	if (ImGui::SliderAngle("Nt Roll", &roll)) {
		rotation_updates = true;
	}
	if (ImGui::SliderAngle("Nt Pitch", &pitch)) {
		rotation_updates = true;
	}
	if (ImGui::SliderAngle("Nt Yaw", &yaw)) {
		rotation_updates = true;
	}
	/*if (ImGui::InputFloat4("Nt Rot:", rot.AsFloatPtr())) {
		t.Orientation = rot;
	}*/
	/*if (ImGui::DragFloat3("Nt Pos:", pos.AsFloatPtr())) {
		t.Position = pos;
	}*/
	if (ImGui::InputFloat3("Nt Scale:", scale.AsFloatPtr())) {
		t.Scale = scale;
		scale_updates = true;
	}

	if (ImGui::InputFloat3("Position", pos.AsFloatPtr())) {
		t.Position = pos;
		position_updates = true;
	}


	/*Transform& ot = bd->BoneOffsetTransform;
	Vector3 opos = ot.Position;
	float oroll = ot.Orientation.Roll();
	float opitch = ot.Orientation.Pitch();
	float oyaw = ot.Orientation.Yaw();
	bool orotation_updates = false;
	if (ImGui::SliderAngle("Offset Roll", &oroll)) {
		orotation_updates = true;
	}
	if (ImGui::SliderAngle("Offset Pitch", &opitch)) {
		rotation_updates = true;
	}
	if (ImGui::SliderAngle("Offset Yaw", &oyaw)) {
		rotation_updates = true;
	}
	if (ImGui::DragFloat3("Offset Pos:", opos.AsFloatPtr())) {
		ot.Position = opos;
	}

	if (orotation_updates) {
		Quaternion oq = Quaternion(opitch, oroll, oyaw);
		ot.Orientation = oq;
	}*/

	if (rotation_updates) {
		Quaternion q = Quaternion(pitch, roll, yaw);
		t.Orientation = q;
		bd->skeleton->UpdateWorldTransforms();
	}

	if (position_updates || scale_updates) {
		bd->skeleton->UpdateWorldTransforms();
	}

	for (int child : bd->Children) {
		if (TreeNode(skeleton->Bones[child]->Name.c_str())) {
			SetBoneTransformGui(skeleton->Bones[child].get(), skeleton);
			TreePop();
		}
		//	}
		//	TreePop();
	}
}

bool SkeletonComponent::ShowGui()
{
	using namespace ImGui;

	static int frameNumber;
	ImGui::InputInt("Frame:", &frameNumber);
	if (ImGui::Button("Save Pose as Animation Frame")) {
		SaveAnimationFrame(frameNumber);
	}

	auto rootBone = m_skeleton->RootBone;

	SetBoneTransformGui(rootBone, m_skeleton);


	return false;
}

void SkeletonComponent::Serialize(std::ofstream & ofs) const
{
	assert(m_skeleton);
	using namespace AssetSerializer;
	Component::Serialize(ofs);
	int bonec = static_cast<int>(m_skeleton->Bones.size());
	WriteInt(ofs, bonec);
	for (const auto& b : m_skeleton->Bones) {
		// write bone name
		WriteString(ofs, b->Name);
		// write bone id, parent id, node transform, and offset transform
		int bid = b->Id;
		int pid = b->ParentID;
		WriteInt(ofs, bid);
		WriteInt(ofs, pid);
		WriteTransform(ofs, b->NodeTransform);
		WriteTransform(ofs, b->BoneOffsetTransform);

	}
}

void SkeletonComponent::SaveAnimationFrame(int frame)
{
	auto go = this->GetGameObject();
	AnimationComponent* animComponent = go->GetComponentByType<AnimationComponent>();
	
	
}

Transform BoneData::GetSkinningTransform() {
	return WorldTransform * BoneOffsetTransform;
}


Transform BoneData::GetWorldTransform() {
	
	return this->WorldTransform;
}

void BoneData::UpdateWorldTransform()
{
	Transform result = NodeTransform;
	auto pb = getParentBone();
	while (pb != nullptr) {
		result = pb->NodeTransform * result;
		pb = pb->getParentBone();
	}
	this->WorldTransform = result;
}



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

void AnimationComponent::Update(double dt)
{

	if (m_isPlaying) {
		auto& currentAnim = m_animations[m_currentAnimationIndex];
		high_resolution_clock::time_point currentTime = high_resolution_clock::now();
		auto animDtms = duration_cast<microseconds>(currentTime - PlaybackStartTime);
		auto animDt = (double)animDtms.count();
		animDt /= 1000000;
		float tps = currentAnim.TicksPerSecond != 0 ? currentAnim.TicksPerSecond : 24.0f;
		tps = tps; // slowing down for debugging
		double animTotalDurationSeconds = currentAnim.Duration / currentAnim.TicksPerSecond;
		double dtTicks = animDt * tps;
		if (dtTicks >= currentAnim.Duration) {
			PlaybackStartTime = currentTime;
			return;
		}
		double animTime = fmod(dtTicks, currentAnim.Duration);
		//int rootBoneID = m_skeleton->m_boneMap[m_skeleton->RootBoneName];
		auto rootBone = m_skeleton->RootBone;
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

void AnimationComponent::UpdateSkeleton(BoneData* rootBone, double animTime, const Transform& parentTransform)
{
	auto skeleton = rootBone->skeleton;
	//auto& boneAnim  =  m_animations[m_currentAnimationIndex].BoneAnimations.at) {
	if (rootBone->Id < m_animations[m_currentAnimationIndex].BoneAnimations.size()) {
		auto& boneAnim = m_animations[m_currentAnimationIndex].BoneAnimations[rootBone->Id];
		bool isRootBone = false;
		if (rootBone->ParentID == -1) {
			isRootBone = true;
		}

		auto& bone = skeleton->Bones[boneAnim.BoneIndex];
		int rotIndex = boneAnim.FindRotationKeyframe(animTime, m_lastPlayedRotation);
		int posIndex = boneAnim.FindPositionKeyframe(animTime, m_lastPlayedPosition);
		int scaleIndex = boneAnim.FindScaleKeyframe(animTime, m_lastPlayedScale);

		int nextRot = (rotIndex + 1) % boneAnim.RotationKeyframes.size();
		int nextPos = (posIndex + 1) % boneAnim.PositionKeyframes.size();
		int nextSca = (scaleIndex + 1) % boneAnim.ScaleKeyframes.size();

		const auto& currentRotation = boneAnim.RotationKeyframes[rotIndex];
		const auto& currentPosition = boneAnim.PositionKeyframes[posIndex];
		const auto& currentScale = boneAnim.ScaleKeyframes[scaleIndex];

		const auto& nextRotation = boneAnim.RotationKeyframes[nextRot];
		const auto& nextPosition = boneAnim.PositionKeyframes[nextPos];
		const auto& nextScale = boneAnim.ScaleKeyframes[nextSca];
		//bone->UpdateWorldTransform();
		//Transform worldTransform = bone->;
		Transform nodeTransform = bone->NodeTransform;
		//Transform nodeTransform = bone->ToParentSpace();

		float rotDelta = nextRotation.Time - currentRotation.Time;
		float posDelta = nextPosition.Time - currentPosition.Time;
		float scaleDelta = nextScale.Time - currentScale.Time;
		float rotPct = rotDelta > 0.f ? (animTime / rotDelta) / 100.0f : 0.f;
		float posPct = posDelta > 0.f ? (animTime / posDelta) / 100.0f : 0.f;
		float sclPct = scaleDelta > 0.f ? (animTime / scaleDelta) / 100.0f : 0.f;

		//Transform animBoneTransform;// = rootBone->BoneTransform;
		//Quaternion rotq = animRotation.Value;
		Quaternion rotq = Interpolate(currentRotation.Value, nextRotation.Value, rotPct);
		Vector3    posq = Lerp(currentPosition.Value, nextPosition.Value, posPct);
		Vector3    sclq = Lerp(currentScale.Value, nextScale.Value, sclPct);

		//animBoneTransform.Orientation = rotq;

		//nodeTransform.Orientation = animRotation.Value;
		//nodeTransform.Position = animPosition.Value;
		//nodeTransform.Scale = animScale.Value;
		nodeTransform.Orientation = rotq;// nextRotation.Value;
		nodeTransform.Position = posq;
		nodeTransform.Scale = sclq;

		m_lastPlayedPosition = m_lastPlayedPosition > 0 ? nextPos - 1 : 0;
		m_lastPlayedRotation = m_lastPlayedRotation > 0 ? nextRot - 1 : 0;
		m_lastPlayedScale = m_lastPlayedScale > 0 ? nextSca - 1 : 0;

		//nodeTransform = bone->InverseBindTransform * nodeTransform;

		//if (isRootBone) {
			//nodeTransform.Position += bone->NodeTransform.Position;
			//nodeTransform.Orientation -= bone->NodeTransform.Orientation;
		//}
		//bone->TransformationTransform = parentTransform * nodeTransform;
		Transform nt = nodeTransform; // Transform(animPosition.Value, animRotation.Value, animScale.Value);
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
}

bool AnimationComponent::ShowGui()
{

	using namespace ImGui;
	Component::ShowGui();
	//ImGui::Text("Animations Defined: %d", this->m_animations.size());
	int selected_animation;
	vector<string> animnames;
	animnames.push_back("None");
	for (const auto& an : this->GetAnimations()) {
		animnames.push_back(an.Name);
	}
	if (Combo("Animations", &selected_animation, animnames)) {

	}
	static char buffer[128];
	ImGui::InputText("New Animation Name:", buffer, 128);
	if (ImGui::Button("New")) {
		Animation new_anim;
		new_anim.Name = buffer;
		AddAnimation(move(new_anim));
	}

	if (Button("Start")) {
		this->PlayAnimation(0);
	}
	if (Button("Stop")) {
		this->StopPlayback();
	}

	return false;
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

void AnimationComponent::StopPlayback()
{
	m_currentAnimationIndex = 0;
	//PlaybackStartTime = std::chrono::high_resolution_clock::now();
	m_isPlaying = false;
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
