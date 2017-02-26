#include "Model.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iterator>

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
		if (m->GetMaxExtents().x > MaxExtents.x) MaxExtents.x = m->GetMaxExtents().x;
		if (m->GetMaxExtents().y > MaxExtents.y) MaxExtents.y = m->GetMaxExtents().y;
		if (m->GetMaxExtents().z > MaxExtents.z) MaxExtents.z = m->GetMaxExtents().z;

		if (m->GetMinExtents().x < MinExtents.x) MinExtents.x = m->GetMinExtents().x;
		if (m->GetMinExtents().y < MinExtents.y) MinExtents.y = m->GetMinExtents().y;
		if (m->GetMinExtents().z < MinExtents.z) MinExtents.z = m->GetMinExtents().z;
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
			modeldata->CreateRagdollCollider(m_scene, go.get());
		}
		if (modeldata->Animator) {
			go->AttachComponent(move(modeldata->Animator));
		}
	}
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
			if (vb.Weight >= 0.5f) {
				Vector3 position = vb.mesh->Positions[vb.Index];
				h->addPoint(position.AsBtVector3());
			}
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
	CreateChildHulls(rootBone, m_skeleton.get(), hulls);

	unique_ptr<CompoundCollider> collider = make_unique<CompoundCollider>(rootBone->Name + "_collider"s, hulls, scene->GetPhysicsWorld());
	collider->Mass = 50.f;
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

void BuildBoneHierarchy(ImporterSceneNode* rootNode, Skeleton* skeleton) {

	int boneIndex = skeleton->m_boneMap.at(rootNode->Name);
	auto& bone = skeleton->Bones[boneIndex];
	if (bone->ParentID == -1) {
		auto parent = rootNode->Parent;
		while (parent != nullptr) {
			bone->BoneTransform = parent->NodeTransform * bone->BoneTransform;
			parent = parent->Parent;
		}
	}
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
			auto parent = childNode.Parent;
			while (parent != nullptr) {
				childBone->BoneTransform = parent->NodeTransform * childBone->BoneTransform;
				parent = parent->Parent;
			}
			childBone->SkinningTransform = childBone->BoneTransform * childBone->BoneOffsetTransform;
			BuildBoneHierarchy(&childNode, skeleton);
		}
	}
}

void ModelData::CreateImporterSceneGraph(aiNode* rootNode) {
	m_importerSceneRoot = make_unique<ImporterSceneNode>(rootNode);
	BuildSceneRecursive(m_importerSceneRoot.get(), nullptr);
}

