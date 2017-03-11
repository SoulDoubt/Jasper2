#include "Model.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iterator>
#include <sstream>

#include "Common.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "PhysicsCollider.h"
#include "Scene.h"
#include <string>
#include <iostream>
#include <fstream>
#include "AssetSerializer.h"
#include "AnimationSystem.h"

#include <PythonInterface.h>




//#include "VHACD.h"

namespace Jasper
{

using namespace std;



Transform ImporterSceneNode::ConcatParentTransforms()
{
	Transform t = NodeTransform;
	ImporterSceneNode* parent = this->Parent;
	while (parent != nullptr) {
		t = parent->NodeTransform * t;
		parent = parent->Parent;
	}
	return t;
}

ImporterSceneNode aiNodeToImporterSceneNode(aiNode* n) {
	ImporterSceneNode isn;
	isn.Name = n->mName.data;
	isn.NodeTransform = aiMatrix4x4ToTransform(n->mTransformation);
	for (int i = 0; i < n->mNumChildren; ++i) {
		isn.Children.push_back(aiNodeToImporterSceneNode(n->mChildren[i]));
	}
	return isn;
}

ImporterSceneNode::ImporterSceneNode(aiNode * node)
{
	*this = aiNodeToImporterSceneNode(node);
}

aiNode* FindAiNode(aiNode* root, const string& name)
{
	if (string(root->mName.data) == name) {
		return root;
	}
	for (int i = 0; i < root->mNumChildren; ++i) {
		aiNode* child = root->mChildren[i];
		auto t = FindAiNode(child, name);
		if (t) {
			return t;
		}
	}
	return nullptr;
}


//void ModelLoader::BuildSkeleton(Skeleton * skel)
//{	
//	BuildSkeletonRecursive(&ImporterSceneRoot, skel);
//}
//
//void ModelLoader::BuildSkeletonRecursive(ImporterSceneNode * node, Skeleton * skel)
//{
//	if (node->isUsedBone) {
//		int id = skel->Bones.size();
//		auto bone = make_unique<BoneData>();
//		
//	}
//}

ModelLoader::ModelLoader(Scene* scene)
	: m_scene(scene)
{

}


ModelLoader::~ModelLoader()
{

}

std::unique_ptr<PhysicsCollider> GenerateSinglePhysicsCollider(ModelData* md, Scene* scene, PHYSICS_COLLIDER_TYPE type)
{
	Vector3 MaxExtents = { -100000.0f, -1000000.0f, -1000000.0f };
	Vector3 MinExtents = { 1000000.0f, 1000000.0f, 1000000.0f };

	for (auto m : md->GetMeshes()) {
		//int TriCount += m->Indices.size() / 3;
		//this->VertCount += m->Positions.size();
		if (m->MaxExtents().x > MaxExtents.x) MaxExtents.x = m->MaxExtents().x;
		if (m->MaxExtents().y > MaxExtents.y) MaxExtents.y = m->MaxExtents().y;
		if (m->MaxExtents().z > MaxExtents.z) MaxExtents.z = m->MaxExtents().z;

		if (m->MinExtents().x < MinExtents.x) MinExtents.x = m->MinExtents().x;
		if (m->MinExtents().y < MinExtents.y) MinExtents.y = m->MinExtents().y;
		if (m->MinExtents().z < MinExtents.z) MinExtents.z = m->MinExtents().z;
	}
	Vector3 hes = { (MaxExtents.x - MinExtents.x) / 2, (MaxExtents.y - MinExtents.y) / 2, (MaxExtents.z - MinExtents.z) / 2 };
	unique_ptr<PhysicsCollider> collider = nullptr;
	switch (type) {
	case PHYSICS_COLLIDER_TYPE::Box:
		collider = make_unique<BoxCollider>(md->GetName() + "_Collider_", hes, scene->GetPhysicsWorld());
		break;
	case PHYSICS_COLLIDER_TYPE::Capsule:
		collider = make_unique<CapsuleCollider>(md->GetName() + "_Collider_", hes, scene->GetPhysicsWorld());
		break;
	case PHYSICS_COLLIDER_TYPE::Sphere:
		collider = make_unique<SphereCollider>(md->GetName() + "_Collider_", hes, scene->GetPhysicsWorld());
		break;
	case PHYSICS_COLLIDER_TYPE::Cylinder:
		collider = make_unique<CylinderCollider>(md->GetName() + "_Collider_", hes, scene->GetPhysicsWorld());
		break;
	case PHYSICS_COLLIDER_TYPE::ConvexHull: {
		//ConvexHullCollider* cvx = GetGameObject()->AttachNewComponent<ConvexHullCollider>(this->GetName() + "_Collider_", hes, m_physicsWorld);
		//cvx->InitFromMeshes(meshes);
		//collider = cvx;
		break;
	}
	case PHYSICS_COLLIDER_TYPE::Compound: {
		//        vector<unique_ptr<btConvexHullShape>> hulls;
		//        for (auto m : m_modelMeshes) {
		//            ConvexDecompose(m, hulls, jScene);
		//        }
		//        printf("Created %d convex hulls in model.\n", (int)hulls.size());
		//        CompoundCollider* cmp = GetGameObject()->AttachNewComponent<CompoundCollider>(this->GetName() + "_collider"s, hulls, m_physicsWorld);
		//        collider = cmp;
		break;
	}
	}
	if (collider) {
		collider->Mass = 10.f;
		collider->Restitution = 0.56f;
		collider->Friction = 0.34f;
	}
	return move(collider);
}



std::unique_ptr<GameObject> ModelLoader::CreateModelInstance(const string& name, const string& modelName, bool generateCollider, bool splitColliders)
{
	auto go = make_unique<GameObject>(name);
	//auto child = make_unique<GameObject>(name + "_model_instance"s);
	auto modeldata = m_scene->GetModelCache().GetResourceByName(modelName);
	if (modeldata) {
		for (const auto mesh : modeldata->GetMeshes()) {
			auto mat = mesh->GetMaterial();
			if (!mat) {
				printf("%s had no material bound.\n", mesh->GetName().c_str());
			}
			go->AttachNewComponent<MeshRenderer>(mesh->GetName() + "_renderer", mesh, mat);
		}
		if (generateCollider && modeldata->GetSkeleton()->Bones.size() == 0) {
			unique_ptr<PhysicsCollider> collider = GenerateSinglePhysicsCollider(modeldata, m_scene, PHYSICS_COLLIDER_TYPE::Box);
			go->AttachComponent(move(collider));
		}
		else if (generateCollider && modeldata->GetSkeleton()->Bones.size() > 0) {
			//modeldata->CreateRagdollCollider(m_scene, go.get());
		}
		if (modeldata->Animator) {
			go->AttachComponent(move(modeldata->Animator));
		}
		if (modeldata->m_skeleton) {
			go->AttachNewComponent<SkeletonComponent>(m_name + "_skeleton", modeldata->m_skeleton);
		}
		//child->SetLocalTransform(modeldata->GetSkeleton()->GlobalInverseTransform);
	}
	

	//go->AttachChild(move(child));
	return move(go);
}


void CreateChildHulls(BoneData* rootBone, Skeleton* skeleton, vector<unique_ptr<btCollisionShape>>& hulls)
{
	for (size_t j = 0; j < rootBone->Children.size(); j++) {
		unique_ptr<btCollisionShape> hull = make_unique<btConvexHullShape>(nullptr, 0);
		auto h = static_cast<btConvexHullShape*>(hull.get());
		BoneData* bone = skeleton->Bones[rootBone->Children[j]].get();
		std::sort(rootBone->Weights.begin(), rootBone->Weights.end());
		std::sort(bone->Weights.begin(), bone->Weights.end());
		vector<VertexBoneWeight> uniqueChildWeights;

		std::set_difference(bone->Weights.begin(), bone->Weights.end(), rootBone->Weights.begin(), rootBone->Weights.end(), std::inserter(uniqueChildWeights, uniqueChildWeights.begin()));
		for (size_t i = 0; i < uniqueChildWeights.size(); ++i) {
			VertexBoneWeight& vb = uniqueChildWeights[i];
			//if (vb.Weight >= 0.75f) {
				Vector3 position = vb.mesh->Positions[vb.Index];
				h->addPoint(position.AsBtVector3());
			//}
		}
		hulls.emplace_back(move(hull));
		CreateChildHulls(bone, skeleton, hulls);
	}
}

void ModelData::CreateRagdollCollider(Scene* scene, GameObject* go)
{
	vector<unique_ptr<btCollisionShape>> hulls;
	BoneData* rootBone = m_skeleton->Bones[m_skeleton->m_boneMap[this->GetSkeleton()->RootBoneName]].get();
	size_t vertCount = rootBone->Weights.size();
	vector<Vector3> boneVerts(vertCount);
	unique_ptr<btCollisionShape> hullShape = make_unique<btConvexHullShape>(nullptr, 0);
	auto hs = static_cast<btConvexHullShape*>(hullShape.get());
	for (size_t i = 0; i < vertCount; ++i) {
		VertexBoneWeight& vb = rootBone->Weights[i];
		if (vb.Weight > 0.75f) {
			Vector3 position = vb.mesh->Positions[vb.Index];
			hs->addPoint(position.AsBtVector3());
		}
	}
	hulls.emplace_back(move(hullShape));
	CreateChildHulls(rootBone, m_skeleton, hulls);



	unique_ptr<RagdollCollider> collider = make_unique<RagdollCollider>(rootBone->Name + "_collider"s, hulls, scene->GetPhysicsWorld());
	//collider->Mass = 50.f;
	go->AttachComponent(std::move(collider));
	printf("Created Ragdoll.\n");


}

bool FileExists(const std::string& name)
{
	if (FILE *file = fopen(name.c_str(), "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

void BuildBoneTreeRecursive(BoneData* parent, Skeleton* skeleton) {
/*	for (int i : parent->Children) {
		BoneData* child = skeleton->Bones[i].get();
		child->InverseBindTransform = parent->InverseBindTransform * child->NodeTransform;
		BuildBoneTreeRecursive(child, skeleton);
	}*/
}

void BuildBoneTree(ImporterSceneNode* rootBoneNode, Skeleton* skeleton) {
	int boneIndex = skeleton->m_boneMap.at(rootBoneNode->Name);
	auto& bone = skeleton->Bones[boneIndex];
	if (bone->ParentID == -1) {
		// is this is the root bone, we will need to concat any other parent transfoms here
		auto parent = rootBoneNode->Parent;
		while (parent != nullptr) {
			bone->NodeTransform = parent->NodeTransform * bone->NodeTransform;
			parent = parent->Parent;
		}
		//bone->InverseBindTransform = bone->NodeTransform.Inverted();
		for (int i = 0; i < bone->Children.size(); ++i) {
			int childIndex = bone->Children.at(i);
			BoneData* childBone = skeleton->Bones.at(childIndex).get();

		}
	}

}



void BuildBoneHierarchy(ImporterSceneNode* rootNode, Skeleton* skeleton) {

	int boneIndex = skeleton->m_boneMap.at(rootNode->Name);
	auto& bone = skeleton->Bones[boneIndex];
	//if (bone->ParentID == -1) {
	//	auto parent = rootNode->Parent;
	//	while (parent != nullptr) {
	//		bone->NodeTransform = parent->NodeTransform * bone->NodeTransform;
	//		parent = parent->Parent;
	//	}
	//	//bone->InverseBindTransform = bone->BoneTransform.Inverted();
	//}
	for (auto& childNode : rootNode->Children) {
		auto childBoneIter = skeleton->m_boneMap.find(childNode.Name);
		if (childBoneIter == skeleton->m_boneMap.end()) {
			printf("Missing bone: %s, creating one...\n", childNode.Name.data());
			//BoneData newbd;
			//newbd.BoneOffsetTransform = Transform(); // assume identity is fine for thie???
			//newbd.Name = childNode.Name;
			//newbd.BoneTransform = childNode.NodeTransform;
			//newbd.Id = (int)skeleton->Bones.size();
			//newbd.ParentID = bone.Id;
			//skeleton->m_boneMap[newbd.Name] = newbd.Id;
			//skeleton->Bones.push_back(newbd);
			//BuildBoneHierarchy(&childNode, skeleton);
		}
		else {
			int childId = childBoneIter->second;
			bone->Children.push_back(childId);
			auto& childBone = skeleton->Bones.at(childId);
			childBone->ParentID = bone->Id;
			//childBone->InverseBindTransform = bone->InverseBindTransform * childBone->NodeTransform;
			BuildBoneHierarchy(childBone->INode, skeleton);
		}
	}
}

void ModelData::CreateImporterSceneGraph(aiNode* rootNode) {
	m_importerSceneRoot = make_unique<ImporterSceneNode>(rootNode);
	BuildSceneRecursive(m_importerSceneRoot.get(), nullptr);
}

void SetYUp(ModelData* md){
	Transform rot;
	rot.Orientation = Quaternion::FromAxisAndAngle({ 1.f, 0.f, 0.f }, DEG_TO_RAD(-90.f));
	/*for (auto mesh : md->GetMeshes()) {
		for (auto& v : mesh->Positions) {
			v = rot * v;
		}
	}*/
	/*if (md->GetSkeleton()) {
		auto skel = md->GetSkeleton();
		skel->RootBone->NodeTransform = rot * skel->RootBone->NodeTransform;		
	}*/
}

void ModelLoader::CenterOnOrigin(std::vector<Jasper::Mesh *> & meshes, Skeleton* skeleton)
{
	MaxExtents = { -1000000.0f, -1000000.0f, -1000000.0f };
	MinExtents = { 1000000.0f, 1000000.0f, 1000000.0f };

	for (auto m : meshes) {

		this->TriCount += m->Indices.size() / 3;
		this->VertCount += m->Positions.size();
		if (m->MaxExtents().x > MaxExtents.x) MaxExtents.x = m->MaxExtents().x;
		if (m->MaxExtents().y > MaxExtents.y) MaxExtents.y = m->MaxExtents().y;
		if (m->MaxExtents().z > MaxExtents.z) MaxExtents.z = m->MaxExtents().z;

		if (m->MinExtents().x < MinExtents.x) MinExtents.x = m->MinExtents().x;
		if (m->MinExtents().y < MinExtents.y) MinExtents.y = m->MinExtents().y;
		if (m->MinExtents().z < MinExtents.z) MinExtents.z = m->MinExtents().z;
	}

	Vector3 localOrigin = { (MinExtents.x + MaxExtents.x) / 2.f, (MinExtents.y + MaxExtents.y) / 2.f , (MinExtents.z + MaxExtents.z) / 2.f };
	float epsilon = 0.000001f;
	if (fabs(localOrigin.x) > epsilon || fabs(localOrigin.y) > epsilon || fabs(localOrigin.z) > epsilon) {
		for (auto& m : meshes) {
			/*for (auto& v : m->Positions) {
				v -= localOrigin;
			}
			m->CalculateExtents();*/
			if (skeleton != nullptr) {
				for (auto& bone : skeleton->Bones) {
					if (!bone->PositionCorrected) {
						bone->NodeTransform.Position += localOrigin;
						bone->PositionCorrected = true;
					}
				}
			}
		}
	}
}

void ModelLoader::LoadModel(const std::string& filename, const std::string& name)
{
	m_processedMeshCount = 0;
	m_name = name;
	Assimp::Importer importer;
	printf("Loading model: %s\n", filename.c_str());
	if (!FileExists(filename)) {
		printf("File: %s does not exist.", filename.c_str());
	}
	const aiScene* scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs);

	if (!scene) {
		scene = importer.ReadFile(filename, 0);
	}

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf("aiScene was corrupt in model load.\n");
		return;
	}

	ModelData* model_data = m_scene->GetModelCache().CreateInstance<ModelData>(m_name);
	string directory = filename.substr(0, filename.find_last_of("/"));

	// ensure that we have a local copy of the assimp scene before mesh processing
	model_data->CreateImporterSceneGraph(scene->mRootNode);
	// process the scene and meshes...
	ProcessAiSceneNode(scene, scene->mRootNode, directory, model_data);

	auto& meshes = model_data->GetMeshes();
	size_t sz = meshes.size();
	printf("Loaded %d meshes in model: %s\n", sz, m_name.c_str());

	

	if (model_data->GetSkeleton()->Bones.size() > 0)
	{
		//CenterOnOrigin(meshes, model_data->GetSkeleton());
	}
	else {
		//CenterOnOrigin(meshes, nullptr);
	}

	// recurse the isn graph and find any bones that are are used in this model...


	if (model_data->GetSkeleton()->Bones.size() > 0) {
	
		Skeleton* skeleton = model_data->GetSkeleton();

		skeleton->GlobalInverseTransform = model_data->ImporterSceneRoot()->NodeTransform;

		auto rootBoneNode = skeleton->GetRootBoneNode();
		assert(rootBoneNode != nullptr);
		skeleton->RootBoneName = rootBoneNode->Name;
		skeleton->Bones[skeleton->m_boneMap[skeleton->RootBoneName]]->ParentID = -1;
		skeleton->GlobalInverseTransform = rootBoneNode->NodeTransform.Inverted();
		BuildBoneHierarchy(rootBoneNode, skeleton);
		//skeleton->BuildIntoHierarchy();
		auto rootBone = skeleton->GetBone(skeleton->RootBoneName);
		//rootBone->CalculateInverseBindTransforms(Transform());
		//rootBone->CalculateInverseBindTransform(Transform());

		printf("There are %d Bones in the skeleton:\n", skeleton->Bones.size());


		for (size_t i = 0; i < skeleton->Bones.size(); ++i) {
			BoneData* bd = skeleton->Bones[i].get();
			printf("Bone ID: %d, Name: %s Has: %d children. Position: %s\n", bd->Id, bd->Name.data(), bd->Children.size(), bd->NodeTransform.Position.ToString().data());
		}

		if (scene->HasAnimations()) {
			model_data->Animator = make_unique<AnimationComponent>(model_data->GetName(), skeleton);
			for (int i = 0; i < scene->mNumAnimations; ++i) {
				auto aiAnim = scene->mAnimations[i];
				Animation anim;
				anim.Name = aiAnim->mName.data;
				anim.Ticks = aiAnim->mDuration;
				anim.TicksPerSecond = aiAnim->mTicksPerSecond;
				anim.Duration = aiAnim->mDuration;
				anim.BoneAnimations.reserve(aiAnim->mNumChannels);
				for (int j = 0; j < aiAnim->mNumChannels; ++j) {
					auto channel = aiAnim->mChannels[j];
					if (skeleton->m_boneMap.find(channel->mNodeName.data) != skeleton->m_boneMap.end()) {
						printf("Animation Channel Name: %s has %d rotation keys.\n", channel->mNodeName.data, channel->mNumPositionKeys);
						BoneAnimation boneAnim;
						boneAnim.Name = channel->mNodeName.data;
						boneAnim.BoneIndex = skeleton->m_boneMap[boneAnim.Name];
						boneAnim.RotationKeyframes.reserve(channel->mNumRotationKeys);
						boneAnim.PositionKeyframes.reserve(channel->mNumPositionKeys);
						boneAnim.ScaleKeyframes.reserve(channel->mNumScalingKeys);

						for (int k = 0; k < channel->mNumRotationKeys; ++k) {
							auto airot = channel->mRotationKeys[k];
							RotationKeyframe rk;
							rk.Time = (float)airot.mTime;
							rk.Value = aiQuaternionToQuatenrion(airot.mValue);
							boneAnim.RotationKeyframes.emplace_back(rk);
						}

						for (int k = 0; k < channel->mNumPositionKeys; ++k) {
							auto aipos = channel->mPositionKeys[k];
							PositionKeyframe pk;
							pk.Time = (float)aipos.mTime;
							pk.Value = aiVector3ToVector3(aipos.mValue);
							boneAnim.PositionKeyframes.emplace_back(pk);
						}

						for (int k = 0; k < channel->mNumScalingKeys; ++k) {
							auto ais = channel->mScalingKeys[k];
							ScaleKeyframe sk;
							sk.Time = (float)ais.mTime;
							sk.Value = aiVector3ToVector3(ais.mValue);
							boneAnim.ScaleKeyframes.emplace_back(sk);
						}
						anim.BoneAnimations.emplace_back(boneAnim);
					}

				}

				printf("Found animation named: %s", aiAnim->mName.data);
				anim.Index = (int)model_data->Animator->GetAnimations().size();
				model_data->Animator->AddAnimation(move(anim));
			}
		}

		if (1) {
			int boneIdToMove = skeleton->m_boneMap["lShoulder_0_"];
			int boneid2 = skeleton->m_boneMap["lUpperArm_0_"];
			BoneData* boneToMove = skeleton->Bones[boneIdToMove].get();
			BoneData* boneToMove2 = skeleton->Bones[boneid2].get();
			//boneToMove.BoneOffsetMatrix.Translate(Vector3(0.25f, 0.f, 0.f));
			Quaternion q = Quaternion::FromAxisAndAngle(Vector3(0.f, 1.f, 0.f), DEG_TO_RAD(45));
			Transform t = boneToMove->NodeTransform;
			t.Orientation *= q;// *t.Orientation;
			boneToMove->NodeTransform = t;
			boneToMove2->NodeTransform = t;
			
			//boneToMove->EvaluateSubtree();
		}


	}
	SetYUp(model_data);

	for (auto& mesh : model_data->GetMeshes()) {

		mesh->SetSkeleton(model_data->GetSkeleton());
	}


}

vector<tinyxml2::XMLNode*> GetChildren(tinyxml2::XMLNode* parent) {
	using namespace tinyxml2;
	vector<XMLNode*> result;
	if (parent->NoChildren()) {
		return result;
	}
	auto ch = parent->FirstChild();
	result.push_back(ch);
	auto sib = ch->NextSibling();
	while (sib) {
		result.push_back(sib);
		sib = sib->NextSibling();
	}
	return result;
}

tinyxml2::XMLNode* GetNodeByID(tinyxml2::XMLNode* rootNode, const std::string& id) {
	auto elem = rootNode->ToElement();
	if (!elem)return nullptr;
	auto nid = elem->Attribute("id");
	if (nid) {
		string nodeid = nid;
		if (nodeid == id || "#" + nodeid == id) {
			return rootNode;
		}
	}
	auto children = GetChildren(rootNode);
	for (auto child : children) {
		auto s = GetNodeByID(child, id);
		if (s) return s;
	}
	return nullptr;
}

vector<tinyxml2::XMLNode*> FindChildNodes(tinyxml2::XMLNode* parent, const string& name) {
	vector<tinyxml2::XMLNode*> result;
	/*if (string(parent->Value()) == name) {

	}
	auto children = GetChildren(parent);
	for (auto c : children) {
		)
	}*/
	return result;
}

vector<tinyxml2::XMLNode*> GetChildNodesByName(tinyxml2::XMLNode* parent, const string& name) {
	using namespace tinyxml2;
	vector<XMLNode*> matches;
	auto ch = parent->FirstChild();
	if (string(ch->Value()) == name) {
		matches.push_back(ch);
		auto sib = ch->NextSibling();
		while (sib) {
			if (string(sib->Value()) == name) {
				matches.push_back(sib);
			}
			sib = sib->NextSibling();
		}
	}
	else {
		auto sib = ch->NextSibling();
		while (sib) {
			if (string(sib->Value()) == name) {
				matches.push_back(sib);
			}
			sib = sib->NextSibling();
		}
	}
	return matches;
}

tinyxml2::XMLNode* GetChildNode(tinyxml2::XMLNode* parent, const string& name) {
	using namespace tinyxml2;

	vector<XMLNode*> children;
	if (!parent->NoChildren()) {
		auto child = parent->FirstChild();
		if (string(child->Value()) == name) {
			return child;
		}
		children.push_back(child);
		auto sib = child->NextSibling();
		while (sib) {
			if (string(sib->Value()) == name) {
				return sib;
			}
			children.push_back(sib);
			sib = sib->NextSibling();
		}
	}
	for (auto ch : children) {
		auto x = GetChildNode(ch, name);
		if (x) return x;
	}
	return nullptr;
}

tinyxml2::XMLNode* GetChildNodeByNameAndAttribute(tinyxml2::XMLNode* parent, const string& nodeName, const string& attrName, const string& attrValue) {
	auto nodes = GetChildNodesByName(parent, nodeName);
	for (auto node : nodes) {
		string attrval = node->ToElement()->Attribute(attrName.c_str());
		if (attrval == attrValue || "#" + attrval == attrValue) {
			return node;
		}
	}
}



unique_ptr<ColladaMesh> ModelLoader::BuildXmlMesh(tinyxml2::XMLNode* meshNode) {

	auto vertsNode = GetChildNode(meshNode, "vertices"s);
	auto polylistNode = GetChildNode(meshNode, "polylist"s);
	auto trianglesNode = GetChildNode(meshNode, "triangles"s);

	if (!polylistNode && trianglesNode) {
		return BuildTriangleListMesh(meshNode);
	}
	else if (polylistNode && !trianglesNode) {
		return BuildPolylistMesh(meshNode);
	}
	return nullptr;
}

unique_ptr<ColladaMesh> ModelLoader::BuildPolylistMesh(tinyxml2::XMLNode * meshNode)
{
	unique_ptr<ColladaMesh> colladaMesh = make_unique<ColladaMesh>();

	auto polylistNode = GetChildNode(meshNode, "polylist"s);
	auto inputs = GetChildNodesByName(polylistNode, "input"s);

	string vertexSourceID;
	string normalSourceID;
	string texcoordSourceID;
	string positionSourceID;
	for (auto input : inputs) {
		string semantic = input->ToElement()->Attribute("semantic");
		if (semantic == "VERTEX") {
			vertexSourceID = input->ToElement()->Attribute("source");
		}
		if (semantic == "NORMAL") {
			normalSourceID = input->ToElement()->Attribute("source");
		}
		if (semantic == "TEXCOORD") {
			texcoordSourceID = input->ToElement()->Attribute("source");
		}
	}
	auto vertexSource = GetChildNodeByNameAndAttribute(meshNode, "vertices"s, "id"s, vertexSourceID);
	auto positionSourceInputs = GetChildNodesByName(vertexSource, "input"s);
	for (auto input : positionSourceInputs) {
		string semantic = input->ToElement()->Attribute("semantic");
		if (semantic == "POSITION"s) {
			positionSourceID = input->ToElement()->Attribute("source");
		}
	}
	auto positionSource = GetChildNodeByNameAndAttribute(meshNode, "source"s, "id"s, positionSourceID);
	string meshName = "collada_loader_mesh"s;
	auto mesh = m_scene->GetMeshCache().CreateInstance<Mesh>(meshName);
	// at the position node the position data will be...
	auto floatNode = GetChildNode(positionSource, "float_array"s);
	auto posElem = floatNode->ToElement();
	int posCount = posElem->IntAttribute("count");
	istringstream floatstr(posElem->GetText());
	vector<float> floats;
	floats.reserve(posCount);
	string fs;
	while (getline(floatstr, fs, ' ')) {
		float pf = stof(fs);
		floats.emplace_back(pf);
	}
	//vector<Vector3> positions;
	colladaMesh->Positions.reserve(floats.size() / 3);
	for (int i = 0; i < posCount / 3; ++i) {
		Vector3 p = { floats[i * 3], floats[i * 3 + 1], floats[i * 3 + 2] };
		//positions.push_back(p);
		colladaMesh->Positions.emplace_back(p);
	}
	floats.clear();
	// normals
	auto normalSource = GetChildNodeByNameAndAttribute(meshNode, "source"s, "id"s, normalSourceID);
	floatNode = GetChildNode(normalSource, "float_array"s);
	auto normalElem = floatNode->ToElement();
	int normalCount = normalElem->IntAttribute("count");
	floats.reserve(normalCount);
	floatstr = istringstream(normalElem->GetText());
	while (getline(floatstr, fs, ' ')) {
		float nf = stof(fs);
		floats.emplace_back(nf);
	}
	//vector<Vector3> normals;
	colladaMesh->Normals.reserve(normalCount / 3);
	for (int i = 0; i < normalCount / 3; ++i) {
		Vector3 p = { floats[i * 3], floats[i * 3 + 1], floats[i * 3 + 2] };
		//normals.push_back(p);
		colladaMesh->Normals.emplace_back(p);
	}
	floats.clear();

	auto texcoordSource = GetChildNodeByNameAndAttribute(meshNode, "source"s, "id"s, texcoordSourceID);
	floatNode = GetChildNode(texcoordSource, "float_array"s);
	auto texcoordElem = floatNode->ToElement();
	int texcoordCount = texcoordElem->IntAttribute("count");
	floats.reserve(texcoordCount);
	floatstr = istringstream(texcoordElem->GetText());

	while (getline(floatstr, fs, ' ')) {
		float tcf = stof(fs);
		floats.emplace_back(tcf);
	}
	//vector<Vector2> texCoords;
	colladaMesh->TexCoords.reserve(floats.size() / 2);
	for (int i = 0; i < texcoordCount / 2; ++i) {
		Vector2 p = { floats[i * 2], floats[i * 2 + 1] };		
		colladaMesh->TexCoords.emplace_back(p);
	};

	// now for the indices...
	int inputCount = inputs.size();
	auto indexNode = GetChildNode(polylistNode, "p"s);
	istringstream intstr(indexNode->ToElement()->GetText());
	vector<int> ints;
	while (getline(intstr, fs, ' ')) {
		ints.emplace_back(atoi(fs.data()));
	}
	//mesh->Positions.resize(positions.size());
	//mesh->Normals.resize(normals.size());
	//mesh->TexCoords.resize(texCoords.size());
	//vector<int> positionsVisited;
	int dupCount = 0;
	for (int i = 0; i < ints.size() / inputCount; ++i) {

		int pi = ints[i * inputCount];
		int ni = ints[i * inputCount + 1];
		int ti = ints[i * inputCount + 2];
		colladaMesh->PositionIndices.emplace_back((uint32_t)pi);
		colladaMesh->NormalIndices.emplace_back((uint32_t)ni);
		colladaMesh->TexCoordIndices.emplace_back((uint32_t)ti);				

		//Vector3 p = positions[pi];
		//Vector3 n = normals[ni];
		//Vector2 t = texCoords[ti];						

		//mesh->Positions.push_back(p);
		//mesh->Normals.push_back(n);
		//mesh->TexCoords.push_back(t);

		//mesh->Indices.push_back(i);
		//dupCount++;
	}
	
	//auto mat = m_scene->GetMaterialCache().CreateInstance<Material>("stupid_material");
	//mat->Diffuse = { 1.f, 0.f, 1.f };
	//mat->Flags &= Material::MATERIAL_FLAGS::USE_MATERIAL_COLOR;
	//mesh->SetMaterial(mat);
	//mesh->CalculateTangentSpace();
	//mesh->CalculateExtents();
	
	return move(colladaMesh);
	//return nullptr;
}

unique_ptr<ColladaMesh> ModelLoader::BuildTriangleListMesh(tinyxml2::XMLNode * meshNode)
{
	unique_ptr<ColladaMesh> colladaMesh = make_unique<ColladaMesh>();
	auto vertsNode = GetChildNode(meshNode, "vertices"s);
	auto inputs = GetChildNodesByName(vertsNode, "input"s);

	string positionSourceID;
	string normalSourceID;
	string texcoordSourceID;
	for (auto input : inputs) {
		string semantic = input->ToElement()->Attribute("semantic");
		if (semantic == "POSITION") {
			positionSourceID = input->ToElement()->Attribute("source");
		}
		if (semantic == "NORMAL") {
			normalSourceID = input->ToElement()->Attribute("source");
		}
		if (semantic == "TEXCOORD") {
			texcoordSourceID = input->ToElement()->Attribute("source");
		}
	}

	// positions
	auto positionSource = GetChildNodeByNameAndAttribute(meshNode, "source"s, "id"s, positionSourceID);
	string meshName = positionSource->ToElement()->Attribute("name");
	auto mesh = m_scene->GetMeshCache().CreateInstance<Mesh>(meshName);
	// at the position node the position data will be...
	auto floatNode = GetChildNode(positionSource, "float_array");
	auto posElem = floatNode->ToElement();
	int posCount = posElem->IntAttribute("count");
	istringstream floatstr(posElem->GetText());
	vector<float> floats;
	floats.reserve(posCount);
	string fs;
	while (getline(floatstr, fs, ' ')) {
		float pf = stof(fs);
		floats.emplace_back(pf);
	}
	colladaMesh->Positions.reserve(floats.size() / 3);
	for (int i = 0; i < posCount / 3; ++i) {
		Vector3 p = { floats[i * 3], floats[i * 3 + 1], floats[i * 3 + 2] };
		colladaMesh->Positions.emplace_back(p);
	}

	floats.clear();
	// normals
	auto normalSource = GetChildNodeByNameAndAttribute(meshNode, "source"s, "id"s, normalSourceID);
	floatNode = GetChildNode(normalSource, "float_array"s);
	auto normalElem = floatNode->ToElement();
	int normalCount = normalElem->IntAttribute("count");
	floats.reserve(normalCount);
	floatstr = istringstream(normalElem->GetText());
	while (getline(floatstr, fs, ' ')) {
		float nf = stof(fs);
		floats.emplace_back(nf);
	}
	colladaMesh->Normals.reserve(floats.size() / 3);	
	for (int i = 0; i < normalCount / 3; ++i) {
		Vector3 p = { floats[i * 3], floats[i * 3 + 1], floats[i * 3 + 2] };
		colladaMesh->Normals.emplace_back(p);
	}
	floats.clear();

	auto texcoordSource = GetChildNodeByNameAndAttribute(meshNode, "source"s, "id"s, texcoordSourceID);
	floatNode = GetChildNode(texcoordSource, "float_array"s);
	auto texcoordElem = floatNode->ToElement();
	int texcoordCount = texcoordElem->IntAttribute("count");
	floats.reserve(texcoordCount);
	floatstr = istringstream(texcoordElem->GetText());
	while (getline(floatstr, fs, ' ')) {
		float tcf = stof(fs);
		floats.emplace_back(tcf);
	}
	colladaMesh->TexCoords.reserve(floats.size() / 2);
	for (int i = 0; i < texcoordCount / 2; ++i) {
		Vector2 p = { floats[i * 2], floats[i * 2 + 1] };
		colladaMesh->TexCoords.emplace_back(p);
	};

	// now for the indices...	
	auto indexNode = GetChildNode(meshNode, "triangles"s);
	auto indexInputs = GetChildNodesByName(indexNode, "input");
	int inputCount = indexInputs.size();
	for (auto inp : indexInputs) {
		auto listNode = GetChildNode(indexNode, "p"s);
		istringstream intstr(listNode->ToElement()->GetText());
		vector<int> ints;
		while (getline(intstr, fs, ' ')) {
			ints.emplace_back(atoi(fs.data()));
		}
		int dupCount = 0;
		colladaMesh->NormalIndices.reserve(ints.size());
		colladaMesh->PositionIndices.reserve(ints.size());
		colladaMesh->TexCoordIndices.reserve(ints.size());
		for (int i = 0; i < ints.size() / inputCount; ++i) {
			
			int pi = ints[i * inputCount];
			colladaMesh->PositionIndices.push_back(ints[i * inputCount]);
			colladaMesh->NormalIndices.push_back(ints[i * inputCount]);
			colladaMesh->TexCoordIndices.push_back(ints[i * inputCount]);
			/*Vector3 p = positions[pi];
			Vector3 n = normals[pi];
			Vector2 t = texCoords[pi];

			mesh->Positions.push_back(p);
			mesh->Normals.push_back(n);
			mesh->TexCoords.push_back(t);

			mesh->Indices.push_back(i);*/
			dupCount++;
		}
	}
	/*auto mat = m_scene->GetMaterialCache().CreateInstance<Material>("stupid_material");
	mat->Diffuse = { 1.f, 0.f, 0.2f };
	mat->Flags &= Material::MATERIAL_FLAGS::USE_MATERIAL_COLOR;
	mesh->SetMaterial(mat);
	mesh->CalculateTangentSpace();
	mesh->CalculateExtents();
	colladaMesh.Positions = move(positions);
	colladaMesh.Normals = move(normals);
	colladaMesh.TexCoords = move(texCoords);*/
	return move(colladaMesh);
}

Material* ModelLoader::BuildXmlMaterial(tinyxml2::XMLNode* materialNode, tinyxml2::XMLNode* effectsLibNode, tinyxml2::XMLNode* imagesLibNode) {
	using namespace tinyxml2;

	string materialID;
	string materialName;
	string effectID;

	materialID = materialNode->ToElement()->Attribute("id");
	materialName = materialNode->ToElement()->Attribute("name");

	auto instanceEffectNode = GetChildNode(materialNode, "instance_effect");
	effectID = instanceEffectNode->ToElement()->Attribute("url");

	auto effectNode = GetNodeByID(effectsLibNode, effectID);

	int i = 0;

	return nullptr;

}

Matrix4 ReadColladaMatrix(tinyxml2::XMLNode* matNode) {
	istringstream ss(matNode->ToElement()->GetText());
	string fs;
	float matFloats[16];
	int i = 0;
	while (getline(ss, fs, ' ')) {
		float mf = stof(fs);
		matFloats[i] = mf;
		++i;
	}
	Vector4 vecs[4];
	for (int i = 0, j = 0; i < 16; i += 4, j++) {
		Vector4 v = { matFloats[i], matFloats[i + 1], matFloats[i + 2], matFloats[i + 3] };
		vecs[j] = v;
	}
	Matrix4 mat = Matrix4(vecs[0], vecs[1], vecs[2], vecs[3]);
	// collada matricies are column major
	return mat.Transposed();
}

void RecurseXmlBones(tinyxml2::XMLNode* parentNode, Bone& parentBone) {
	// it is assumed that the bone coresponding to the parent node
	// is on the top of the bone stack
	auto childNodes = GetChildNodesByName(parentNode, "node"s);
	for (auto child : childNodes) {
		Bone b;
		b.Name = child->ToElement()->Attribute("name");
		auto matNode = GetChildNode(child, "matrix");
		Matrix4 mat = ReadColladaMatrix(matNode);
		Transform t = mat.Decompose();
		b.BindLocalTransform = t;
		parentBone.Children.push_back(b);
		RecurseXmlBones(child, parentBone.Children[parentBone.Children.size() - 1]);
	}

}

void RecursivePrintBones(const Bone& parent) {
	printf("Bone: %s Has %d children.\n", parent.Name.data(), parent.Children.size());
	for (const auto& child : parent.Children) {
		RecursivePrintBones(child);
	}
}

//std::vector<string> GetJointsList() {
//
//}

Skeleton* ModelLoader::BuildXmlSkeleton(tinyxml2::XMLNode * rootNode)
{
	using namespace tinyxml2;
	vector<Bone> bones;
	auto skeleton = m_scene->GetSkeletonCache().CreateInstance<Skeleton>(this->m_name);

	auto nodelist = GetChildNodesByName(rootNode, "node");
	for (auto n : nodelist) {
		XMLNode* jn = nullptr;
		string nodeType = n->ToElement()->Attribute("type");
		if (nodeType == "JOINT") {
			jn = n;
		}
		else {
			auto children = GetChildNodesByName(n, "node");
			for (auto cc : children) {
				nodeType = cc->ToElement()->Attribute("type");
				if (nodeType == "JOINT") {
					jn = cc;
					break;
				}
			}
		}
		if (jn) {
			auto nd = n->ToElement();
			auto matNode = GetChildNode(nd, "matrix");
			if (matNode) {
				Matrix4 mat = ReadColladaMatrix(matNode);
				Transform t = mat.Decompose();
				Bone bd;
				bd.Name = jn->ToElement()->Attribute("name");
				bd.BindLocalTransform = t;
				bones.push_back(bd);
				RecurseXmlBones(jn, bones[bones.size() - 1]);
			}
		}
	}
	
	return skeleton;
}

ColladaSkin BuildXMLSkin(tinyxml2::XMLNode* node) {
	auto skinNode = GetChildNode(GetChildNode(node, "controller"), "skin");	
	auto weightNode = GetChildNode(skinNode, "vertex_weights");
	auto jointsDataNode = GetChildNodeByNameAndAttribute(weightNode, "input", "semantic", "JOINT");
	auto weightsDataNode = GetChildNodeByNameAndAttribute(weightNode, "input", "semantic", "WEIGHT");
	string jointsDataID = jointsDataNode->ToElement()->Attribute("source");
	string weightsDataID = weightsDataNode->ToElement()->Attribute("source");
	auto jointsNode = GetChildNodeByNameAndAttribute(skinNode, "source", "id", jointsDataID);
	auto nameArrayNode = GetChildNode(jointsNode, "Name_array");

	istringstream intstr(nameArrayNode->ToElement()->GetText());
	vector<string> jointNames;
	string nm;
	while (getline(intstr, nm, ' ')) {
		jointNames.push_back(nm);
	}

	auto weightsNode = GetChildNodeByNameAndAttribute(skinNode, "source", "id", weightsDataID);

	auto weightArrayNode = GetChildNode(weightsNode, "float_array");
	auto weightsCount = weightArrayNode->ToElement()->IntAttribute("count");
	vector<float> weights;
	weights.reserve(weightsCount);
	intstr = istringstream(weightArrayNode->ToElement()->GetText());
	string wf;
	while (getline(intstr, wf, ' ')) {
		weights.push_back(stof(wf));
	}

	auto countNode = GetChildNode(weightNode, "vcount");
	vector<int> weightCounts;
	intstr = istringstream(countNode->ToElement()->GetText());
	string wc;
	while (getline(intstr, wc, ' ')) {
		weightCounts.push_back(atoi(wc.data()));
	}

	ColladaSkin skin = { jointNames, weights };

	int x = 0;
	return skin;
}



vector<std::unique_ptr<ColladaMesh>> ModelLoader::LoadColladaPython(const std::string& filename) {
	using namespace PythonInterface;
	std::vector<std::unique_ptr<ColladaMesh>> colladaMeshes;
	auto& python = PythonInterpreter::GetInstance();
	auto pyModule = python.LoadModule("load_collada"s);

	if (pyModule) {
		auto args = make_tuple("C:/Users/Al/documents/visual studio 2017/Projects/Jasper/models/C3P0/C3P0.dae");
		auto result = python.ExecuteFunction(pyModule, "load_model"s, args);
		if (result != nullptr) {
			if (PyObject_HasAttrString(result, "geometries"))
			{
				printf("Has Geometries\n");
				PyObject* pyGeoms = PyObject_GetAttrString(result, "geometries");
				PythonInterface::ParseColladaGeometries(pyGeoms, colladaMeshes);
				Py_DecRef(pyGeoms);
			}
			if (PyObject_HasAttrString(result, "controllers")) {
				auto controllers = PyObject_GetAttrString(result, "controllers");
				ParseColladaControllers(controllers, colladaMeshes);
			}
			Py_DecRef(result);
		}
	}
	else {
		PyErr_Print();
	}

	Py_DecRef(pyModule);
	return colladaMeshes;



}

void ModelLoader::LoadXmlModel(const std::string & filename, const std::string & name)
{
	//auto colladaMeshes = LoadColladaPython(filename);

	using namespace tinyxml2;
	auto modelData = m_scene->GetModelCache().CreateInstance<ModelData>(name);
	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename.c_str());
	auto docRoot = doc.RootElement();
	auto geomLib = GetChildNode(docRoot, "library_geometries"s);
	auto geomNodes = GetChildNodesByName(geomLib, "geometry"s);

	auto controllerLib = GetChildNode(docRoot, "library_controllers"s);
	auto materialsLib = GetChildNode(docRoot, "library_materials"s);
	auto effectsLib = GetChildNode(docRoot, "library_effects"s);
	auto imagesLib = GetChildNode(docRoot, "library_images"s);
	auto materials = GetChildNodesByName(materialsLib, "material");
	for (auto mat : materials) {
		auto mater = BuildXmlMaterial(mat, effectsLib, imagesLib);
	}
	vector<unique_ptr<ColladaMesh>> colladaMeshes;
	int geomCount = 0;
	for (auto geomNode : geomNodes) {
		auto meshNode = GetChildNode(geomNode, "mesh"s);
		colladaMeshes.emplace_back(BuildXmlMesh(meshNode));
		//modelData->AddMesh(mesh);
		//modelData->AddMaterial(mesh->GetMaterial());
	}
	//CenterOnOrigin(modelData->GetMeshes());
	auto vsLib = GetChildNode(docRoot, "library_visual_scenes");
	auto scenes = GetChildNodesByName(vsLib, "visual_scene");
	for (auto scene : scenes) {
		BuildXmlSkeleton(scene);
	}
	ColladaSkin skin = BuildXMLSkin(controllerLib);

	
	int x = 0;
}

void ModelLoader::ProcessAiSceneNode(const aiScene* scene, aiNode* node, const string& directory, ModelData* model_data)
{
	for (uint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessAiMesh(mesh, scene, directory, model_data);
	}

	for (uint i = 0; i < node->mNumChildren; i++) {
		ProcessAiSceneNode(scene, node->mChildren[i], directory, model_data);
	}
}


void ModelLoader::ProcessAiMesh(const aiMesh* aiMesh, const aiScene* scene, const string& directory, ModelData* model_data)
{
	Mesh* m;
	static int num = 0;
	string meshName = "";
	if (aiMesh->mName != aiString("")) {
		string ainame = string(aiMesh->mName.data);
		if (ainame != "defaultobject") {
			meshName = ainame;
		}
		else {
			meshName = m_name + "_mesh_" + to_string(m_processedMeshCount);
		}
	}
	else {
		meshName = m_name + "_mesh_" + to_string(m_processedMeshCount);
	}
	printf("Loading mesh: %s\n", meshName.c_str());
	num++;
	Mesh* cachedMesh = m_scene->GetMeshCache().GetResourceByName(meshName);
	if (cachedMesh) {
		model_data->GetMeshes().push_back(cachedMesh);
		m = cachedMesh;
	}
	else {

		m = m_scene->GetMeshCache().CreateInstance<Mesh>(meshName);
		m->Positions.reserve(aiMesh->mNumVertices);
		m->Normals.reserve(aiMesh->mNumVertices);
		m->Tangents.reserve(aiMesh->mNumVertices);
		m->Bitangents.reserve(aiMesh->mNumVertices);
		m->TexCoords.reserve(aiMesh->mNumVertices);

		m->Indices.reserve(aiMesh->mNumFaces * 3);

		for (uint i = 0; i < aiMesh->mNumVertices; i++) {
			Vertex v;
			auto avert = aiMesh->mVertices[i];
			v.Position = { avert.x, avert.y, avert.z };
			if (aiMesh->HasNormals()) {
				auto norm = aiMesh->mNormals[i];
				v.Normal = { norm.x, norm.y, norm.z };
			}
			if (aiMesh->HasTextureCoords(0)) {
				auto tc = aiMesh->mTextureCoords[0][i];
				v.TexCoords = { tc.x, tc.y };
			}
			if (aiMesh->HasTangentsAndBitangents()) {
				auto tang = aiMesh->mTangents[i];
				v.Tangent = { tang.x, tang.y, tang.z, 1.f };
				auto bitang = aiMesh->mBitangents[i];
				v.Bitangent = { bitang.x, bitang.y, bitang.z };
				//v.Tangent.w = (Dot(Cross(v.Normal, v.Tangent.AsVector3()), v.Bitangent) > 0.0f) ? -1.0f : 1.0f;

			}
			if (aiMesh->HasVertexColors(0)) {
				//int x = 0;
				//printf("Has colors\n");
			}
			m->AddVertex(v);
		}

		if (aiMesh->HasFaces()) {
			for (uint i = 0; i < aiMesh->mNumFaces; ++i) {
				auto face = aiMesh->mFaces[i];
				for (unsigned j = 0; j < face.mNumIndices; ++j) {
					m->Indices.push_back(face.mIndices[j]);
				}
			}
		}

		if (aiMesh->HasBones()) {
			Skeleton* sk = nullptr;
			if (m_scene->GetSkeletonCache().GetResourceByName(this->m_name + "_skeleton") != nullptr) {
				sk = m_scene->GetSkeletonCache().GetResourceByName(this->m_name + "_skeleton");
			}
			else {
				sk = m_scene->GetSkeletonCache().CreateInstance<Skeleton>(this->m_name + "_skeleton");
			}
			model_data->m_skeleton = sk;			
			m->SetSkeleton(sk);
			for (uint i = 0; i < aiMesh->mNumBones; ++i) {
				aiBone* bone = aiMesh->mBones[i];
				string bname = bone->mName.data;

				auto bd = make_unique<BoneData>();
				bd->Name = bname;
				bd->mesh = m;
				bd->skeleton = sk;
				aiMatrix4x4 mm = bone->mOffsetMatrix;
				bd->BoneOffsetTransform = aiMatrix4x4ToTransform(mm);
				ImporterSceneNode* isn = model_data->FindImporterSceneNode(bname);
				bd->NodeTransform = isn->NodeTransform;
				isn->isUsedBone = true;
				bd->INode = isn;
				//bd->NodeTransform = isn->NodeTransform;				
				//auto pnode = isn->Parent;
				//while (pnode != nullptr) {
				//	bd->NodeTransform = pnode->NodeTransform * bd->NodeTransform;	
				//	bd->BoneTransform = pnode->NodeTransform;
				//	pnode = pnode->Parent;
				//}

				for (uint j = 0; j < bone->mNumWeights; ++j) {
					auto bw = bone->mWeights[j];
					bd->Weights.emplace_back(VertexBoneWeight{ bw.mVertexId, bw.mWeight, m });
				}
				if (sk->m_boneMap.find(bname) == sk->m_boneMap.end()) {
					bd->Id = (int)sk->Bones.size();
					sk->m_boneMap[bname] = bd->Id;
					m->Bones.push_back(bd->Id);
					sk->Bones.push_back(move(bd));
				}
				else {
					int idx = sk->m_boneMap[bname];
					//BoneData& bdata = skeleton->Bones[idx];
					m->Bones.push_back(idx);
					printf("Bone already in boneMap: %s\n", bd->Name.data());
				}
			}
		}

		m->CalculateExtents();
		if (!aiMesh->HasNormals()) {
			m->CalculateFaceNormals();
		}
		model_data->GetMeshes().push_back(m);

	}

	Material* myMaterial = nullptr;
	if (aiMesh->mMaterialIndex >= 0) {
		aiMaterial* mat = scene->mMaterials[aiMesh->mMaterialIndex];
		aiString matName;
		mat->Get(AI_MATKEY_NAME, matName);
		printf("Found Material...%s\n", matName.C_Str());
		Material* cachedMaterial = m_scene->GetMaterialCache().GetResourceByName(string(matName.data));
		if (cachedMaterial) {
			model_data->GetMaterials().push_back(cachedMaterial);
			myMaterial = cachedMaterial;
			m->SetMaterial(myMaterial);
		}
		else {
			myMaterial = m_scene->GetMaterialCache().CreateInstance<Material>(matName.C_Str());
			model_data->GetMaterials().push_back(myMaterial);
			aiString texString;
			mat->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &texString);
			string textureFileName = string(texString.C_Str());
			if (textureFileName.find(".") == string::npos) {
				textureFileName += "_D.tga";
			}
			aiColor3D diffuse, ambient, specular;
			float shine;
			mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			mat->Get(AI_MATKEY_SHININESS, shine);
			if (shine <= 0.0f) {
				shine = 32.0f;
			}
			myMaterial->Ambient = Vector3(ambient.r, ambient.g, ambient.b);
			myMaterial->Diffuse = Vector3(diffuse.r, diffuse.g, diffuse.b);
			myMaterial->Specular = Vector3(specular.r, specular.g, specular.b);
			myMaterial->Shine = shine / 4.0f;
			if (texString.length > 0) {
				string texturePath = directory + "/" + textureFileName;
				myMaterial->SetTextureDiffuse(texturePath);
			}
			// try to load a normal map
			texString.Clear();
			mat->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &texString);
			if (texString.length > 0) {
				myMaterial->SetTextureNormalMap(directory + "/" + texString.C_Str());
			}
			else {
				texString.Clear();
				mat->GetTexture(aiTextureType::aiTextureType_HEIGHT, 0, &texString);
				if (texString.length > 0) {
					myMaterial->SetTextureNormalMap(directory + "/" + texString.C_Str());
				}
			}
			texString.Clear();
			mat->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &texString);
			if (texString.length > 0) {

				myMaterial->SetTextureSpecularMap(directory + "/" + texString.C_Str());
			}
			if (myMaterial->GetTextureDiffuseMap() == nullptr) {
				myMaterial->Flags &= Material::MATERIAL_FLAGS::USE_MATERIAL_COLOR;
			}
			m->SetMaterial(myMaterial);
		}

	}
	else {
		int xx = 0;
	}
	m_processedMeshCount++;
}

