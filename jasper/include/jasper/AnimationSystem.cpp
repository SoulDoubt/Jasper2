#include <AnimationSystem.h>
#include <Model.h>
#include <vector>
#include <Scene.h>
#include <imgui.h>
#include <AssetSerializer.h>
#include <algorithm>
#include <StringFunctions.h>

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
	t.Position = aiVector3ToVector3(bpos);
	t.Orientation = aiQuaternionToQuatenrion(brot);
	t.Scale = aiVector3ToVector3(bscale);

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



void Skeleton::SetPhysicsControl(bool p)
{
	this->m_physicsControlled = p;
	for (auto& b : Bones) {
		b->PhysicsControlled = p;
	}
}

bool Skeleton::IsPhysicsControlled() const
{
	return false;
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

BoneData* Skeleton::FindBone(const std::string & name, const std::string& side)
{
	if (side == "") {
		// don't bother testing for left / right
		for (const auto& bone : Bones) {
			if (FindInString(name, bone->Name)) return bone.get();
		}
		return nullptr;
	} else{
		for (const auto& bone : Bones) {
			if (FindInString(side + name, bone->Name)) return bone.get();
		}
		return nullptr;
	}
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

void ShowBoneTransformGui(BoneData* bd, Skeleton* skeleton) {
	using namespace ImGui;

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
	if (ImGui::SliderAngle("Roll", &roll)) {
		rotation_updates = true;
	}
	if (ImGui::SliderAngle("Pitch", &pitch)) {
		rotation_updates = true;
	}
	if (ImGui::SliderAngle("Yaw", &yaw)) {
		rotation_updates = true;
	}
	if (ImGui::InputFloat3("Scale:", scale.AsFloatPtr())) {
		t.Scale = scale;
		scale_updates = true;
	}
	if (ImGui::InputFloat3("Position", pos.AsFloatPtr())) {
		t.Position = pos;
		position_updates = true;
	}

	if (rotation_updates) {
		Quaternion q = Quaternion(pitch, roll, yaw);
		t.Orientation = q;		
	}

	if (position_updates || scale_updates || rotation_updates) {
		bd->skeleton->UpdateWorldTransforms();
	}
}

bool SkeletonComponent::ShowGui()
{
	using namespace ImGui;

	static int frameNumber;
	vector<string> bonenames;
	bonenames.reserve(m_skeleton->Bones.size());
	for (const auto& b : m_skeleton->Bones) {
		bonenames.push_back(b->Name);
	}
	static int selected_bone = 0;
	ListBox("Bones", &selected_bone, bonenames);
	ImGui::Separator();
	ShowBoneTransformGui(m_skeleton->Bones[selected_bone].get(), m_skeleton);	

	if (ImGui::Button("Mirror Pose")) {
		this->MirrorPose();
	}
	return false;
}

void SkeletonComponent::Initialize()
{
	int x = 0;
	for (auto& bone : m_skeleton->Bones) {
		bone->BindTransform = bone->GetWorldTransform();

	}
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

void SkeletonComponent::MirrorPose()
{
	// get all the right side bones
	std::vector<BoneData*> rsbs;
	for (auto& bone : m_skeleton->Bones) {
		if (bone->Name[0] == 'r') {
			rsbs.push_back(bone.get());
		}
	}

	// get all the left side bones
	std::vector<BoneData*> lsbs;
	for (auto& bone : m_skeleton->Bones) {
		if (bone->Name[0] == 'l') {
			lsbs.push_back(bone.get());
		}
	}

	// for each bone on the right side, find the corresponding bone
	// on the left side...
	for (auto rsb : rsbs) {
		auto correspondingLeftSide = find_if(lsbs.begin(), lsbs.end(), [&](BoneData* bd) {
			return bd->Name.substr(1, string::npos) == rsb->Name.substr(1, string::npos);
		});
		if (correspondingLeftSide != lsbs.end()) {
			BoneData* lsb = *correspondingLeftSide;
			printf("found match: %s matches %s\n", rsb->Name.c_str(), lsb->Name.c_str());
			float pit = rsb->NodeTransform.Orientation.Pitch();
			float rol = rsb->NodeTransform.Orientation.Roll();
			float yaw = rsb->NodeTransform.Orientation.Yaw();

			lsb->NodeTransform.Orientation = Quaternion(-pit, -rol, -yaw);
		}
	}
	m_skeleton->UpdateWorldTransforms();
}

Transform BoneData::GetSkinningTransform() {
	return WorldTransform * BoneOffsetTransform;
}


Transform BoneData::GetWorldTransform() {

	return this->WorldTransform;
}

void BoneData::SetWorldTransform(const Transform & t)
{
	this->WorldTransform = t;
}

void BoneData::UpdateWorldTransform()
{
	if (!PhysicsControlled) {
		Transform result = NodeTransform;
		auto pb = getParentBone();
		while (pb != nullptr) {
			result = pb->NodeTransform * result;
			pb = pb->getParentBone();
		}
		this->WorldTransform = result;
	}
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
		//tps = tps; // slowing down for debugging		
		// we are this many frames into the animation
		double dtFrames = animDt * tps;
		if (dtFrames > currentAnim.Duration) {
			if (m_loop) {
				PlaybackStartTime = currentTime;
			}
			else {
				StopPlayback();
				return;
			}
			
		}

		auto prevNext = GetPreviousAndNextKeyframes(dtFrames);
		int prevFrame = prevNext[0];
		int nextFrame = prevNext[1];
		for (auto& boneAnim : currentAnim.BoneAnimations) {			
			UpdateBoneAnimation(boneAnim, prevFrame, nextFrame, dtFrames);
		}
		m_skeleton->UpdateWorldTransforms();	
	}	
}

void AnimationComponent::UpdateBoneAnimation(Jasper::BoneAnimation & boneAnim, int prevFrame, int nextFrame, double dtFrames)
{	
	Quaternion poseRotation, nextRotation, previousRotation;
	Vector3 posePosition, nextPosition, previousPosition;
	Vector3 poseScale, nextScale, previousScale;
	
	float prevTime, nextTime;

	double ddd, ddx, pct;

	bool updateRotation, updatePosition, updateScale;

	//updateRotation = m_animations[m_currentAnimationIndex].Keyframes[nextFrame].Index < boneAnim.RotationKeyframes.size();
	updateRotation = nextFrame < boneAnim.RotationKeyframes.size();
	updatePosition = nextFrame < boneAnim.PositionKeyframes.size();
	updateScale = nextFrame < boneAnim.ScaleKeyframes.size();

	if (updateRotation) {
		previousRotation = boneAnim.RotationKeyframes[prevFrame].Value;
		prevTime = boneAnim.RotationKeyframes[prevFrame].Time;
		nextRotation = boneAnim.RotationKeyframes[nextFrame].Value;
		nextTime = boneAnim.RotationKeyframes[nextFrame].Time;
		ddd = nextTime - prevTime;
		ddx = dtFrames - prevTime;
		if (ddd > 0) {
			pct = ddx / ddd;
		}
		else {
			pct = 0; // I guess??? Seems to work.
		}
		poseRotation = Interpolate(previousRotation, nextRotation, pct);
	}
	else {
		poseRotation = boneAnim.RotationKeyframes[0].Value;
	}
	

	if (updatePosition) {
		previousPosition = boneAnim.PositionKeyframes[prevFrame].Value;
		prevTime = boneAnim.PositionKeyframes[prevFrame].Time;
		nextPosition = boneAnim.PositionKeyframes[nextFrame].Value;
		nextTime = boneAnim.PositionKeyframes[nextFrame].Time;
		ddd = nextTime - prevTime;
		ddx = dtFrames - prevTime;
		if (ddd > 0) {
			pct = ddx / ddd;
		}
		else {
			pct = 0; // I guess??? Seems to work.
		}
		posePosition = Lerp(previousPosition, nextPosition, pct);
	}
	else {
		posePosition = boneAnim.PositionKeyframes[0].Value;
	}
	
	if (updateScale) {
		previousScale = boneAnim.ScaleKeyframes[prevFrame].Value;
		prevTime = boneAnim.ScaleKeyframes[prevFrame].Time;
		nextScale = boneAnim.ScaleKeyframes[nextFrame].Value;
		nextTime = boneAnim.ScaleKeyframes[nextFrame].Time;
		ddd = nextTime - prevTime;
		ddx = dtFrames - prevTime;
		if (ddd > 0) {
			pct = ddx / ddd;
		}
		else {
			pct = 0; // I guess??? Seems to work.
		}
		poseScale = Lerp(previousScale, nextScale, pct);
	}
	else {
		poseScale = boneAnim.ScaleKeyframes[0].Value;
	}


	m_skeleton->Bones[boneAnim.BoneIndex]->NodeTransform = Transform(posePosition, poseRotation, poseScale);
}

std::vector<int> AnimationComponent::GetPreviousAndNextKeyframes(double dt)
{
	auto& anim = m_animations[m_currentAnimationIndex];
	int previousFrame = 0;
	int nextFrame = 0;
	for (int i = 1; i < anim.Keyframes.size(); ++i) {
		nextFrame = i;
		if ((float)anim.Keyframes[i].Index > dt) {
			break;
		}
		previousFrame = i;
	}

	return std::vector<int>({previousFrame, nextFrame});
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
		// these are the parametars we are heading towards...
		int rotIndex = boneAnim.FindRotationKeyframe(animTime, m_lastPlayedRotation);
		int posIndex = boneAnim.FindPositionKeyframe(animTime, m_lastPlayedPosition);
		int scaleIndex = boneAnim.FindScaleKeyframe(animTime, m_lastPlayedScale);

		double toTime = boneAnim.RotationKeyframes[rotIndex].Time;
		Quaternion toRotation = boneAnim.RotationKeyframes[rotIndex].Value;
		Vector3 toPosition = boneAnim.PositionKeyframes[posIndex].Value;
		Vector3 toScale = boneAnim.ScaleKeyframes[scaleIndex].Value;


		// we need the provious frames to know where to start.
		int prevRotIndex = rotIndex - 1;
		int prevPosIndex = posIndex - 1;
		int prevScaIndex = scaleIndex - 1;

		Quaternion fromRotation = rootBone->NodeTransform.Orientation;
		Vector3 fromPosition = rootBone->NodeTransform.Position;
		Vector3 fromScale = rootBone->NodeTransform.Scale;
		double fromTime = 0;
		if (prevRotIndex > 0) {
			fromRotation = boneAnim.RotationKeyframes[prevRotIndex].Value;
			fromTime = boneAnim.RotationKeyframes[prevRotIndex].Time;
		}
		if (prevPosIndex > 0) {
			fromPosition = boneAnim.PositionKeyframes[prevPosIndex].Value;
		}
		if (prevScaIndex > 0) {
			fromScale = boneAnim.ScaleKeyframes[prevScaIndex].Value;
		}

		// now, how far are we between the 2 frames
		double tickDiff = toTime - fromTime;
		double ddd = animTime;
		if (animTime - tickDiff > 0) {
			ddd = animTime - tickDiff;
		}
		double pct = ddd / tickDiff;
		if (pct > 0) {
			Quaternion rotResult = Interpolate(fromRotation, toRotation, pct);

			rootBone->NodeTransform.Orientation = rotResult;
			rootBone->UpdateWorldTransform();
		}


		//int nextRot = (rotIndex + 1) % boneAnim.RotationKeyframes.size();
		//int nextPos = (posIndex + 1) % boneAnim.PositionKeyframes.size();
		//int nextSca = (scaleIndex + 1) % boneAnim.ScaleKeyframes.size();

		//if (nextRot > 0 || nextPos > 0 || nextSca > 0) {
		//	const auto& currentRotation = boneAnim.RotationKeyframes[rotIndex];
		//	const auto& currentPosition = boneAnim.PositionKeyframes[posIndex];
		//	const auto& currentScale = boneAnim.ScaleKeyframes[scaleIndex];

		//	const auto& nextRotation = boneAnim.RotationKeyframes[nextRot];
		//	const auto& nextPosition = boneAnim.PositionKeyframes[nextPos];
		//	const auto& nextScale = boneAnim.ScaleKeyframes[nextSca];
		//	//bone->UpdateWorldTransform();
		//	//Transform worldTransform = bone->;
		//	Transform nodeTransform = bone->NodeTransform;
		//	//Transform nodeTransform = bone->ToParentSpace();

		//	float rotDelta = nextRotation.Time - currentRotation.Time;
		//	float posDelta = nextPosition.Time - currentPosition.Time;
		//	float scaleDelta = nextScale.Time - currentScale.Time;
		//	float rotPct = rotDelta > 0.f ? (animTime / rotDelta) / 100.0f : 0.f;
		//	float posPct = posDelta > 0.f ? (animTime / posDelta) / 100.0f : 0.f;
		//	float sclPct = scaleDelta > 0.f ? (animTime / scaleDelta) / 100.0f : 0.f;

		//	//Transform animBoneTransform;// = rootBone->BoneTransform;
		//	//Quaternion rotq = animRotation.Value;
		//	Quaternion rotq = Interpolate(currentRotation.Value, nextRotation.Value, rotPct);
		//	Vector3    posq = Lerp(currentPosition.Value, nextPosition.Value, posPct);
		//	Vector3    sclq = Lerp(currentScale.Value, nextScale.Value, sclPct);

		//	//animBoneTransform.Orientation = rotq;

		//	//nodeTransform.Orientation = animRotation.Value;
		//	//nodeTransform.Position = animPosition.Value;
		//	//nodeTransform.Scale = animScale.Value;
		//	nodeTransform.Orientation = rotq;// nextRotation.Value;
		//	nodeTransform.Position = posq;
		//	nodeTransform.Scale = sclq;

		//	m_lastPlayedPosition = m_lastPlayedPosition > 0 ? nextPos - 1 : 0;
		//	m_lastPlayedRotation = m_lastPlayedRotation > 0 ? nextRot - 1 : 0;
		//	m_lastPlayedScale = m_lastPlayedScale > 0 ? nextSca - 1 : 0;

		//	//nodeTransform = bone->InverseBindTransform * nodeTransform;

		//	//if (isRootBone) {
		//		//nodeTransform.Position += bone->NodeTransform.Position;
		//		//nodeTransform.Orientation -= bone->NodeTransform.Orientation;
		//	//}
		//	//bone->TransformationTransform = parentTransform * nodeTransform;
		//	Transform nt = nodeTransform; // Transform(animPosition.Value, animRotation.Value, animScale.Value);
		//	bone->NodeTransform = nt;// nodeTransform;
		//	bone->UpdateWorldTransform();

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
	static int selected_animation;
	vector<string> animnames;
	for (const auto& an : this->GetAnimations()) {
		animnames.push_back(an.Name);
	}
	
	static char buffer[128];
	ImGui::InputText("New Animation", buffer, 128);
	ImGui::SameLine();
	if (ImGui::Button("New")) {
		Animation new_anim;
		new_anim.Name = buffer;
		
		AddAnimation(move(new_anim));
	}
	if (Combo("Animations", &selected_animation, animnames)) {
		m_edit_animation = selected_animation;
	}
	ImGui::Separator();

	if (m_edit_animation > -1) {

		auto& edit_animation = m_animations[m_edit_animation];
		ImGui::InputFloat("Duration", &(edit_animation.Duration));
		ImGui::InputFloat("TPS", &(edit_animation.TicksPerSecond));
		static int new_frame_number = 0;
		ImGui::InputInt("", &new_frame_number);
		ImGui::SameLine();
		if (ImGui::Button("Add Keyframe")) {
			AddKeyframe(new_frame_number);
		}
		if (m_edit_animation > -1) {
			std::vector<string> kfnames;
			kfnames.reserve(this->GetAnimations()[m_edit_animation].Keyframes.size());
			static int selected_keyframe = 0;
			for (Keyframe i : this->GetAnimations()[m_edit_animation].Keyframes) {
				string fname = "Keyframe: " + to_string(i.Index);
				kfnames.emplace_back("Keyframe: " + to_string(i.Index));
			}
			if (ListBox("Keyframes", &selected_keyframe, kfnames)) {
				PoseSkeleton(m_edit_animation, this->GetAnimations()[m_edit_animation].Keyframes[selected_keyframe]);
			}
			if (SmallButton("Update")) {
				int fn = this->GetAnimations()[m_edit_animation].Keyframes[selected_keyframe].Index;
				UpdateKeyframe(fn);
			}
			ImGui::SameLine();
			if (SmallButton("Delete")) {
				int fn = this->GetAnimations()[m_edit_animation].Keyframes[selected_keyframe].Index;
				DeleteKeyframe(fn);
			}
		}

		ImGui::Separator();

		if (Button("Start")) {
			this->PlayAnimation(m_edit_animation);
		}
		ImGui::SameLine();
		if (Button("Stop")) {
			this->StopPlayback();
		}
		ImGui::SameLine();
		static bool loop = true;
		if (ImGui::Checkbox("Loop", &loop)) {
			SetLooping(loop);
		}
		ImGui::Text("Save animation as:");
	}

	static vector<string> anims = AssetSerializer::GetFileNamesByExtension("../animations/", "anim");
	static int selected_anim_file_idx = -1;
	ListBox("Animations", &selected_anim_file_idx, anims);

	if (ImGui::Button("Load")) {
		if (selected_anim_file_idx > -1) {
			string pathToLoad = anims[selected_anim_file_idx];
			ifstream ifs;
			ifs.open(pathToLoad, ios::in | ios::binary);
			Animation a;
			AssetSerializer::DeserializeAnimation(ifs, &a);
			sort(a.Keyframes.begin(), a.Keyframes.end(), [&](const Keyframe& a, const Keyframe& b) {
				return a.Index > b.Index;
			});
			this->m_animations.emplace_back(move(a));
		}
	}

	if (ImGui::Button("Save")) {
		ofstream ofs;
		string fname = "../animations/" + GetGameObject()->GetName() + "_" + m_animations[m_edit_animation].Name + ".anim";
		ofs.open(fname, ios::out | ios::binary);
		AssetSerializer::SerializeAnimation(ofs, &(m_animations[m_edit_animation]));
	}


	return false;
}

void AnimationComponent::PoseSkeleton(int animIndex, const Keyframe& keyframe)
{
	assert(animIndex > -1);
	Animation& anim = this->GetAnimations()[animIndex];
	auto skel = this->GetSkeleton();

	for (BoneAnimation& ba : anim.BoneAnimations) {
		auto& bone = this->GetSkeleton()->Bones[ba.BoneIndex];
		if (keyframe.Contents & KEYFRAME_CONTENTS::HAS_ROTATION) {
			auto& rkf = ba.RotationKeyframes[ba.GetRotationKeyframeByTime((float)keyframe.Index)];
			bone->NodeTransform.Orientation = rkf.Value;
		}
		if (keyframe.Contents & KEYFRAME_CONTENTS::HAS_POSITION) {
			auto& pkf = ba.PositionKeyframes[ba.GetPositionKeyframeByTime((float)keyframe.Index)];
			bone->NodeTransform.Position = pkf.Value;
		}
		if (keyframe.Contents & KEYFRAME_CONTENTS::HAS_SCALE) {
			auto& skf = ba.ScaleKeyframes[ba.GetScaleKeyframeByTime((float)keyframe.Index)];
			bone->NodeTransform.Scale = skf.Value;
		}

	}
	this->GetSkeleton()->UpdateWorldTransforms();

}

void AnimationComponent::AddKeyframe(int framenumber)
{
	if (m_edit_animation > -1) {
		//assert(m_edit_animation < m_animations.size(), "Animation index out of range.");
		Animation& anim = this->m_animations[m_edit_animation];
		anim.Keyframes.push_back(Keyframe{framenumber, 0});
		sort(anim.Keyframes.begin(), anim.Keyframes.end(), [&](const Keyframe& a, const Keyframe& b) {
			return a.Index > b.Index;
		});
		if ((float)framenumber > anim.Duration) {
			anim.Duration = (float)framenumber;
			anim.Ticks = (float)framenumber;
		}
		for (auto& bone : m_skeleton->Bones) {
			BoneAnimation* ba = anim.GetBoneAnimationByBoneIndex(bone->Id);
			if (ba == nullptr) {
				// insert a new animation for this bone
				BoneAnimation boneAnim;
				boneAnim.BoneIndex = bone->Id;
				boneAnim.Name = bone->Name;
				boneAnim.PositionKeyframes.push_back(PositionKeyframe((float)framenumber, bone->NodeTransform.Position));
				boneAnim.RotationKeyframes.push_back(RotationKeyframe((float)framenumber, bone->NodeTransform.Orientation));
				boneAnim.ScaleKeyframes.push_back(ScaleKeyframe((float)framenumber, bone->NodeTransform.Scale));
				anim.BoneAnimations.emplace_back(boneAnim);
			}
			else {
				//ba->BoneIndex = bone->Id;
				//ba->Name = bone->Name;
				int pkf = ba->GetPositionKeyframeByTime((float)framenumber);
				if (pkf == -1) {
					ba->PositionKeyframes.push_back(PositionKeyframe((float)framenumber, bone->NodeTransform.Position));
				}
				else {
					auto& pk = ba->PositionKeyframes[pkf];
					pk.Value = bone->NodeTransform.Position;
				}
				int rkf = ba->GetRotationKeyframeByTime((float)framenumber);
				if (rkf == -1) {
					ba->RotationKeyframes.push_back(RotationKeyframe((float)framenumber, bone->NodeTransform.Orientation));
				}
				else {
					auto& rk = ba->RotationKeyframes[rkf];
					rk.Value = bone->NodeTransform.Orientation;
				}
				int skf = ba->GetScaleKeyframeByTime((float)framenumber);
				if (skf == -1) {
					ba->ScaleKeyframes.push_back(ScaleKeyframe((float)framenumber, bone->NodeTransform.Scale));
				}
				else {
					auto& sk = ba->ScaleKeyframes[skf];
					sk.Value = bone->NodeTransform.Scale;
				}
			}
		}
		int x = 0;

	}
}

void AnimationComponent::UpdateKeyframe(int framenumber)
{
	if (m_edit_animation > -1) {
		//assert(m_edit_animation < m_animations.size(), "Animation index out of range.");
		Animation& anim = this->m_animations[m_edit_animation];
		for (auto& bone : m_skeleton->Bones) {
			BoneAnimation* ba = anim.GetBoneAnimationByBoneIndex(bone->Id);
			if (ba != nullptr) {

				int pkf = ba->GetPositionKeyframeByTime((float)framenumber);
				auto& pk = ba->PositionKeyframes[pkf];
				pk.Value = bone->NodeTransform.Position;

				int rkf = ba->GetRotationKeyframeByTime((float)framenumber);
				auto& rk = ba->RotationKeyframes[rkf];
				rk.Value = bone->NodeTransform.Orientation;

				int skf = ba->GetScaleKeyframeByTime((float)framenumber);
				auto& sk = ba->ScaleKeyframes[skf];
				sk.Value = bone->NodeTransform.Scale;
			}
		}
	}
}

void AnimationComponent::DeleteKeyframe(int framenumber)
{
	if (m_edit_animation > -1) {
		//assert(m_edit_animation < m_animations.size(), "Animation index out of range.");
		Animation& anim = this->m_animations[m_edit_animation];
		for (auto& bone : m_skeleton->Bones) {
			BoneAnimation* ba = anim.GetBoneAnimationByBoneIndex(bone->Id);
			if (ba != nullptr) {
				ba->PositionKeyframes.erase(std::remove_if(ba->PositionKeyframes.begin(), ba->PositionKeyframes.end(), [&](PositionKeyframe& pkf) {
					return pkf.Time == float(framenumber);
				}));
				ba->RotationKeyframes.erase(std::remove_if(ba->RotationKeyframes.begin(), ba->RotationKeyframes.end(), [&](RotationKeyframe& pkf) {
					return pkf.Time == float(framenumber);
				}));

				ba->ScaleKeyframes.erase(std::remove_if(ba->ScaleKeyframes.begin(), ba->ScaleKeyframes.end(), [&](ScaleKeyframe& pkf) {
					return pkf.Time == float(framenumber);
				}));
			}
		}
		anim.Keyframes.erase(std::remove_if(anim.Keyframes.begin(), anim.Keyframes.end(), [&](Keyframe& kf) {
			return kf.Index == framenumber;
		}));
	}
}

void AnimationComponent::PlayAnimation(int index)
{
	assert(index < m_animations.size());
	m_currentAnimationIndex = index;
	PlaybackStartTime = clock::now();
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

int BoneAnimation::GetRotationKeyframeByTime(float time)
{
	for (int i = 0; i < RotationKeyframes.size(); ++i) {
		auto& rkf = RotationKeyframes[i];
		if (rkf.Time == time) {
			return i;
		}
	}
	return -1;
}

int BoneAnimation::GetPositionKeyframeByTime(float time)
{
	for (int i = 0; i < PositionKeyframes.size(); ++i) {
		auto& pkf = PositionKeyframes[i];
		if (pkf.Time == time) {
			return i;
		}
	}
	return -1;
}

int BoneAnimation::GetScaleKeyframeByTime(float time)
{
	for (int i = 0; i < ScaleKeyframes.size(); ++i) {
		auto& skf = ScaleKeyframes[i];
		if (skf.Time == time) {
			return i;
		}
	}
	return -1;
}


BoneAnimation* Animation::GetBoneAnimationByBoneIndex(int idx)
{
	for (int i = 0; i < static_cast<int>(BoneAnimations.size()); ++i) {
		auto& ba = BoneAnimations[i];
		if (ba.BoneIndex == idx) {
			return &ba;
		}
	}
	return nullptr;
}

} // Jasper
