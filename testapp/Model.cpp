#include "Model.h"

#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>

#include "Common.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "ConvexHullCollider.h"
#include "CylinderCollider.h"
#include "SphereCollider.h"
#include "Scene.h"
#include <string>
#include <iostream>
#include <fstream>
#include "AssetSerializer.h"
#include "VHACD.h"

namespace Jasper
{

using namespace std;

Model::Model(const std::string& name, const std::string& filename, Shader* shader, bool enablePhysics, PhysicsWorld* physicsWorld)
    :Component(name), m_filename(filename), m_shader(shader), m_enablePhysics(enablePhysics), m_physicsWorld(physicsWorld)
{

}


Model::~Model()
{

}

void Model::Setup(Scene* jScene)
{
	
    Assimp::Importer importer;
    printf("Loading model: %s\n", m_filename.c_str());
    const aiScene* scene = importer.ReadFile(m_filename, aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        printf("aiScene was corrupt in model load.\n");
        return;
    }

    m_directory = m_filename.substr(0, m_filename.find_last_of("/"));

    ProcessAiSceneNode(scene, scene->mRootNode, jScene);

    //auto meshes = this->GetComponentsByType<Mesh>();
    auto& meshes = m_model_meshes;
    size_t sz = meshes.size();
    printf("\nLoaded %d meshes in model: %s", sz, this->GetName().c_str());

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
    if (m_enablePhysics) {
        Vector3 hes = { (MaxExtents.x - MinExtents.x) / 2, (MaxExtents.y - MinExtents.y) / 2, (MaxExtents.z - MinExtents.z) / 2 };
        PhysicsCollider* collider = nullptr;
        switch (this->ColliderType) {
        case PHYSICS_COLLIDER_TYPE::Box:
            collider = GetGameObject()->AttachNewComponent<BoxCollider>(this->GetName() + "_Collider_", hes, m_physicsWorld);
            break;
        case PHYSICS_COLLIDER_TYPE::Capsule:
            collider = GetGameObject()->AttachNewComponent<CapsuleCollider>(this->GetName() + "_Collider_", hes, m_physicsWorld);
            break;
        case PHYSICS_COLLIDER_TYPE::Sphere:
            collider = GetGameObject()->AttachNewComponent<SphereCollider>(this->GetName() + "_Collider_", hes, m_physicsWorld);
            break;
        case PHYSICS_COLLIDER_TYPE::Cylinder:
            collider = GetGameObject()->AttachNewComponent<CylinderCollider>(this->GetName() + "_Collider_", hes, m_physicsWorld);
            break;
        case PHYSICS_COLLIDER_TYPE::ConvexHull:
		{
			ConvexHullCollider* cvx = GetGameObject()->AttachNewComponent<ConvexHullCollider>(this->GetName() + "_Collider_", hes, m_physicsWorld);
			cvx->InitFromMeshes(meshes);
			collider = cvx;
			break;
		}
		case PHYSICS_COLLIDER_TYPE::Compound:
		{
			vector<unique_ptr<btConvexHullShape>> hulls;
			for (auto m : meshes) {
				ConvexDecompose(m, hulls, jScene);				
			}
			printf("Created %d convex hulls in model.\n", hulls.size());
			CompoundCollider* cmp = GetGameObject()->AttachNewComponent<CompoundCollider>(this->GetName() + "_collider"s, hulls, m_physicsWorld);
			
			
			collider = cmp;
			break;
		}
        }
        if (collider) {
            collider->Mass = this->Mass;
            collider->Restitution = this->Restitution;
            collider->Friction = this->Friction;
        }
    }


    int i = 0;
    for (auto& mesh : meshes) {
        //Vector3 meshOrigin = { (mesh->GetMinExtents().x + mesh->GetMaxExtents().x) / 2.f, (mesh->GetMinExtents().y + mesh->GetMaxExtents().y) / 2.f , (mesh->GetMinExtents().z + mesh->GetMaxExtents().z) / 2.f };
        //auto child = make_unique<GameObject>("child_" + std::to_string(i));
        //child->GetLocalTransform().Position = meshOrigin;
        GetGameObject()->AttachNewComponent<MeshRenderer>(mesh->GetName() + "_renderer", mesh, mesh->m_material);
        //this->AttachChild(move(child));
        i++;
    }
    //SaveToAssetFile("modelSave.bin");
    //OutputMeshData();
    printf("\nModel Contains %d Vertices and %d Triangles and %d Materials.", VertCount, TriCount, m_materialManager.GetCache().size());

}

void Model::Initialize()
{


}

void Model::ProcessAiSceneNode(const aiScene* scene, aiNode* node, Scene* jScene)
{
    for (uint i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessAiMesh(mesh, scene, jScene);
    }

    for (uint i = 0; i < node->mNumChildren; i++) {
        ProcessAiSceneNode(scene, node->mChildren[i], jScene);
    }



}

void Model::ProcessAiMesh(const aiMesh* aiMesh, const aiScene* scene, Scene* jScene)
{
	static int num = 0;
    string meshName = "";
    aiMesh->mName != aiString("") ? meshName = aiMesh->mName.C_Str() : meshName = "unnamed_model_mesh_" + std::to_string(num);
	num++;
    auto m = jScene->GetMeshCache().CreateInstance<Mesh>(meshName);
    m_model_meshes.push_back(m);
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
        for (uint i = 0; i < aiMesh->mNumFaces; i++) {
            auto face = aiMesh->mFaces[i];
            for (unsigned j = 0; j < face.mNumIndices; j++) {
                m->Indices.push_back(face.mIndices[j]);
            }
        }
    }
    //m->CalculateFaceNormals();
    m->CalculateExtents();
    Material* myMaterial = nullptr;
    if (aiMesh->mMaterialIndex >= 0) {
        aiMaterial* mat = scene->mMaterials[aiMesh->mMaterialIndex];
        aiString matName;
        mat->Get(AI_MATKEY_NAME, matName);

        printf("Found Material...%s\n", matName.C_Str());

        aiString texString;
        mat->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &texString);
        string textureFileName = string(texString.C_Str());
        if (textureFileName.find(".") == string::npos) {
            textureFileName += "_D.tga";
        }
        