//class MyCallback : public VHACD::IVHACD::IUserCallback
//{
//public:
//    MyCallback(void) {}
//    ~MyCallback() {};
//    void Update(const double  overallProgress,
//                const double          stageProgress,
//                const double          operationProgress,
//                const char * const    stage,
//                const char * const    operation) {
//        cout << (int)(overallProgress + 0.5) << "% "
//             << "[ " << stage << " " << (int)(stageProgress + 0.5) << "% ] "
//             << operation << " " << (int)(operationProgress + 0.5) << "%" << endl;
//    };
//};

void ModelLoader::ConvexDecompose(Mesh* mesh, std::vector<std::unique_ptr<btConvexHullShape>>& shapes, Scene* scene)
{
	//    using namespace VHACD;
	//    MyCallback mcallback;
	//    IVHACD::Parameters params;
	//    params.m_resolution = 100000;
	//    params.m_depth = 20;
	//    params.m_concavity = 0.05;
	//    //params.m_planedownsampling = 8;
	//    //params.m_convexhulldownsampling = 8;
	//    params.m_alpha = 0.05;
	//    params.m_beta = 0.05;
	//    params.m_gamma = 0.015;//15;
	//    params.m_pca = 0;
	//    params.m_mode = 0; // 0: voxel-based (recommended), 1: tetrahedron-based
	//    //params.m_maxnumverticesperch = 64;
	//    //params.m_minvolumeperch = 0.05;
	//    params.m_callback = &mcallback;
	////	params.m_logger = 0;
	//
	//
	//    IVHACD* cdInterface = CreateVHACD();
	//    int tsz = mesh->Indices.size();
	//    int* tris = new int[tsz];
	//    for (int i = 0; i < tsz; ++i) {
	//        tris[i] = (int)mesh->Indices[i];
	//    }
	//
	//    float* positions = reinterpret_cast<float*>(&(mesh->Positions[0]));
	//    int numIndices = mesh->Indices.size() / 3;
	//    int numPositions = mesh->Positions.size();
	//    bool res = cdInterface->Compute(positions, 3, numPositions, tris, 3, numIndices, params);
	//    if (res) {
	//        auto debugShader = scene->GetShaderCache().GetResourceByName("basic_shader"s);
	//        auto mat = scene->GetMaterialCache().CreateInstance<Material>("ch_material");
	//        mat->IsTransparent = true;
	//        uint nConvexHulls = cdInterface->GetNConvexHulls();
	//
	//        IVHACD::ConvexHull ch;
	//        for (unsigned int p = 0; p < nConvexHulls; ++p) {
	//
	//            cdInterface->GetConvexHull(p, ch); // get the p-th convex-hull information
	//            std::unique_ptr<btConvexHullShape> bhull = make_unique<btConvexHullShape>(nullptr, 0);
	//
	//            //interfaceVHACD->GetConvexHull(p, ch); // get the p-th convex-hull information
	//            Mesh* testMesh = scene->GetMeshCache().CreateInstance<Mesh>("ch_" + std::to_string(p));
	//            //srand(time(nullptr));
	//            float dr = ((float)rand() / (RAND_MAX));
	//            float dg = ((float)rand() / (RAND_MAX));
	//            float db = ((float)rand() / (RAND_MAX));
	//            testMesh->Color = { dr, dg, db, 0.8f };
	//            for (unsigned int v = 0, idx = 0; v < ch.m_nPoints; ++v, idx += 3) {
	//                Vector3 vp = Vector3(ch.m_points[idx], ch.m_points[idx + 1], ch.m_points[idx + 2]);
	//                testMesh->Positions.push_back(vp);
	//                btVector3 vv = btVector3(ch.m_points[idx], ch.m_points[idx + 1], ch.m_points[idx + 2]);
	//                bhull->addPoint(vv);
	//            }
	//
	//            for (unsigned int t = 0, idx = 0; t < ch.m_nTriangles; ++t, idx += 3) {
	//                testMesh->Indices.push_back(ch.m_triangles[idx]);
	//                testMesh->Indices.push_back(ch.m_triangles[idx + 1]);
	//                testMesh->Indices.push_back(ch.m_triangles[idx + 2]);
	//
	//            }
	//
	//
	//            //auto go = GetGameObject();
	//            //go->AttachNewComponent<MeshRenderer>("ch_renderer_" + to_string(p), testMesh, mat);
	//
	//
	//
	//
	//
	//
	//
	//            shapes.emplace_back(move(bhull));
	//
	//        }
	//    }
	//
	//    delete[] tris;
	//    //delete[] pos;
	//    cdInterface->Clean();
	//    cdInterface->Release();

}

