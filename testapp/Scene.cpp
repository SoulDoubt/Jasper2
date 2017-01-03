#include <memory>
#include <chrono>
#include "Scene.h"
#include "Quad.h"
#include "Cube.h"
#include "Material.h"
#include "BasicShader.h"
#include "LitShader.h"
#include "SkyboxShader.h"
#include "TextureShader.h"
#include "MeshRenderer.h"
#include "SkyboxRenderer.h"
#include "Model.h"
#include "PlaneCollider.h"
#include "BoxCollider.h"
#include "Sphere.h"
#include "SphereCollider.h"
#include <fstream>
#include "AssetSerializer.h"
#include <random>

#include <typeinfo>
#include <typeindex>

#define DEBUG_DRAW_PHYSICS

namespace Jasper
{

using namespace std;

std::unique_ptr<GameObject> desRoot = nullptr;
ResourceManager<Mesh> desMeshes;
ResourceManager<Material> desMaterials;
ResourceManager<Shader> desShaders;

Scene::Scene(int width, int height)// : m_camera(Camera::CameraType::FLYING)
{
	m_windowWidth = width;
	m_windowHeight = height;
}

Scene::~Scene() {
	m_meshManager.Clear();
	m_shaderManager.Clear();
	m_materialManager.Clear();
	m_textureManager.Clear();
	m_modelManager.Clear();
	m_scripts.clear();
}

void Scene::Resize(int width, int height) {

	printf("Resizing scene... %d x %d\n", width, height);
	m_windowWidth = width;
	m_windowHeight = height;

	auto pm = Matrix4();
	auto om = Matrix4();

	float aspectRatio = (float)m_windowWidth / (float)m_windowHeight;

	pm.CreatePerspectiveProjection(30, aspectRatio, 0.1f, 1000.0f);
	om.CreateOrthographicProjection(0.0f, m_windowWidth, m_windowHeight, 0.0f, 1.0f, -1.0f);
	m_projectionMatrix = pm;
	m_orthoMatrix = om;
}

void Scene::Serialize(const std::string& filepath) {
	using namespace AssetSerializer;
	ofstream ofs;
	ofs.open(filepath.c_str(), ios::out | ios::binary);
	// first serialize the shaders

	// write the count of shaders as the first thing in the file.
	auto& shaderCache = m_shaderManager.GetCache();
	size_t shaderCount = shaderCache.size();
	ofs.write(CharPtr(&shaderCount), sizeof(shaderCount));
	for (const auto& shader : m_shaderManager.GetCache()) {
		SerializeShader(ofs, shader.get());
	}

	// now the materials
	const size_t materialCount = m_materialManager.GetCache().size();
	ofs.write(ConstCharPtr(&materialCount), sizeof(materialCount));
	for (const auto& material : m_materialManager.GetCache()) {
		SerializeMaterial(ofs, material.get());
	}

	const size_t meshCount = m_meshManager.GetCache().size();
	ofs.write(ConstCharPtr(&meshCount), sizeof(meshCount));
	for (const auto& mesh : m_meshManager.GetCache()) {
		mesh->Serialize(ofs);
	}

	//serialize GameObjects
	SerializeGameObject(m_rootNode.get(), ofs);

	ofs.close();

}

void Scene::SerializeGameObject(const GameObject* go, std::ofstream& ofs) {
	using namespace AssetSerializer;
	// write the game object name
	// write the transform 
	// write the number of children
	// write each child (recursively);

	const GameObjectType typ = go->GetGameObjectType();
	ofs.write(ConstCharPtr(&typ), sizeof(typ));
	const string name = go->GetName();
	const size_t namesize = name.size();
	ofs.write(ConstCharPtr(&namesize), sizeof(namesize));
	ofs.write(name.data(), namesize);
	const Transform t = go->GetWorldTransform();
	ofs.write(ConstCharPtr(t.Position.AsFloatPtr()), sizeof(t.Position));
	ofs.write(ConstCharPtr(t.Orientation.AsFloatPtr()), sizeof(t.Orientation));
	ofs.write(ConstCharPtr(t.Scale.AsFloatPtr()), sizeof(t.Scale));
	//not the components for this game object
	const size_t componentsize = go->Components().size();
	ofs.write(ConstCharPtr(&componentsize), sizeof(componentsize));
	for (const auto& cmp : go->Components()) {
		cmp->Serialize(ofs);
	}

	// now write the children recursively
	size_t childsize = go->Children().size();
	ofs.write(ConstCharPtr(&childsize), sizeof(childsize));
	for (const auto& child : go->Children()) {
		SerializeGameObject(child.get(), ofs);
	}

}

void Scene::DeserializeGameObject(std::ifstream & ifs, GameObject* parent) {
	using namespace AssetSerializer;
	GameObjectType typ;
	ifs.read(CharPtr(&typ), sizeof(typ));
	size_t namesize;
	ifs.read(CharPtr(&namesize), sizeof(namesize));
	char* goname = new char[namesize + 1];
	ifs.read(goname, namesize);
	goname[namesize] = '\0';
	string name = string(goname);
	delete[] goname;
	Transform trans;
	ifs.read(CharPtr(trans.Position.AsFloatPtr()), sizeof(trans.Position));
	ifs.read(CharPtr(trans.Orientation.AsFloatPtr()), sizeof(trans.Orientation));
	ifs.read(CharPtr(trans.Scale.AsFloatPtr()), sizeof(trans.Scale));
	// construct the game object as a child of the parent
	GameObject* gameObject = nullptr;
	if (!parent) {
		m_rootNode = make_unique<GameObject>(name);
		m_rootNode->SetScene(this);
		m_rootNode->GetLocalTransform().Position = trans.Position;
		m_rootNode->GetLocalTransform().Orientation = trans.Orientation;
		m_rootNode->GetLocalTransform().Scale = trans.Scale;
		gameObject = m_rootNode.get();
	} else {
		Camera* c = nullptr;
		DirectionalLight* dl = nullptr;
		PointLight* pl = nullptr;

		switch (typ) {
		case GameObjectType::Default:
			gameObject = parent->AttachNewChild<GameObject>(name);
			break;
		case GameObjectType::Camera:
		{
			Camera* c = parent->AttachNewChild<Camera>(Camera::CameraType::FLYING);
			gameObject = c;
			m_camera = c;
		}
		break;
		case GameObjectType::DirectionalLight:
		{
			DirectionalLight* dl = parent->AttachNewChild<DirectionalLight>(name);
			gameObject = dl;
		}
		break;
		case GameObjectType::PointLight:

			PointLight* pl = parent->AttachNewChild<PointLight>(name);
			gameObject = pl;
			break;
		}
		assert(gameObject);
		gameObject->GetLocalTransform().Position = trans.Position;
		gameObject->GetLocalTransform().Orientation = trans.Orientation;
		gameObject->GetLocalTransform().Scale = trans.Scale;
	}


	// get the components	
	size_t componentsize;
	ifs.read(CharPtr(&componentsize), sizeof(componentsize));
	for (size_t c = 0; c < componentsize; ++c) {
		// grab the name and component type
		size_t componentNameSize;
		ifs.read(CharPtr(&componentNameSize), sizeof(componentNameSize));
		char* cnamebuff = new char[componentNameSize + 1];
		ifs.read(cnamebuff, componentNameSize);
		cnamebuff[componentNameSize] = '\0';
		string componentName = string(cnamebuff);
		delete[] cnamebuff;

		ComponentType ct;
		ifs.read(CharPtr(&ct), sizeof(ct));
		switch (ct) {
		case ComponentType::MeshRenderer:
			ConstructMeshRenderer(ifs, componentName, gameObject, this);
			break;
		case ComponentType::PhysicsCollider:
			ConstructPhysicsCollider(ifs, componentName, gameObject, this);
			break;
		case ComponentType::SkyboxRenderer:
			ConstructSkyboxRenderer(ifs, componentName, gameObject, this);
			break;
		case ComponentType::ScriptComponent:
			ConstructScriptComponent(ifs, componentName, gameObject, this);
			break;
		default:
			int x = 0;
			printf("Unhandled Component Type in deserialize..\n");
		}

	}
	size_t childSize;
	ifs.read(CharPtr(&childSize), sizeof(childSize));
	for (size_t i = 0; i < childSize; ++i) {
		DeserializeGameObject(ifs, gameObject);
	}


}

void Scene::DebugDrawPhysicsWorld() {
	auto viewMatrix = m_camera->GetViewMatrix().Inverted();
	for (auto& go : m_rootNode->Children()) {
		auto phys = go->GetComponentsByType<PhysicsCollider>();
		if (phys.size() > 0) {
			auto trans = go->GetWorldTransform();
			trans.Scale = { 1.f, 1.f, 1.f };
			auto mvp = m_projectionMatrix * viewMatrix * trans.TransformMatrix();
			m_physicsWorld->debugDrawer->mvpMatrix = mvp;
			btTransform dt;
			dt.setIdentity();
			for (auto collider : phys) {
				if (collider->GetColliderType() != PHYSICS_COLLIDER_TYPE::ConvexHull) {
					m_physicsWorld->debugDrawer->debugShader->Bind();
					Vector4 color = collider->GetDebugColor();
					reinterpret_cast<BasicShader*>(m_physicsWorld->debugDrawer->debugShader)->SetColor(color);
					m_physicsWorld->DrawPhysicsShape(dt, collider->GetCollisionShape(), { 1.f, 1.f, 1.f });
				}
			}
		}
		for (auto& child : go->Children()) {
			auto phys = child->GetComponentsByType<PhysicsCollider>();
			if (phys.size() > 0) {
				auto trans = child->GetWorldTransform();
				trans.Scale = { 1.f, 1.f, 1.f };
				auto mvp = m_projectionMatrix * viewMatrix * trans.TransformMatrix();
				m_physicsWorld->debugDrawer->mvpMatrix = mvp;
				btTransform dt;
				dt.setIdentity();
				for (auto collider : phys) {
					if (collider->GetColliderType() != PHYSICS_COLLIDER_TYPE::ConvexHull) {
						m_physicsWorld->DrawPhysicsShape(dt, collider->GetCollisionShape(), { 1.f, 1.f, 1.f });
					}
				}
			}
		}
	}
}

void Scene::Deserialize(const std::string& filepath) {
	using namespace AssetSerializer;

	ifstream ifs;
	ifs.open(filepath, ios::in | ios::binary);

	// first are the shaders.
	size_t shadercount;
	ifs.read(CharPtr(&shadercount), sizeof(shadercount));
	for (int i = 0; i < shadercount; ++i) {
		AssetSerializer::ConstructShader(ifs, this);
	}

	size_t materialcount;
	ifs.read(CharPtr(&materialcount), sizeof(materialcount));
	for (int i = 0; i < materialcount; ++i) {
		AssetSerializer::ConstructMaterial(ifs, this);
	}

	size_t meshCount;
	ifs.read(CharPtr(&meshCount), sizeof(meshCount));
	for (int i = 0; i < meshCount; ++i) {
		ConstructMesh(ifs, this);
	}

	DeserializeGameObject(ifs, nullptr);

	ifs.close();

}

void Scene::InitializeManual() {
	m_rootNode = make_unique<GameObject>("Root_Node");
	m_rootNode->SetScene(this);

	m_player = m_rootNode->AttachNewChild<CharacterController>();
	m_player->AttachNewComponent<CapsuleCollider>("player_collider"s, Vector3(0.75f, 1.82f, 0.75f), m_physicsWorld.get());
	m_camera = m_player->AttachNewChild<Camera>(Camera::CameraType::FLYING);
	m_camera->GetLocalTransform().Position = { 0.f, 0.f, 6.f };
	//m_camera->SetPhysicsWorld(m_physicsWorld.get());
	
	m_player->GetLocalTransform().Position = { 10.0f, 5.0f, 0.0f };
	auto sh = m_player->AttachNewComponent<ShooterScript>();
	sh->Force = 50;

	// perform actual game object initialization

	//create the skybox
	auto skybox = m_rootNode->AttachNewChild<GameObject>("skybox");
	auto skyboxMesh = m_meshManager.CreateInstance<Cube>("skybox_cube_mesh", Vector3(100.0f, 100.0f, 100.0f), true);
	skyboxMesh->SetCubemap(true); // we want to render the inside of the cube
	skyboxMesh->Initialize();
	auto skyboxShader = m_shaderManager.CreateInstance<SkyboxShader>("skybox_shader");
	auto skyboxMaterial = m_materialManager.CreateInstance<Material>(skyboxShader, "skybox_material");
	string posx = "../textures/darkskies/darkskies_lf.tga"s;
	string negx = "../textures/darkskies/darkskies_ft.tga"s;
	string posy = "../textures/darkskies/darkskies_up.tga"s;
	string negy = "../textures/darkskies/darkskies_dn.tga"s;
	string posz = "../textures/darkskies/darkskies_rt.tga"s;
	string negz = "../textures/darkskies/darkskies_bk.tga"s;
	skyboxMaterial->SetCubemapTextures(posx, negz, posy, negy, posz, negx);
	skybox->AttachNewComponent<SkyboxRenderer>("skybox_renderer", skyboxMesh, skyboxMaterial);

	// create the Basic Shader Instance to render most objects
	auto defaultShader = m_shaderManager.CreateInstance<LitShader>();

	m_fontRenderer = make_unique<FontRenderer>();
	m_fontRenderer->Initialize();
	m_fontRenderer->SetOrthoMatrix(m_orthoMatrix);

	Material* m1 = m_materialManager.CreateInstance<Material>(defaultShader, "wall_material");
	m1->SetTextureDiffuse("../textures/196.JPG");
	m1->SetTextureNormalMap("../textures/196_norm.JPG");
	m1->Diffuse = { 0.85f, 0.85f, 0.85f };
	m1->Ambient = { 0.25f, 0.25f, 0.25f };
	m1->Specular = { 0.99f, 0.99f, 0.9f };
	m1->Shine = 255;

	//    std::ofstream serializer;
	//    serializer.open("scenedata.scene", ios::binary | ios::out);
	//    
	//    AssetSerializer::SerializeMaterial(serializer, m1);
	//    serializer.close();
	//    
	//    std::ifstream reader;
	//    reader.open("scenedata.scene", ios::in | ios::binary);
	//    AssetSerializer::ConstructMaterial(reader, this);
		// Floor
	auto floor = m_rootNode->AttachNewChild<GameObject>("floor");
	auto quadMesh = m_meshManager.CreateInstance<Quad>("floor_quad", Vector2(100.0f, 100.0f), 25, 25, Quad::AxisAlignment::XZ);
	Material* floorMaterial = m_materialManager.CreateInstance<Material>(defaultShader, "floor_material");
	floorMaterial->SetTextureDiffuse("../textures/186.JPG");
	floorMaterial->SetTextureNormalMap("../textures/186_norm.JPG");
	floorMaterial->Specular = { 0.8f, .9f, .9f };
	floorMaterial->Shine = 64;
	floor->AttachNewComponent<MeshRenderer>("quad_renderer", quadMesh, floorMaterial);
	floor->GetLocalTransform().Translate(Vector3(0.0f, -1.f, 0.0f));
	auto floorP = floor->AttachNewComponent<PlaneCollider>("floor_collider", Vector3(0.0, 1.0, 0.0), 0.0, m_physicsWorld.get());
	floorP->Friction = 0.9f;
	//floorP->Serialize(serializer);
	floorMaterial->Ambient = { 0.0f, 0.0f, 0.0f };

	auto basic = m_shaderManager.GetResourceByName("basic_shader"s);
	auto red = m_materialManager.CreateInstance<Material>(basic, "red_material"s);
	


	// wall
	/*auto wall = m_rootNode->AttachNewChild<GameObject>("wall_0");
	auto wallMesh = m_meshManager.CreateInstance<Cube>("wall_mesh", Vector3(50.f, 50.0f, 3.0f), 10.f, 10.f);
	wall->AttachNewComponent<MeshRenderer>("wall_renderer", wallMesh, m1);
	auto wallCollider = wall->AttachNewComponent<BoxCollider>("wall_0_collider", wallMesh->GetHalfExtents(), m_physicsWorld.get());
	wallCollider->Mass = 1.0f;
	wall->GetLocalTransform().Translate(0.0f, 25.0f, -20.0f);*/
	//wallCollider->Serialize(serializer);

	// launcher
	auto launcher = m_rootNode->AttachNewChild<GameObject>("Launcher"s);
	launcher->GetLocalTransform().Translate(0.f, 1.f, 10.f);
	//launcher->AttachNewComponent<LauncherScript>("Launcher_script");

	// secondary launcher
	auto launcher2 = launcher->AttachNewChild<GameObject>("Launcher2"s);
	launcher2->GetLocalTransform().Translate(10.f, 2.f, 5.f);
	//launcher2->AttachNewComponent<LauncherScript>("Launcher2_script");

	auto model = m_rootNode->AttachNewChild<GameObject>("mathias_model"s);
	auto mdl = model->AttachNewComponent<Model>("mathias"s, "../models/mathias/mathias.obj"s, defaultShader, true, m_physicsWorld.get());
	mdl->ColliderType = PHYSICS_COLLIDER_TYPE::Compound;
	//model->GetLocalTransform().Scale = { 0.2f, 0.2f, 0.2f };
	mdl->Setup(this);
	
	auto collider = model->GetComponentByType<PhysicsCollider>();
	if (collider) {
		//collider->GetCollisionShape()->setLocalScaling({ 0.035f, 0.035f, 0.035f });
		collider->Mass = 20.f;
	}
	////model->getlocaltransform().uniformscale(0.02f);
	//if (collider) {
	//	//collider->GetCollisionShape()->setLocalScaling({ 0.015f, 0.015f, 0.015f });
	//}
	auto unused = model->DetachComponent(*mdl);
	//model->AttachNewComponent<RotateInPlaceScript>("rotate_in_place_script", Vector3(0.f, 1.f, 0.f), 90);

	auto light0 = m_rootNode->AttachNewChild<PointLight>("p_light"s);
	light0->GetLocalTransform().Translate({ 0.0f, 10.f, 15.0f });
	light0->ConstAtten = 0.002f;
	light0->Color = { 1.f, 0.f, 0.f };
	light0->AmbientIntensity = 0.15f;
	light0->DiffuseIntensity = 1.f;
	light0->Radius = 12.0f;
	auto lightMesh = m_meshManager.CreateInstance<Cube>("point_light_mesh"s, Vector3(0.1f, 0.1f, 0.1f));
	auto lightMaterial = m_materialManager.CreateInstance<Material>(defaultShader, "point_light_material"s);
	lightMaterial->SetTextureDiffuse("../textures/white.jpg"s);
	light0->AttachNewComponent<MeshRenderer>("point_light_renderer"s, lightMesh, lightMaterial);
	light0->AttachNewComponent<RotateAboutPointScript>("Rotate_Light_Script"s, Vector3(0.f, 7.5f, 0.f), Vector3(0.f, 1.f, 0.f), 45);
	//

	auto dlight = m_rootNode->AttachNewChild<DirectionalLight>("d_light"s);
	dlight->Direction = Normalize(Vector3(0.0, -1.f, 0.0f));
	dlight->AmbientIntensity = 0.01f;
	dlight->Diffuseintensity = 0.85f;
}

void Scene::Initialize() {

	srand(time(nullptr));
	m_physicsWorld = make_unique<PhysicsWorld>(this);
	m_physicsWorld->Initialize();

	m_renderer = make_unique<Renderer>(this);

	Resize(m_windowWidth, m_windowHeight);

	auto debugShader = m_shaderManager.CreateInstance<BasicShader>();
	debugShader->SetColor(Vector4(0.f, 1.f, 0.f, 1.0f));
	m_physicsWorld->debugDrawer->debugShader = debugShader;
	m_physicsWorld->debugDrawer->Initialize();

	InitializeManual();
	//Deserialize("../scenes/scenedata.scene");
	m_renderer->Initialize();
	m_rootNode->Initialize();
	//Serialize("../scenes/scenedata.scene");
}

Shader* Scene::GetShaderByName(const std::string& name) {
	return m_shaderManager.GetResourceByName(name);
}

Material* Scene::GetMaterialByName(const std::string& name) {

	return m_materialManager.GetResourceByName(name);
}

void Scene::AddGameObject(std::unique_ptr<GameObject> go) {
	m_rootNode->AttachChild(move(go));
}

GameObject* Scene::GetGameObjectByName(const string & name) {
	const auto gp = m_rootNode->FindChildByName(name);
	if (gp) {
		return gp;
	}
	return nullptr;
}


static const int max_samples = 64;
float fpsSamples[max_samples];

float CalcFPS(float dt) {
	static int frameNumber = 0;
	fpsSamples[frameNumber % max_samples] = 1.0f / dt;
	float fps = 0;
	for (int i = 0; i < max_samples; i++) {
		fps += fpsSamples[i];
	}
	fps /= max_samples;
	frameNumber++;
	return fps;
}

Vector3 pb = { 20.f, 1.f, -20.f };
Vector3 pa = { -10.f, 1.f, -3.f };
float lerpTime = 1.0f;
float currentLerpTime = 0.f;

void Scene::Update(float dt) {
	using namespace std::chrono;

	// step the physics simulation
	const auto physicsStart = high_resolution_clock::now();
	m_physicsWorld->Update(dt);
	const auto physicsEnd = high_resolution_clock::now();
	const auto physicsTime = physicsEnd - physicsStart;
	const double pt = physicsTime.count();
	this->PhysicsFrameTime = pt / 1000000;
	// updating game objects will collect their updated physics transforms
	// and perform any scripted activities.
	const auto updateStart = high_resolution_clock::now();
	m_rootNode->Update(dt);
	const auto updateEnd = high_resolution_clock::now();
	auto updateMili = duration_cast<milliseconds>(updateEnd - updateStart);
	double ut = updateMili.count();
	this->UpdateFrameTime = ut;
	// after update the scene is ready for rendering...
	m_rootNode->LateUpdate();
	const auto renderStart = high_resolution_clock::now();
	m_renderer->RenderScene();
	
	const auto renderEnd = high_resolution_clock::now();
	const auto renderTime = duration_cast<nanoseconds>(renderEnd - renderStart);
	long long rt = renderTime.count();
	this->RendererFrameTime = (double)rt / 1000000.0;

	debug_draw_physics = true;
	if (debug_draw_physics) {
		DebugDrawPhysicsWorld();
	}

}


void Scene::Awake() {
	
	m_rootNode->Awake();
	
	for (auto& mesh : m_meshManager.GetCache()) {
		//mesh->UnloadClientData();
	}

}

void Scene::Start() {
	m_rootNode->Start();
}

void Scene::Destroy() {
	m_rootNode->Destroy();
	m_meshManager.Clear();
	m_shaderManager.Clear();
	m_materialManager.Clear();
	m_textureManager.Clear();
}

GameObject* Scene::CreateEmptyGameObject(std::string name, GameObject* parent) {
	auto goup = make_unique<GameObject>(name);
	auto ret = goup.get();
	parent->AttachChild(move(goup));
	return ret;
}

void Scene::DoLeftClick(double x, double y) {
	//if (!m_)
	auto sh = m_player->GetComponentByType<ShooterScript>();
	sh->ShootRay();
}

void Scene::DestroyGameObject(std::unique_ptr<GameObject> object) {
	GameObject* go = object.release();
	go->Destroy();
	delete go;
}

void Scene::DestroyGameObject(GameObject* go) {
	m_renderer->UnregisterGameObject(go);
	auto parent = go->GetParent();
	auto up = parent->DetachChild(*go);
	up->Destroy();
}

} // namespace Jasper
