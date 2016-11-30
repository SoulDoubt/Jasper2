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




namespace Jasper
{

using namespace std;

Scene::Scene(int width, int height)// : m_camera(Camera::CameraType::FLYING)
{
    m_windowWidth = width;
    m_windowHeight = height;
}

Scene::~Scene()
{
    m_meshManager.Clear();
    m_shaderManager.Clear();
    m_materialManager.Clear();
    m_textureManager.Clear();
    m_modelManager.Clear();
    m_scripts.clear();
}

void Scene::Resize(int width, int height)
{

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

void Scene::Serialize(const std::string& filepath){
    using namespace AssetSerializer;
    ofstream ofs;
    ofs.open(filepath.c_str(), ios::out | ios::binary);
    // first serialize the shaders
    
    // write the count of shaders as the first thing in the file.
    auto& shaderCache = m_shaderManager.GetCache();
    size_t shaderCount = shaderCache.size();
    ofs.write(CharPtr(&shaderCount), sizeof(shaderCount));
    for (const auto& shader : m_shaderManager.GetCache()){ 
        SerializeShader(ofs, shader.get());
    }
    
    // now the materials
    size_t materialCount = m_materialManager.GetCache().size();
    ofs.write(CharPtr(&materialCount), sizeof(materialCount));
    for (const auto& material : m_materialManager.GetCache()){
        SerializeMaterial(ofs, material.get());
    }
    
    //serialize GameObjects
    SerializeGameObject(m_rootNode.get(), ofs);
    
    ofs.close();
    
}

void Scene::SerializeGameObject(const GameObject* go, std::ofstream& ofs){
    using namespace AssetSerializer;
    // write the game object name
    // write the transform 
    // write the number of children
    // write each child (recursively);
    const string name = go->GetName();
    size_t namesize = name.size();
    ofs.write(CharPtr(&namesize), sizeof(namesize));
    ofs.write(name.c_str(), namesize);
    Transform t = go->GetWorldTransform();
    ofs.write(CharPtr(t.Position.AsFloatPtr()), sizeof(t.Position));
    ofs.write(CharPtr(t.Orientation.AsFloatPtr()), sizeof(t.Orientation));
    //not the components for this game object
    const size_t componentsize = go->Components().size();
    ofs.write(ConstCharPtr(&componentsize), sizeof(componentsize));
    for (const auto& cmp : go->Components()){
        cmp->Serialize(ofs);
    }
    
    for (const auto& child : go->Children()){
        SerializeGameObject(go->get(), ofs);
    }
}

void Scene::Deserialize(const std::string& filepath){
    using namespace AssetSerializer;
    
    ifstream ifs;
    ifs.open(filepath, ios::in | ios::binary);
    
    // first are the shaders.
    size_t shadercount;
    ifs.read(CharPtr(&shadercount), sizeof(shadercount));
    for (int i = 0; i < shadercount; i++){
        AssetSerializer::ConstructShader(ifs, this);
    }
    
    size_t materialcount;
    ifs.read(CharPtr(&materialcount), sizeof(materialcount));
    for (int i = 0; i < materialcount; i++){
        AssetSerializer::ConstructMaterial(ifs, this);
    }
    
    ifs.close();
    
}

void Scene::Initialize()
{

    m_rootNode = make_unique<GameObject>("Root_Node");
    m_rootNode->SetScene(this);

    m_physicsWorld = make_unique<PhysicsWorld>(this);
    m_physicsWorld->Initialize();

    m_renderer = make_unique<Renderer>(this);

    Resize(m_windowWidth, m_windowHeight);

    auto debugShader = m_shaderManager.CreateInstance<BasicShader>();
    m_physicsWorld->debugDrawer->debugShader = debugShader;
    m_physicsWorld->debugDrawer->Initialize();

    // create the player
    //btPairCachingGhostObject* ghost = new btPairCachingGhostObject();
    //ghost->setBroadphaseHandle(m_physicsWorld->get());
    //auto playerShape = new btCapsuleShape(0.5f, 1.76f);
    //m_player = make_unique<CharacterController>(ghost, playerShape, 1.0f, m_physicsWorld.get());


    //m_player = make_unique<CharacterController>()

    m_camera = m_rootNode->AttachNewChild<Camera>(Camera::CameraType::FLYING);
    m_camera->SetPhysicsWorld(m_physicsWorld.get());
    //m_camera->AttachNewComponent<CapsuleCollider>("camera_collider", Vector3(1.f, 2.f, 1.f), m_physicsWorld.get());

    // perform actual game object initialization

    //create the skybox
    auto skybox = m_rootNode->AttachNewChild<GameObject>("skybox");
    auto skyboxMesh = m_meshManager.CreateInstance<Cube>("skybox_cube_mesh", Vector3(100.0f, 100.0f, 100.0f), true);
    skyboxMesh->SetCubemap(true); // we want to render the inside of the cube
    auto skyboxShader = m_shaderManager.CreateInstance<SkyboxShader>("skybox_shader");
    auto skyboxMaterial = m_materialManager.CreateInstance<Material>(skyboxShader, "skybox_material");
    string posx = "../textures/darkskies/darkskies_lf.tga";
    string negx = "../textures/darkskies/darkskies_ft.tga";
    string posy = "../textures/darkskies/darkskies_up.tga";
    string negy = "../textures/darkskies/darkskies_dn.tga";
    string posz = "../textures/darkskies/darkskies_rt.tga";
    string negz = "../textures/darkskies/darkskies_bk.tga";
    skyboxMaterial->SetCubemapTextures(posx, negz, posy, negy, posz, negx);
    skybox->AttachNewComponent<SkyboxRenderer>(skyboxMesh, skyboxMaterial);

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
    floorMaterial->Specular = {0.8f, .9f, .9f};
    floorMaterial->Shine = 64;
    floor->AttachNewComponent<MeshRenderer>(quadMesh, floorMaterial);
    floor->GetLocalTransform().Translate(Vector3( 0.0f, -1.f, 0.0f ));
    auto floorP = floor->AttachNewComponent<PlaneCollider>("plane_collider", quadMesh, m_physicsWorld.get());
    floorP->Friction = 0.9f;
    //floorP->Serialize(serializer);
    floorMaterial->Ambient = { 0.0f, 0.0f, 0.0f };

    // wall
    auto wall = m_rootNode->AttachNewChild<GameObject>("wall_0");
    auto wallMesh = m_meshManager.CreateInstance<Cube>("wall_mesh", Vector3(50.f, 50.0f, 3.0f), 10.f, 10.f);
    wall->AttachNewComponent<MeshRenderer>(wallMesh, m1);
    auto wallCollider = wall->AttachNewComponent<BoxCollider>("wall_0_collider", wallMesh, m_physicsWorld.get());
    wallCollider->Mass = 1.0f;
    wall->GetLocalTransform().Translate(0.0f, 25.0f, -20.0f);
    //wallCollider->Serialize(serializer);

    // launcher
    auto launcher = m_rootNode->AttachNewChild<GameObject>("Launcher");
    launcher->GetLocalTransform().Translate(0.f, 1.f, 10.f);
    //launcher->AttachNewComponent<LauncherScript>("Launcher_script");

    // secondary launcher
    auto launcher2 = launcher->AttachNewChild<GameObject>("Launcher2");
    launcher2->GetLocalTransform().Translate(10.f, 2.f, 5.f);
    //launcher2->AttachNewComponent<LauncherScript>("Launcher2_script");

    auto model = m_rootNode->AttachNewChild<GameObject>("mathias_model");
    auto mdl = model->AttachNewComponent<Model>("mathias", "../models/Mathias/Mathias.obj", defaultShader, true, m_physicsWorld.get());
    mdl->Setup(this);
    auto collider = model->GetComponentByType<PhysicsCollider>();
    //model->getlocaltransform().uniformscale(0.02f);
    if (collider) {
        collider->Mass = 75.0f;
        collider->Restitution = 0.2f;
    }
    //model->AttachNewComponent<RotateInPlaceScript>("rotate_in_place_script", Vector3(0.f, 1.f, 0.f), 90);

    auto light0 = m_rootNode->AttachNewChild<PointLight>("p_light");
    light0->GetLocalTransform().Translate( { 0.0f, 10.f, 15.0f });
    light0->ConstAtten = 0.002f;
    light0->Color = { 1.f, 0.f, 0.f };
    light0->AmbientIntensity = 0.15f;
    light0->DiffuseIntensity = 1.f;
    light0->Radius = 12.0f;
    auto lightMesh = m_meshManager.CreateInstance<Cube>("point_light_mesh", Vector3(0.1f, 0.1f, 0.1f));
    auto lightMaterial = m_materialManager.CreateInstance<Material>(defaultShader, "point_light_material");
    lightMaterial->SetTextureDiffuse("../textures/white.jpg");
    light0->AttachNewComponent<MeshRenderer>(lightMesh, lightMaterial);
    light0->AttachNewComponent<RotateAboutPointScript>("Rotate_Light_Script", Vector3(0.f, 7.5f, 0.f), Vector3(0.f, 1.f, 0.f), 45);
//

    auto dlight = m_rootNode->AttachNewChild<DirectionalLight>("d_light");
    dlight->Direction = Normalize( Vector3( 0.0, -1.f, 0.0f ));
    dlight->AmbientIntensity = 0.01f;
    dlight->Diffuseintensity = 0.85f;

    auto crateMaterial = m_materialManager.CreateInstance<Material>(defaultShader, "crate_material");
    crateMaterial->Specular = {1.2f, 1.2f, 1.2f};
    crateMaterial->SetTextureDiffuse("../models/Crate/crate_tex3.jpg");



    m_rootNode->Initialize();
    
    Serialize("scenedata.scene");
    Deserialize("scenedata.scene");
}

Shader* Scene::GetShaderByName(std::string name)
{
//    auto res = find_if(begin(m_shaderManager.GetCache()), end(m_shaderManager.GetCache()),
//    [&](const std::unique_ptr<Shader>& c) {
//        return c->GetName() == name;
//    });
//    if (res != end(m_shaderManager.GetCache())) {
//        return res->get();
//    }
//    return nullptr;
    return m_shaderManager.GetResourceByName(name);
    
}

Material* Scene::GetMaterialByName(std::string name){
//    auto res = find_if(begin(m_materialManager.GetCache()), end(m_materialManager.GetCache()), 
//    [&](const std::unique_ptr<Material>& mat){
//       return mat->GetName() == name; 
//    });
//    if (res != end(m_materialManager.GetCache())){
//        return res->get();
//    }
//    return nullptr;
    return m_materialManager.GetResourceByName(name);
}

void Scene::AddGameObject(std::unique_ptr<GameObject> go)
{
    m_rootNode->AttachChild(move(go));
}

GameObject* Scene::GetGameObjectByName(string name)
{
    auto gp = m_rootNode->FindChildByName(name);
    if (gp) {
        return gp;
    }
    return nullptr;
}


static const int max_samples = 64;
float fpsSamples[max_samples];

float CalcFPS(float dt)
{
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

void Scene::Update(float dt)
{
    using namespace std::chrono;
    //Vector3 position = m_camera->GetPosition();
    //Vector3 direction = m_camera->GetViewDirection();
    // step the physics simulation
    high_resolution_clock::time_point physicsStart = high_resolution_clock::now();
    m_physicsWorld->Update(dt);
    high_resolution_clock::time_point physicsEnd = high_resolution_clock::now();
    auto physicsTime = physicsEnd - physicsStart;  
    double pt = physicsTime.count();
    this->PhysicsFrameTime = pt / 1000000;
    // updating game objects will collect their updated physics transforms
    // and perform any scripted activities.
    high_resolution_clock::time_point updateStart = high_resolution_clock::now();
    m_rootNode->Update(dt);
    high_resolution_clock::time_point updateEnd = high_resolution_clock::now();
    auto updateTime = updateEnd - updateStart;
    double ut = updateTime.count();
    this->UpdateFrameTime = ut / 1000000;
    // after update the scene is ready for rendering...
    high_resolution_clock::time_point renderStart = high_resolution_clock::now();
    m_renderer->RenderScene();
    high_resolution_clock::time_point renderEnd = high_resolution_clock::now();
    auto renderTime = renderEnd - renderStart;
    double rt = renderTime.count();
    this->RendererFrameTime = rt / 1000000;

#ifdef DEBUG_DRAW_PHYSICS
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
                m_physicsWorld->DrawPhysicsShape(dt, collider->GetCollisionShape(), { 1.f, 1.f, 1.f });
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
                    m_physicsWorld->DrawPhysicsShape(dt, collider->GetCollisionShape(),  { 1.f, 1.f, 1.f });
                }
            }
        }
    }
    //auto playerTrans = m_player->GetGhostWorldTransform();
    //playerTrans.UniformScale(1.0f);
    //auto mvp = m_projectionMatrix * viewMatrix * playerTrans.TransformMatrix();
    //m_physicsWorld->debugDrawer->mvpMatrix = mvp;
    //btTransform btt;
    //btt.setIdentity();
    //m_physicsWorld->DrawPhysicsShape(btt, m_player->GetCollisionShape(), {1.f, 1.f, 1.f});