void ModelLoader::CalculateHalfExtents()
{
	float minx, miny, minz;
	minx = miny = minz = 1000000.f;
	float maxx, maxy, maxz;
	maxx = maxy = maxz = -100000.f;
	for (auto& mesh : m_model_meshes) {
		auto he = mesh->HalfExtents();
		if (he.x > maxx) maxx = he.x;
		if (he.y > maxy) maxy = he.y;
		if (he.z > maxz) maxz = he.z;

		if (he.x < minx) minx = he.x;
		if (he.y < miny) miny = he.y;
		if (he.z < minz) minz = he.z;
	}
	this->HalfExtents = Vector3();
}

void ModelLoader::Destroy()
{
	//m_materialManager.Clear();
	//m_meshManager.Clear();
	//Component::Destroy();
}

//void ModelLoader::Awake()
//{
//    //Initialize();
//    //Component::Awake();
//}
//
//void ModelLoader::Update(float dt)
//{
//    //Component::Update(dt);
//}

void ModelLoader::SaveToAssetFile(const std::string& filename)
{

	//    auto& meshes = m_meshManager.GetCache();
	//    int numMeshes = meshes.size();
	//    int numMaterials = m_materialManager.GetCache().size();
	//
	//    ofstream ofs;
	//    ofs.open(filename, std::ios::binary | std::ios::out);
	//
	//    ofs.write(reinterpret_cast<char*>(&numMeshes), sizeof(numMeshes));
	//
	//    for (const auto& mesh : meshes) {
	//        AssetSerializer::SerializeMesh(ofs, mesh.get());
	//    }
	//
	//    ofs.close();
	//
	//
	//}
	//
}

