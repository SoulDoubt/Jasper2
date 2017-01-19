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
#include "VHACD.h"

namespace Jasper
{

using namespace std;

ModelLoader::ModelLoader(Scene* scene, Shader* shader)
    : m_scene(scene), m_shader(shader)
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
//        for (auto m : meshes) {
//            ConvexDecompose(m, hulls, jScene);
//        }
//        printf("Created %d convex hulls in model.\n", (int)hulls.size());
//        CompoundCollider* cmp = GetGameObject()->AttachNewComponent<CompoundCollider>(this->GetName() + "_collider"s, hulls, m_physicsWorld);
//
//
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
            go->AttachNewComponent<MeshRenderer>(mesh->GetName() + "_renderer", mesh, mat);
        }
        if (generateCollider) {
            unique_ptr<PhysicsCollider> collider = GenerateSinglePhysicsCollider(modeldata, m_scene, PHYSICS_COLLIDER_TYPE::Box);
            go->AttachComponent(move(collider));
        }
    }
    return move(go);
}

void ModelLoader::LoadModel(const std::string& filename, const std::string& name)
{
    m_name = name;
    Assimp::Importer importer;
    printf("Loading model: %s\n", filename.c_str());
    const aiScene* scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        printf("aiScene was corrupt in model load.\n");
        return;
    }
    string directory = filename.substr(0, filename.find_last_of("/"));


    ProcessAiSceneNode(scene, scene->mRootNode, directory);

    //auto meshes = this->GetComponentsByType<Mesh>();
    auto& meshes = m_model_meshes;
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
//


    int i = 0;
    auto md = m_scene->GetModelCache().CreateInstance<ModelData>(m_name);
    for (auto& mesh : m_model_meshes) {
        md->AddMesh(mesh);
        md->AddMaterial(mesh->GetMaterial());
        //modelData.push_back(mesh);
        //modelData.push_back(mesh->GetMaterial());
        //mesh->Initialize();
        //Vector3 meshOrigin = { (mesh->GetMinExtents().x + mesh->GetMaxExtents().x) / 2.f, (mesh->GetMinExtents().y + mesh->GetMaxExtents().y) / 2.f , (mesh->GetMinExtents().z + mesh->GetMaxExtents().z) / 2.f };
        //auto child = make_unique<GameObject>("child_" + std::to_string(i));
        //child->GetLocalTransform().Position = meshOrigin;
        //GetGameObject()->AttachNewComponent<MeshRenderer>(mesh->GetName() + "_renderer" + to_string(i), mesh, mesh->m_material);
        //this->AttachChild(move(child));
        i++;
    }
    //SaveToAssetFile("modelSave.bin");
    //OutputMeshData();
    //printf("\nModel Contains %d Vertices and %d Triangles and %d Materials.", VertCount, TriCount, m_materialManager.GetCache().size());

}
//
//void ModelLoader::Initialize()
//{
//
//
//}

void ModelLoader::ProcessAiSceneNode(const aiScene* scene, aiNode* node, const string& directory)
{
    for (uint i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessAiMesh(mesh, scene, directory);
    }

    for (uint i = 0; i < node->mNumChildren; i++) {
        ProcessAiSceneNode(scene, node->mChildren[i], directory);
    }



}

aiNode* FindAiNode(aiNode* root, string name)
{
    if (string(root->mName.data) == name) {
        return root;
    }
    for (uint i = 0; i < root->mNumChildren; ++i) {
        auto t = FindAiNode(root->mChildren[i], name);
        if (t) {
            return t;
        }
    }
    return nullptr;

}

