#include "Model.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

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
#include "VHACD.h"

namespace Jasper
{

using namespace std;


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

aiNode* FindAiNode(aiNode* root, aiNode* treeRoot, string name, BoneData* bd)
{
    if (string(root->mName.data) == name) {
        if (root == treeRoot) {
            bd->ParentName = "treeroot"s;
            bd->Depth = 0;
            bd->ainode = root;

        } else {
            int dep = 0;
            aiNode* pn = root->mParent;
            bd->ParentName = string(pn->mName.data);
            while (pn != nullptr) {
                dep++;
                pn = pn->mParent;
            }
            bd->Depth = dep;
        }
        bd->ainode = root;
        return root;
    }
    for (uint i = 0; i < root->mNumChildren; ++i) {
        auto t = FindAiNode(root->mChildren[i], treeRoot, name, bd);
        if (t) {
            if (t == treeRoot) {
                bd->Depth = 0;
            } else {
                int dep = 0;
                aiNode* pn = t->mParent;
                while (pn != nullptr) {
                    dep++;
                    pn = pn->mParent;
                }
                bd->Depth = dep;
            }
            bd->ainode = t;
            return t;
        }
    }
    return nullptr;

}

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
        } else if (generateCollider && modeldata->GetSkeleton()->Bones.size() >0) {
            modeldata->CreateRagdollCollider(m_scene, go.get());
        }
    }
    return move(go);
}

//
//void TraverseSkeleton(const aiNode* node, Skeleton* skeleton)
//{
//    int bdidx = skeleton->m_boneMap[node->mName.data];
//    BoneData& parent = skeleton->Bones[bdidx];
//    int childCount = node->mNumChildren;
//    printf("Traversing children of: %s \n", parent.Name.data());
//    for (size_t i = 0; i < childCount; ++i) {
//        aiNode* child = node->mChildren[i];
//        int childbdidx = skeleton->m_boneMap[child->mName.data];
//        BoneData& childbd = skeleton->Bones[childbdidx];
//        childbd.BoneTransform = aiMatrix4x4ToTransform(child->mTransformation);
//        childbd.Parent = &parent;
//        Transform nodeTransform = parent.BoneTransform * aiMatrix4x4ToTransform(child->mTransformation);
//        
//        childbd.InverseBindTransform = parent.InverseBindTransform * nodeTransform;
//        //childbd.InverseBindMatrix = parent.InverseBindMatrix * aiMatrix4x4ToMatrix4(child->mTransformation);
//        parent.Children.push_back(&childbd);
//        if (node != child) {
//            TraverseSkeleton(child, skeleton);
//        }
//    }
//}

void CreateChildHulls(BoneData& rootBone, vector<unique_ptr<btCollisionShape>>& hulls)
{
    for (size_t j = 0; j < rootBone.Children.size(); j++) {
        unique_ptr<btCollisionShape> hull = make_unique<btConvexHullShape>(nullptr, 0);
        auto h = static_cast<btConvexHullShape*>(hull.get());
        BoneData& bone = *(rootBone.Children[j]);
        std::sort(rootBone.Weights.begin(), rootBone.Weights.end());
        std::sort(bone.Weights.begin(), bone.Weights.end());
        vector<VertexBoneWeight> uniqueChildWeights;

        std::set_difference(bone.Weights.begin(), bone.Weights.end(), rootBone.Weights.begin(), rootBone.Weights.end(), std::inserter(uniqueChildWeights, uniqueChildWeights.begin()));
        for (size_t i = 0; i < uniqueChildWeights.size(); ++i) {
            VertexBoneWeight& vb = uniqueChildWeights[i];
            if (vb.Weight >= 0.5f) {
                Vector3 position = vb.mesh->Positions[vb.Index];
                h->addPoint(position.AsBtVector3());
            }
        }
        hulls.emplace_back(move(hull));
        CreateChildHulls(bone, hulls);
    }
}