void ModelLoader::OutputMeshData(const string& filename)
{
	ofstream ofs;
	ofs.open(filename.c_str(), ios::out);

	int numMeshes = m_model_meshes.size();

	for (auto& mesh : m_model_meshes) {
		ofs << "// Outputting Mesh data for: " << m_name << "\n";
		ofs << "// Position Data:" << "\n";
		ofs << "Positions.reserve(" << std::to_string(mesh->Positions.size()) << ");\n";
		for (const auto& position : mesh->Positions) {
			ofs << "Positions.push_back(Vector3" << position.ToString() << ");\n";
		}
		ofs << "// TexCoord Data:" << "\n";
		ofs << "TexCoords.reserve(" << std::to_string(mesh->TexCoords.size()) << ");\n";
		for (const auto& tex : mesh->TexCoords) {
			ofs << "TexCoords.push_back(Vector2" << tex.ToString() << ");\n";
		}

		ofs << "// Normal Data:" << "\n";
		ofs << "Normals.reserve(" << std::to_string(mesh->Normals.size()) << ");\n";
		for (const auto& n : mesh->Normals) {
			ofs << "Normals.push_back(Vector3" << n.ToString() << ");\n";
		}

		ofs << "// Tangent Data:" << "\n";
		ofs << "Tangents.reserve(" << std::to_string(mesh->Tangents.size()) << ");\n";
		for (const auto& n : mesh->Tangents) {
			ofs << "Tangents.push_back(Vector4" << n.ToString() << ");\n";
		}

		ofs << "// BiTangent Data:" << "\n";
		ofs << "Bitangents.reserve(" << std::to_string(mesh->Bitangents.size()) << ");\n";
		for (const auto& n : mesh->Bitangents) {
			ofs << "Bitangents.push_back(Vector3" << n.ToString() << ");\n";
		}

		ofs << "// Index Data:" << "\n";
		ofs << "Indices.reserve(" << std::to_string(mesh->Indices.size()) << ");\n";
		for (const auto& idx : mesh->Indices) {
			ofs << "Indices.push_back(" << std::to_string(idx) << ");\n";
		}
	}

	ofs.close();

}



}