        string texturePath = m_directory + "/" + textureFileName;
        if (texString.length > 0) {
            printf("Looking for texture: %s in model.", textureFileName.c_str());
            auto existingMat = std::find_if(std::begin(jScene->GetMaterialCache().GetCache()), std::end(jScene->GetMaterialCache().GetCache()),
            [&](const std::unique_ptr<Material>& mm) {
                return mm->GetName() == matName.C_Str();
            });
            if (existingMat != std::end(jScene->GetMaterialCache().GetCache())) {
                myMaterial = existingMat->get();
            } else {
                myMaterial = jScene->GetMaterialCache().CreateInstance<Material>(m_shader, matName.C_Str());

                aiColor3D diffuse, ambient, specular;
                float shine;
                mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
                mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
                mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
                mat->Get(AI_MATKEY_SHININESS, shine);
                if (shine == 0.0f) {
                    shine = 64.0f;
                }
                myMaterial->SetTextureDiffuse(texturePath);
                myMaterial->Ambient = Vector3(ambient.r, ambient.g, ambient.b);
                myMaterial->Diffuse = Vector3(diffuse.r, diffuse.g, diffuse.b);
                myMaterial->Specular = Vector3(specular.r, specular.g, specular.b);
                myMaterial->Shine = shine / 4.0f;
                // try to load a normal map
                texString.Clear();
                mat->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &texString);
                if (texString.length > 0) {
                    myMaterial->SetTextureNormalMap(m_directory + "/" + texString.C_Str());
                } else {
                    texString.Clear();
                    mat->GetTexture(aiTextureType::aiTextureType_HEIGHT, 0, &texString);
                    if (texString.length > 0) {
                        myMaterial->SetTextureNormalMap(m_directory + "/" + texString.C_Str());
                    }
                }
                texString.Clear();
                mat->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &texString);
                if (texString.length > 0) {

                    myMaterial->SetTextureSpecularMap(m_directory + "/" + texString.C_Str());
                }
            }
        }
    }

    if (!aiMesh->HasNormals()) {
        m->CalculateFaceNormals();
    }

    Material* renderMaterial;
    if (myMaterial) {
        renderMaterial = myMaterial;
    } else {
        renderMaterial = jScene->GetMaterialCache().CreateInstance<Material>(m_shader, "jasper_default_material");
        renderMaterial->SetTextureDiffuse("./textures/default.png");
    }
    m->m_material = renderMaterial;
    //auto mr = this->AttachNewComponent<MeshRenderer>(m, renderMaterial);

    printf("Loaded Model Mesh\n");

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
		const char * const    operation)
	{
		cout << (int)(overallProgress + 0.5) << "% "
			<< "[ " << stage << " " << (int)(stageProgress + 0.5) << "% ] "
			<< operation << " "  << (int)(operationProgress + 0.5) << "%" << endl;
	};
};