void ModelData::CreateRagdollCollider(Scene* scene, GameObject* go)
{

    //CompoundCollider(std::string name, std::vector<std::unique_ptr<btConvexHullShape>>& hulls, PhysicsWorld * world)
    vector<unique_ptr<btCollisionShape>> hulls;
    BoneData& rootBone = m_skeleton->Bones[m_skeleton->m_boneMap[this->GetSkeleton()->RootBoneName]];
    size_t vertCount = rootBone.Weights.size();
    vector<Vector3> boneVerts(vertCount);
    //Mesh* boneMesh = scene->GetMeshCache().CreateInstance<Mesh>(rootBone.Name + "_mesh");
    unique_ptr<btCollisionShape> hullShape = make_unique<btConvexHullShape>(nullptr, 0);
    auto hs = static_cast<btConvexHullShape*>(hullShape.get());
    for (size_t i = 0; i < vertCount; ++i) {
        VertexBoneWeight& vb = rootBone.Weights[i];
        if (vb.Weight > 0.75f) {
            Vector3 position = vb.mesh->Positions[vb.Index];
            hs->addPoint(position.AsBtVector3());
        }
    }
    hulls.emplace_back(move(hullShape));
    CreateChildHulls(rootBone, hulls);

    unique_ptr<CompoundCollider> collider = make_unique<CompoundCollider>(rootBone.Name + "_collider"s, hulls, scene->GetPhysicsWorld());
    collider->Mass = 50.f;
    go->AttachComponent(std::move(collider));
    printf("Created Ragdoll.\n");


}