ImporterSceneNode* ModelData::GetRootBoneNode(Skeleton* skeleton) {
	const auto bd = skeleton->Bones[0].get();
	auto boneNode = FindImporterSceneNode(bd->Name);
	assert(boneNode != nullptr);
	ImporterSceneNode* rootBoneNode = nullptr;
	while (boneNode->Parent != nullptr) {
		auto parentNode = boneNode->Parent;

		auto parentBoneIterator = skeleton->m_boneMap.find(parentNode->Name);
		if (parentBoneIterator != skeleton->m_boneMap.end()) {
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

void ModelLoader::CenterOnOrigin(std::vector<Jasper::Mesh *> & meshes)
{
	MaxExtents = { -1000000.0f, -1000000.0f, -1000000.0f };
	MinExtents = { 1000000.0f, 1000000.0f, 1000000.0f };

	for (auto m : meshes) {

		this->TriCount += m->Indices.size() / 3;
		this->VertCount += m->Positions.size();
		if (m->GetMaxExtents().x > MaxExtents.x) MaxExtents.x = m->GetMaxExtents().x;
		if (m->GetMaxExtents().y > MaxExtents.y) MaxExtents.y = m->GetMaxExtents().y;
		if (m->GetMaxExtents().z > MaxExtents.z) MaxExtents.z = m->GetMaxExtents().z;

		if (m->GetMinExtents().x < MinExtents.x) MinExtents.x = m->GetMinExtents().x;
		if (m->GetMinExtents().y < MinExtents.y) MinExtents.y = m->GetMinExtents().y;
		if (m->GetMinExtents().z < MinExtents.z) MinExtents.z = m->GetMinExtents().z;
	}

	Vector3 localOrigin = { (MinExtents.x + MaxExtents.x) / 2.f, (MinExtents.y + MaxExtents.y) / 2.f , (MinExtents.z + MaxExtents.z) / 2.f };
	float epsilon = 0.000001f;
	if (fabs(localOrigin.x) > epsilon || fabs(localOrigin.y) > epsilon || fabs(localOrigin.z) > epsilon) {
		for (auto& m : meshes) {
			for (auto& v : m->Positions) {
				v -= localOrigin;
			}
			m->CalculateExtents();
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

	CenterOnOrigin(meshes);

	// recurse the isn graph and find any bones that are are used in this model...


	if (model_data->GetSkeleton()->Bones.size() > 0) {


		Skeleton* skeleton = model_data->GetSkeleton();

		skeleton->GlobalInverseTransform = model_data->ImporterSceneRoot()->NodeTransform.Inverted();

		auto rootBoneNode = model_data->GetRootBoneNode(skeleton);
		assert(rootBoneNode != nullptr);
		skeleton->RootBoneName = rootBoneNode->Name;
		skeleton->Bones[skeleton->m_boneMap[skeleton->RootBoneName]]->ParentID = -1;
		BuildBoneHierarchy(rootBoneNode, skeleton);

		printf("There are %d Bones in the skeleton:\n", skeleton->Bones.size());


		for (size_t i = 0; i < skeleton->Bones.size(); ++i) {
			BoneData* bd = skeleton->Bones[i].get();
			printf("Bone ID: %d, Name: %s Has: %d children.\n", bd->Id, bd->Name.data(), bd->Children.size());
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

		if (0) {
			int boneIdToMove = skeleton->m_boneMap["Bip01_L_Thigh"];
			BoneData* boneToMove = skeleton->Bones[boneIdToMove].get();
			//boneToMove.BoneOffsetMatrix.Translate(Vector3(0.25f, 0.f, 0.f));
			Quaternion q = Quaternion::FromAxisAndAngle(Vector3(0.f, 1.f, 0.f), DEG_TO_RAD(25));
			Transform t = boneToMove->BoneTransform;
			t.Orientation *= q;// *t.Orientation;
			boneToMove->BoneTransform = t;
			boneToMove->EvaluateSubtree();
		}


	}

	for (auto& mesh : model_data->GetMeshes()) {

		mesh->SetSkeleton(model_data->GetSkeleton());
	}


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
				printf("Has colors\n");
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
			Skeleton* skeleton = model_data->GetSkeleton();
			for (uint i = 0; i < aiMesh->mNumBones; ++i) {
				aiBone* bone = aiMesh->mBones[i];
				string bname = bone->mName.data;

				auto bd = make_unique<BoneData>();
				bd->Name = bname;
				bd->mesh = m;
				bd->skeleton = skeleton;
				aiMatrix4x4 mm = bone->mOffsetMatrix;
				bd->BoneOffsetTransform = aiMatrix4x4ToTransform(mm);
				ImporterSceneNode* isn = model_data->FindImporterSceneNode(bname);
				isn->isUsedBone = true;
				bd->INode = isn;
				//bd->NodeTransform = isn->NodeTransform;
				bd->BoneTransform = isn->NodeTransform;
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
				if (skeleton->m_boneMap.find(bname) == skeleton->m_boneMap.end()) {
					bd->Id = (int)skeleton->Bones.size();
					skeleton->m_boneMap[bname] = bd->Id;
					m->Bones.push_back(bd->Id);
					skeleton->Bones.push_back(move(bd));					
				}
				else {
					int idx = skeleton->m_boneMap[bname];
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
		auto he = mesh->GetHalfExtents();
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