#endif
//    string pos = "Position: " + position.ToString();
//    string dir = "Direction: " + direction.ToString();
//
//    string meshCount = "Meshes in Cache: " + std::to_string(m_meshManager.GetSize());
//    string renderedObjectCount = "Rendered Objects: " + std::to_string(m_renderer->GetMeshRendererCount());
//    \
//    m_fontRenderer->SetColor(1.0f, 0.0f, 0.0f);
//    m_fontRenderer->RenderText(pos, 25, 25);
//    //fontRenderer->SetColor(0.0f, 0.0f, 1.0f);
//    m_fontRenderer->RenderText(dir, 25, 52);
//    float fps = CalcFPS(dt);
//    m_fontRenderer->RenderText("FPS: " + to_string(fps), 25, 75);
//
//    m_fontRenderer->RenderText(meshCount, 25, 102);
//    m_fontRenderer->RenderText(renderedObjectCount, 25, 129);
}


void Scene::Awake()
{
    Initialize();
    m_rootNode->Awake();
    m_renderer->Initialize();
    for (auto& mesh : m_meshManager.GetCache()) {
        mesh->UnloadClientData();
    }

}

void Scene::Start()
{
    m_rootNode->Start();
}

void Scene::Destroy()
{
    m_rootNode->Destroy();
    m_meshManager.Clear();
    m_shaderManager.Clear();
    m_materialManager.Clear();
    m_textureManager.Clear();
}

GameObject* Scene::CreateEmptyGameObject(std::string name, GameObject* parent)
{
    auto goup = make_unique<GameObject>(name);
    auto ret = goup.get();
    parent->AttachChild(move(goup));
    return ret;
}

void Scene::DoLeftClick(double x, double y)
{
    printf("\n Mouse Click at: %f.2, %f.2", x, y);
}

void Scene::DestroyGameObject(std::unique_ptr<GameObject> object)
{
    GameObject* go = object.release();
    go->Destroy();
    delete go;
}

void Scene::DestroyGameObject(GameObject* go)
{
    m_renderer->UnregisterGameObject(go);
    auto parent = go->GetParent();
    auto up = parent->DetachChild(*go);
    up->Destroy();
}

} // namespace Jasper