void Model::ConvexDecompose(Mesh* mesh, std::vector<std::unique_ptr<btConvexHullShape>>& shapes, Scene* scene)
{	
	using namespace VHACD;
	MyCallback mcallback;
	IVHACD::Parameters params;
	params.m_resolution = 100000;
	params.m_depth = 20;
	params.m_concavity = 0.01;
	params.m_planeDownsampling = 8;
	params.m_convexhullDownsampling = 8;
	params.m_alpha = 0.05;
	params.m_beta = 0.05;
	params.m_gamma = 0.0015;
	params.m_pca = 0;
	params.m_mode = 0; // 0: voxel-based (recommended), 1: tetrahedron-based
	params.m_maxNumVerticesPerCH = 64;
	params.m_minVolumePerCH = 0.01;
	params.m_callback = &mcallback;
	params.m_logger = 0;
	
	
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
		auto debugShader = scene->GetShaderCache().GetResourceByName("Basic_Shader"s);
		auto mat = scene->GetMaterialCache().CreateInstance<Material>(debugShader, "ch_material");
		mat->IsTransparent = true;
		uint nConvexHulls = cdInterface->GetNConvexHulls();
		
		IVHACD::ConvexHull ch;
		for (unsigned int p = 0; p < nConvexHulls; ++p)
		{
			
			cdInterface->GetConvexHull(p, ch); // get the p-th convex-hull information			
			std::unique_ptr<btConvexHullShape> bhull = make_unique<btConvexHullShape>(nullptr, 0);

			//interfaceVHACD->GetConvexHull(p, ch); // get the p-th convex-hull information
			Mesh* testMesh = scene->GetMeshCache().CreateInstance<Mesh>("ch_" + std::to_string(p));
			float dr = ((float)rand() / (RAND_MAX));
			float dg = ((float)rand() / (RAND_MAX));
			float db = ((float)rand() / (RAND_MAX));
			testMesh->Color = { dr, dg, db, 0.95f };
			for (unsigned int v = 0, idx = 0; v < ch.m_nPoints; ++v, idx += 3)
			{
				Vector3 vp = Vector3(ch.m_points[idx], ch.m_points[idx + 1], ch.m_points[idx + 2]);
				testMesh->Positions.push_back(vp);				
				btVector3 vv = btVector3(ch.m_points[idx], ch.m_points[idx + 1], ch.m_points[idx + 2]);
				bhull->addPoint(vv);
			}
			
			for (unsigned int t = 0, idx = 0; t < ch.m_nTriangles; ++t, idx += 3)
			{
				testMesh->Indices.push_back(ch.m_triangles[idx]);
				testMesh->Indices.push_back(ch.m_triangles[idx + 1]);
				testMesh->Indices.push_back(ch.m_triangles[idx + 2]);
				
			}
			
			
			auto go = GetGameObject();
			go->AttachNewComponent<MeshRenderer>("ch_renderer_" + to_string(p), testMesh, mat);
			
			

			

		
						
			shapes.emplace_back(move(bhull));			

		}
	}

	delete[] tris;
	//delete[] pos;
	cdInterface->Clean();
	cdInterface->Release();	

}

void Model::CalculateHalfExtents()
{
    float minx, miny, minz;
    minx = miny = minz = 1000000.f;
    float maxx, maxy, maxz;
    maxx = maxy = maxz = -100000.f;
    for (auto& mesh : m_meshManager.GetCache()) {
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

void Model::Destroy()
{
    m_materialManager.Clear();
    m_meshManager.Clear();
    Component::Destroy();
}

void Model::Awake()
{
    Initialize();
    Component::Awake();
}

void Model::Update(float dt)
{
    Component::Update(dt);
}

void Model::SaveToAssetFile(const std::string& filename)
{

    auto& meshes = m_meshManager.GetCache();
    int numMeshes = meshes.size();
    int numMaterials = m_materialManager.GetCache().size();

    ofstream ofs;
    ofs.open(filename, std::ios::binary | std::ios::out);

    ofs.write( reinterpret_cast<char*>(&numMeshes), sizeof(numMeshes) );

    for(const auto& mesh : meshes) {
        AssetSerializer::SerializeMesh(ofs, mesh.get());
    }

    ofs.close();


}

void Model::OutputMeshData()
{
    ofstream ofs ;
    ofs.open("mesh_data.txt", ios::out);

    auto& meshes = m_meshManager.GetCache();
    int numMeshes = meshes.size();

    for (auto& mesh : meshes) {
        ofs << "// Outputting Mesh data for: " << GetName() << "\n";
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