bool FileExists(const std::string& name)
{
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
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


    ProcessAiSceneNode(scene, scene->mRootNode, directory, model_data);

    //auto meshes = this->GetComponentsByType<Mesh>();
    auto& meshes = model_data->GetMeshes();
    size_t sz = meshes.size();
    printf("Loaded %d meshes in model: %s\n", sz, m_name.c_str());

    MaxExtents = { -100000.0f, -1000000.0f, -1000000.0f };
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
//    if (m_enablePhysics) {
    if (model_data->GetSkeleton()->Bones.size() > 0) {
        Skeleton* skeleton = model_data->GetSkeleton();
        printf("There are %d Bones in the skeleton:\n", skeleton->Bones.size());


        aiNode* root = scene->mRootNode;
        skeleton->GlobalInverseTransform = aiMatrix4x4ToTransform(root->mTransformation).Inverted();
        //skeleton->GlobalInverseMatrix = aiMatrix4x4ToMatrix4(root->mTransformation).Inverted();
        //printf("aiScene Root node is named: %s\n", root->mName.data);
        for (uint i = 0; i < skeleton->Bones.size(); ++i) {
            auto& b = skeleton->Bones[i];
            FindAiNode(root, root, b.Name, &b);
        }
        int minDepth = 1000000;
        const BoneData* minBone = nullptr;
        for (size_t j = 0; j < skeleton->Bones.size(); ++j) {
            const auto& bone = skeleton->Bones[j];
            if (bone.Depth < minDepth) {
                minDepth = bone.Depth;
                minBone = &bone;
            }
        }

        // set the minimum depth bone as the root bone by making it's parent -1
        auto rootBoneIter = skeleton->m_boneMap.find(minBone->Name);
        if (rootBoneIter == skeleton->m_boneMap.end()) {
            printf("ROOT BONE NOT FOUND IN MAP!!! %s\n", minBone->Name.data());
        } else {
            int rootBoneID = rootBoneIter->second;
            auto& rootBone = skeleton->Bones[rootBoneID];


            printf("The Root Bone is: %s \n", minBone->Name.data());
            skeleton->RootBoneName = rootBone.Name;

            aiNode* rootBoneNode = FindAiNode(scene->mRootNode, minBone->Name);
            
//            Transform testtransform = aiMatrix4x4ToTransform(rootBoneNode->mTransformation);
//            Matrix4 testtransformmat = testtransform.TransformMatrix();
//            printf("Root Bone Position is: %s\n", testtransform.Position.ToString().data());
//            Matrix4 testmatrixmat = aiMatrix4x4ToMatrix4(rootBoneNode->mTransformation);
//            
//            printf("Transform Matrix of root bone: \n%s\n", testtransformmat.ToString().data());
//            printf("Matrix of root bone: \n%s\n", testmatrixmat.ToString().data());
            
            
            rootBone.InverseBindTransform = aiMatrix4x4ToTransform(rootBoneNode->mTransformation).Inverted();
            rootBone.BoneTransform = aiMatrix4x4ToTransform(rootBoneNode->mTransformation);
            //Matrix4 testm = rootBone.InverseBindTransform.TransformMatrix();
            //printf("Inverse Bind Transform Matrix of root bone: \n%s\n", testm.ToString().data());
            //rootBone.InverseBindMatrix = aiMatrix4x4ToMatrix4(rootBoneNode->mTransformation).Inverted();
            //printf("Inverse Bind Matrix of root bone: \n%s\n", rootBone.InverseBindMatrix.ToString().data());
            if (rootBoneNode != scene->mRootNode) {
                aiNode* rootBoneParent = rootBoneNode->mParent;
                Transform tempTransform = aiMatrix4x4ToTransform(rootBoneNode->mTransformation);
                auto tempMatrix = rootBoneNode->mTransformation;
                while (rootBoneParent != NULL) {
                    printf("Root Bone Parent Node: %s\n", rootBoneParent->mName.data);
                    tempTransform = aiMatrix4x4ToTransform(rootBoneParent->mTransformation) * tempTransform ;
                    tempMatrix = rootBoneParent->mTransformation * tempMatrix;
                    //rootBone.BoneMatrix = aiMatrix4x4ToMatrix4(rootBoneParent->mTransformation).Inverted() * rootBone.BoneMatrix;
                    rootBoneParent = rootBoneParent->mParent;
                }
                //rootBone.BoneMatrix = rootBone.BoneMatrix.Inverted();
                //Matrix4 mmm;
                //mmm.SetToIdentity();
                //rootBone.InverseBindMatrix = skeleton->GlobalInverseMatrix * tempMatrix;
                rootBone.InverseBindTransform = skeleton->GlobalInverseTransform * tempTransform;
                rootBone.BoneTransform = tempTransform;
            }
            //rootBone.InverseBindTransform = rootTransform;

            skeleton->TraverseSkeleton(rootBoneNode);


            for (size_t i = 0; i < skeleton->Bones.size(); ++i) {
                BoneData& bd = skeleton->Bones[i];
                printf("Bone ID: %d, Name: %s Has: %d children.\n", bd.Index, bd.Name.data(), bd.Children.size());
            }

            if (1) {
                int boneIdToMove = skeleton->m_boneMap["lUpperLeg_0_"];
                BoneData& boneToMove = skeleton->Bones[boneIdToMove];
                //boneToMove.BoneOffsetMatrix.Translate(Vector3(0.25f, 0.f, 0.f));
                Quaternion q = Quaternion::FromAxisAndAngle(Vector3(0.f, 0.f, 1.f), DEG_TO_RAD(45));    
                Transform t = boneToMove.BoneTransform;
                t.Orientation *= q;
                boneToMove.BoneTransform = t;
                //skeleton->TransformBone(boneToMove);
                
                //boneToMove.BoneOffsetMatrix = bom;
                skeleton->EvaluateBoneSubtree(boneToMove);
            }
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
        } else {
            meshName = m_name + "_mesh_" + to_string(m_processedMeshCount);
        }
    } else {
        meshName = m_name + "_mesh_" + to_string(m_processedMeshCount);
    }
    printf("Loading mesh: %s\n", meshName.c_str());
    num++;
    Mesh* cachedMesh = m_scene->GetMeshCache().GetResourceByName(meshName);
    if (cachedMesh) {
        model_data->GetMeshes().push_back(cachedMesh);
        m = cachedMesh;
    } else {

        m = m_scene->GetMeshCache().CreateInstance<Mesh>(meshName);
        //m->SetVertexFormat(Mesh::VERTEX_FORMAT::Vertex_PNUTB);
        //auto m = this->AttachNewComponent<Mesh>();
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

                BoneData bd;
                bd.Name = bname;
                aiMatrix4x4 mm = bone->mOffsetMatrix;
                bd.BoneOffsetTransform = aiMatrix4x4ToTransform(mm);
                //Matrix4 bm = aiMatrix4x4ToMatrix4(mm);
                //bd.BoneOffsetMatrix = bm;

                for (uint j = 0; j < bone->mNumWeights; ++j) {
                    auto bw = bone->mWeights[j];
                    bd.Weights.emplace_back(VertexBoneWeight { bw.mVertexId, bw.mWeight, m });
                }
                if (skeleton->m_boneMap.find(bname) == skeleton->m_boneMap.end()) {
                    bd.Index = skeleton->Bones.size();
                    skeleton->Bones.push_back(bd);
                    skeleton->m_boneMap[bname] = bd.Index;
                    m->Bones.push_back(&bd);
                } else {
                    int idx = skeleton->m_boneMap[bname];
                    BoneData& bdata = skeleton->Bones[idx];
                    m->Bones.push_back(&bdata);
                    printf("Bone already in boneMap: %s\n", bd.Name.data());
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
        } else {
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
            } else {
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

    } else {
        int xx = 0;
    }
    m_processedMeshCount++;
}

class MyCallback : public VHACD::IVHACD::IUserCallback
{
public:
    MyCallback(void) {}
    ~MyCallback() {};
    void Update(const double  overallProgress,
                const double          stageProgress,
                const double          operationProgress,
                const char * const    stage,
                const char * const    operation) {
        cout << (int)(overallProgress + 0.5) << "% "
             << "[ " << stage << " " << (int)(stageProgress + 0.5) << "% ] "
             << operation << " " << (int)(operationProgress + 0.5) << "%" << endl;
    };
};

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