void ModelLoader::ProcessAiMesh(const aiMesh* aiMesh, const aiScene* scene, const string& directory)
{
    Mesh* m;
    static int num = 0;
    string meshName = "";
    if (aiMesh->mName != aiString("")) {
        string ainame = string(aiMesh->mName.data);
//? meshName = aiMesh->mName.C_Str() : meshName = "unnamed_model_mesh_" + std::to_string(num);
        if (ainame != "defaultobject") {
            meshName = ainame;
        } else {
            meshName = m_name + "_mesh_" + to_string(m_processedMeshCount);
        }
    } else {
        meshName = m_name + "_mesh_" + to_string(m_processedMeshCount);
    }
    num++;
    Mesh* cachedMesh = m_scene->GetMeshCache().GetResourceByName(meshName);
    if (cachedMesh) {
        m_model_meshes.push_back(cachedMesh);
        m = cachedMesh;
    } else {

        m = m_scene->GetMeshCache().CreateInstance<Mesh>(meshName);
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
            for (uint i = 0; i < aiMesh->mNumBones; ++i) {
                auto bone = aiMesh->mBones[i];
                string bname = bone->mName.data;
                Mesh::BoneData bd;
                bd.Name = bname;
                auto mm = bone->mOffsetMatrix;
                auto rootMatrix = scene->mRootNode->mTransformation;
                rootMatrix.Inverse();
                mm = mm * rootMatrix;
                aiQuaternion brot;
                aiVector3D bpos;
                aiVector3D bscale;
                mm.Decompose(bscale, brot, bpos);
                bd.BoneTransform.Position = Vector3(bpos.x, bpos.y, bpos.z);
                bd.BoneTransform.Orientation = Quaternion(brot.x, brot.y, brot.z, brot.w);
                bd.BoneTransform.Scale = Vector3(bscale.x, bscale.y, bscale.z);
                for (uint j = 0; j < bone->mNumWeights; ++j) {
                    auto bw = bone->mWeights[j];
                    bd.Weights.emplace_back(Mesh::VertexBoneWeight { bw.mVertexId, bw.mWeight });
                }
                m->Bones.emplace_back(bd);
            }
            aiNode* root = scene->mRootNode;
            for (uint i = 0; i < m->Bones.size(); ++i) {
                auto& b = m->Bones[i];
                aiNode* boneNode = FindAiNode(root, b.Name);
                aiNode* pNode = boneNode->mParent;
                auto parentBone = find_if(begin(m->Bones), end(m->Bones), [&](const auto& mb) {
                    return mb.Name == string(pNode->mName.data);
                });
                if (parentBone != end(m->Bones)) {
                    b.Parent = &(*parentBone);
                }
            }
        }
        //m->CalculateFaceNormals();
        m->CalculateExtents();
        if (!aiMesh->HasNormals()) {
            m->CalculateFaceNormals();
        }
        m_model_meshes.push_back(m);

    }

    Material* myMaterial = nullptr;
    if (aiMesh->mMaterialIndex >= 0) {
        aiMaterial* mat = scene->mMaterials[aiMesh->mMaterialIndex];
        aiString matName;
        mat->Get(AI_MATKEY_NAME, matName);
        printf("Found Material...%s\n", matName.C_Str());
        Material* cachedMaterial = m_scene->GetMaterialCache().GetResourceByName(string(matName.data));
        if (cachedMaterial) {
            m_model_materials.push_back(cachedMaterial);
            myMaterial = cachedMaterial;
        } else {
            myMaterial = m_scene->GetMaterialCache().CreateInstance<Material>(m_shader, matName.C_Str());
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

    }
    //GetGameObject()->AttachNewComponent<MeshRenderer>(m, m->m_material);


//    Material* renderMaterial;
//    if (myMaterial) {
//        renderMaterial = myMaterial;
//    } else {
//        renderMaterial = jScene->GetMaterialCache().CreateInstance<Material>(m_shader, "jasper_default_material");
//        renderMaterial->SetTextureDiffuse("./textures/default.png");
//    }
//    m->m_material = renderMaterial;
////auto mr =
//
//    printf("Loaded Model Mesh\n");
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
    using namespace VHACD;
    MyCallback mcallback;
    IVHACD::Parameters params;
    params.m_resolution = 100000;
    params.m_depth = 20;
    params.m_concavity = 0.05;
    //params.m_planedownsampling = 8;
    //params.m_convexhulldownsampling = 8;
    params.m_alpha = 0.05;
    params.m_beta = 0.05;
    params.m_gamma = 0.015;//15;
    params.m_pca = 0;
    params.m_mode = 0; // 0: voxel-based (recommended), 1: tetrahedron-based
    //params.m_maxnumverticesperch = 64;
    //params.m_minvolumeperch = 0.05;
    params.m_callback = &mcallback;
//	params.m_logger = 0;


    IVHACD* cdInterface = CreateVHACD();
    int tsz = mesh->Indices.size();
    int* tris = new int[tsz];
    for (int i = 0; i < tsz; ++i) {
        tris[i] = (int)mesh->Indices[i];
    }

    float* positions = reinterpret_cast<float*>(&(mesh->Positions[0]));
    int numIndices = mesh->Indices.size() / 3;
    int numPositions = mesh->Positions.size();
    bool res = cdInterface->Compute(positions, 3, numPositions, tris, 3, numIndices, params);
    if (res) {
        auto debugShader = scene->GetShaderCache().GetResourceByName("basic_shader"s);
        auto mat = scene->GetMaterialCache().CreateInstance<Material>(debugShader, "ch_material");
        mat->IsTransparent = true;
        uint nConvexHulls = cdInterface->GetNConvexHulls();

        IVHACD::ConvexHull ch;
        for (unsigned int p = 0; p < nConvexHulls; ++p) {

            cdInterface->GetConvexHull(p, ch); // get the p-th convex-hull information
            std::unique_ptr<btConvexHullShape> bhull = make_unique<btConvexHullShape>(nullptr, 0);

            //interfaceVHACD->GetConvexHull(p, ch); // get the p-th convex-hull information
            Mesh* testMesh = scene->GetMeshCache().CreateInstance<Mesh>("ch_" + std::to_string(p));
            //srand(time(nullptr));
            float dr = ((float)rand() / (RAND_MAX));
            float dg = ((float)rand() / (RAND_MAX));
            float db = ((float)rand() / (RAND_MAX));
            testMesh->Color = { dr, dg, db, 0.8f };
            for (unsigned int v = 0, idx = 0; v < ch.m_nPoints; ++v, idx += 3) {
                Vector3 vp = Vector3(ch.m_points[idx], ch.m_points[idx + 1], ch.m_points[idx + 2]);
                testMesh->Positions.push_back(vp);
                btVector3 vv = btVector3(ch.m_points[idx], ch.m_points[idx + 1], ch.m_points[idx + 2]);
                bhull->addPoint(vv);
            }

            for (unsigned int t = 0, idx = 0; t < ch.m_nTriangles; ++t, idx += 3) {
                testMesh->Indices.push_back(ch.m_triangles[idx]);
                testMesh->Indices.push_back(ch.m_triangles[idx + 1]);
                testMesh->Indices.push_back(ch.m_triangles[idx + 2]);

            }


            //auto go = GetGameObject();
            //go->AttachNewComponent<MeshRenderer>("ch_renderer_" + to_string(p), testMesh, mat);







            shapes.emplace_back(move(bhull));

        }
    }

    delete[] tris;
    //delete[] pos;
    cdInterface->Clean();
    cdInterface->Release();

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
