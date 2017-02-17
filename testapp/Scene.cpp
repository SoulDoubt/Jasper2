#include <memory>
#include <chrono>
#include "Scene.h"
#include "Quad.h"
#include "Cube.h"
#include "Material.h"
#include "Shader.h"
#include "MeshRenderer.h"
#include "SkyboxRenderer.h"
#include "Model.h"
#include "PhysicsCollider.h"
#include "Sphere.h"
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

    auto om = Matrix4();

    om.CreateOrthographicProjection(0.0f, m_windowWidth, m_windowHeight, 0.0f, 1.0f, -1.0f);
    //m_projectionMatrix = pm;
    m_orthoMatrix = om;
}

void Scene::Serialize(const std::string& filepath)
{
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

void Scene::SerializeGameObject(const GameObject* go, std::ofstream& ofs)
{
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

void Scene::DeserializeGameObject(std::ifstream & ifs, GameObject* parent)
{
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

        switch (typ) {
        case GameObjectType::Default: {
            gameObject = parent->AttachNewChild<GameObject>(name);
        }
        break;
        case GameObjectType::Camera: {
            Camera* c = parent->AttachNewChild<Camera>(Camera::CameraType::FLYING);
            gameObject = c;
            m_camera = c;
        }
        break;
        case GameObjectType::DirectionalLight: {
            DirectionalLight* dl = parent->AttachNewChild<DirectionalLight>(name);
            gameObject = dl;
        }
        break;
        case GameObjectType::PointLight: {

            PointLight* pl = parent->AttachNewChild<PointLight>(name);
            gameObject = pl;
        }
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
            //int x = 0;
            printf("Unhandled Component Type in deserialize..\n");
        }

    }
    size_t childSize;
    ifs.read(CharPtr(&childSize), sizeof(childSize));
    for (size_t i = 0; i < childSize; ++i) {
        DeserializeGameObject(ifs, gameObject);
    }


}

void Scene::DrawPickRay()
{
    auto viewMatrix = m_camera->GetViewMatrix();
    auto projMatrix = m_camera->GetProjectionMatrix();
    auto& dd = m_physicsWorld->debugDrawer;

    auto rmvp = projMatrix * viewMatrix;
    dd->SetMatrix(rmvp);
    dd->GetShader()->Bind();
    //Vector4 col = {1.f, 0.f, 0.f, 1.f};

    btVector3 st = m_pickRay.start.AsBtVector3();
    btVector3 ed = m_pickRay.end.AsBtVector3();
    dd->drawLine(st, ed, {1.f, 0.f, 0.f});
}

void Scene::DebugDrawPhysicsWorld()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    auto viewMatrix = m_camera->GetViewMatrix();
    auto projMatrix = m_camera->GetProjectionMatrix();
    auto& dd = m_physicsWorld->debugDrawer;
    dd->SetMatrix(projMatrix * viewMatrix);
    dd->Reset();
    m_physicsWorld->DebugDrawWorld();
    dd->Draw();

}

void Scene::Deserialize(const std::string& filepath)
{
    using namespace AssetSerializer;

    ifstream ifs;
    ifs.open(filepath, ios::in | ios::binary);

    // first are the shaders.
    size_t shadercount;
    ifs.read(CharPtr(&shadercount), sizeof(shadercount));
    for (uint i = 0; i < shadercount; ++i) {
        AssetSerializer::ConstructShader(ifs, this);
    }

    size_t materialcount;
    ifs.read(CharPtr(&materialcount), sizeof(materialcount));
    for (uint i = 0; i < materialcount; ++i) {
        AssetSerializer::ConstructMaterial(ifs, this);
    }

    size_t meshCount;
    ifs.read(CharPtr(&meshCount), sizeof(meshCount));
    for (uint i = 0; i < meshCount; ++i) {
        ConstructMesh(ifs, this);
    }

    DeserializeGameObject(ifs, nullptr);

    ifs.close();

}

void Scene::InitializeManual()
{
    m_rootNode = make_unique<GameObject>("Root_Node");
    m_rootNode->SetScene(this);

    float ar = (float) m_windowWidth / (float)m_windowHeight;
    m_player = m_rootNode->AttachNewChild<Camera>(Camera::CameraType::FLYING, 60.f, ar, 0.1f, 500.f);
    //m_player->AttachNewComponent<CapsuleCollider>("player_collider"s, Vector3(0.75f, 1.82f, 0.75f), m_physicsWorld.get());
    m_camera = m_player;
    m_camera->GetLocalTransform().Position = { 0.f, 0.f, 0.f };
    //70, aspectRatio, 0.1f, 1000.0f
//    m_camera->SetFov(70.f);
//    m_camera->SetAspectRatio();
//    m_camera->SetNearDistance(0.1f);
//    m_camera->SetFarDistance(1000.0f);
    //m_camera->SetPhysicsWorld(m_physicsWorld.get());

    m_player->GetLocalTransform().Position = { 10.0f, 5.0f, 0.0f };
    //auto sh = m_player->AttachNewComponent<ShooterScript>();
    //sh->Force = 50;

    // perform actual game object initialization

    //create the skybox
    m_skybox = make_unique<GameObject>("skybox");
    m_skybox->SetScene(this);
    auto skyboxMesh = m_meshManager.CreateInstance<Cube>("skybox_cube_mesh", Vector3(100.0f, 100.0f, 100.0f), true);
    skyboxMesh->SetCubemap(true); // we want to render the inside of the cube    
    m_shaderManager.CreateInstance<SkyboxShader>("skybox_shader");
    auto skyboxMaterial = m_materialManager.CreateInstance<Material>("skybox_material");
    string posx = "../textures/CloudyLightRays/CloudyLightRaysLeft2048.png"s;
    string negx = "../textures/CloudyLightRays/CloudyLightRaysRight2048.png"s;
    string posy = "../textures/CloudyLightRays/CloudyLightRaysUp2048.png"s;
    string negy = "../textures/CloudyLightRays/CloudyLightRaysDown2048.png"s;
    string posz = "../textures/CloudyLightRays/CloudyLightRaysFront2048.png"s;
    string negz = "../textures/CloudyLightRays/CloudyLightRaysBack2048.png"s;
    skyboxMaterial->SetCubemapTextures(posx, negx, posy, negy, posz, negz);
    auto sbr = m_skybox->AttachNewComponent<SkyboxRenderer>("skybox_renderer", skyboxMesh, skyboxMaterial); 
    m_renderer->SetSkyboxRenderer(sbr);    
    skyboxMesh->SetMaterial(skyboxMaterial);
    
    //skyboxMesh->Initialize();
    // create the Lit Shader Instance to render most objects
    //auto defaultShader = m_shaderManager.CreateInstance<LitShader>();
    m_shaderManager.CreateInstance<GeometryPassShader>();
    m_shaderManager.CreateInstance<DirectionalLightPassShader>();
    m_shaderManager.CreateInstance<PointLightPassShader>();
    m_shaderManager.CreateInstance<DeferredStencilPassShader>();
    m_shaderManager.CreateInstance<LitShader>();
    m_shaderManager.CreateInstance<AnimatedLitShader>();
    
    Material* icosMat = m_materialManager.CreateInstance<Material>("icosmat");
    icosMat->SetTextureDiffuse("../textures/icos.png");

    m_fontRenderer = make_unique<FontRenderer>();
    m_fontRenderer->Initialize();
    m_fontRenderer->SetOrthoMatrix(m_orthoMatrix);

    Material* m1 = m_materialManager.CreateInstance<Material>("wall_material");
    //m1->SetTextureDiffuse("../textures/176.JPG");
    //m1->SetTextureNormalMap("../textures/176_norm.JPG");
    m1->Diffuse = { 0.85f, 0.34f, 0.05f };
    m1->Ambient = { 0.25f, 0.25f, 0.25f };
    m1->Specular = { 0.65f, 0.65f, 0.65f };
    m1->Shine = 32;
    
    m1->Flags = Material::MATERIAL_FLAGS::USE_MATERIAL_COLOR;

    //    std::ofstream serializer;
    //    serializer.open("scenedata.scene", ios::binary | ios::out);
    //
    //    AssetSerializer::SerializeMaterial(serializer, m1);
    //    serializer.close();
    //
    //    std::ifstream reader;
    //    reader.open("scenedata.scene", ios::in | ios::binary);
    //    AssetSerializer::this);ConstructMaterial(reader, 
    // Floor
    auto floor = m_rootNode->AttachNewChild<GameObject>("floor");
    auto quadMesh = m_meshManager.CreateInstance<Quad>("floor_quad", Vector2(100.0f, 100.0f), 25, 25, Quad::AxisAlignment::XZ);
    Material* floorMaterial = m_materialManager.CreateInstance<Material>("floor_material");
    floorMaterial->SetTextureDiffuse("../textures/186.JPG");
    floorMaterial->SetTextureNormalMap("../textures/186_norm.JPG");
    floorMaterial->Specular = { 0.8f, .9f, .9f };
    floorMaterial->Shine = 64;
    floor->AttachNewComponent<MeshRenderer>("quad_renderer", quadMesh, floorMaterial);
    floor->GetLocalTransform().Translate(Vector3(0.0f, -1.f, 0.0f));    
    floorMaterial->Ambient = { 0.0f, 0.0f, 0.0f };
    
    quadMesh->SetMaterial(floorMaterial);
    //quadMesh->Initialize();
    auto floorP = floor->AttachNewComponent<PlaneCollider>("floor_collider", Vector3(0.0, 1.0, 0.0), 0.0, m_physicsWorld.get());
    floorP->Friction = 0.9f;    

    auto basic = m_shaderManager.GetResourceByName("basic_shader"s);
    auto red = m_materialManager.CreateInstance<Material>("red_material"s);



    // wall
    auto wall = m_rootNode->AttachNewChild<GameObject>("wall_0");
    auto wallMesh = m_meshManager.CreateInstance<Cube>("wall_mesh", Vector3(50.f, 10.0f, 3.0f), 20.f, 4.f);    
    wall->GetLocalTransform().Translate(0.0f, 25.0f, -20.0f);    
    wall->AttachNewComponent<MeshRenderer>("wall_renderer", wallMesh, m1);
    wallMesh->SetMaterial(m1);
    //wallMesh->Initialize();
    auto wallCollider = wall->AttachNewComponent<BoxCollider>("wall_0_collider", wallMesh, m_physicsWorld.get());
    wallCollider->Mass = 100.0f;    

    // launcher
    auto launcher = m_rootNode->AttachNewChild<GameObject>("Launcher"s);
    launcher->GetLocalTransform().Translate(0.f, 1.f, 10.f);
    //launcher->AttachNewComponent<LauncherScript>("Launcher_script");

    // secondary launcher
    auto launcher2 = launcher->AttachNewChild<GameObject>("Launcher2"s);
    launcher2->GetLocalTransform().Translate(10.f, 2.f, 5.f);
   

    
    //ml->LoadModel("../models/Mathias/Mathias.obj"s, "Cyborg");
    
    auto ss = m_meshManager.CreateInstance<Sphere>("base_sphere", 1.0f);  
    ss->SetMaterial(m1);
    auto icos = m_rootNode->AttachNewChild<GameObject>("icos");
    icos->GetLocalTransform().Position = {0.f, 1.f, 0.f };
    icos->AttachNewComponent<MeshRenderer>("icos_renderer", ss, icosMat);
    auto sc = icos->AttachNewComponent<SphereCollider>("icos_collider", ss, m_physicsWorld.get());
    sc->Mass = 50.f;
    sc->Friction = 0.75f;
    sc->Restitution = 0.86f;
    
    ModelLoader ml(this);
    //ml.LoadModel("../models/nanosuit/nanosuit.obj"s, "teapot");    
    
    //auto& ninja = m_rootNode->AttachChild(ml->CreateModelInstance("teapot_model", "pot1", true, false));
    //ninja.GetLocalTransform().Position = {1.f, 1.f, 0.f};
    //ninja.GetLocalTransform().Scale = {0.05f, 0.05f, 0.05f};
        
    //auto& pot2 = m_rootNode->AttachChild(ml->CreateModelInstance("teapot_model1", "pot2", true, false));
    //pot2.GetLocalTransform().Position = {-1.f, 1.f, 0.f};
    //pot2.GetLocalTransform().Scale = {0.05f, 0.05f, 0.05f};

        
    auto cylMesh = m_meshManager.CreateInstance<Cylinder>("base_cyl", 2.f, 1.f);    
    cylMesh->SetMaterial(m1);    
    auto cyl = m_rootNode->AttachNewChild<GameObject>("Cyl");
    auto cylrend = cyl->AttachNewComponent<MeshRenderer>("cyl_renderer", cylMesh, m1);
    auto cylcol = cyl->AttachNewComponent<CylinderCollider>("cyl_collider", cylMesh, m_physicsWorld.get());    
    cylrend->ToggleWireframe(true);
    cylcol->Mass = 20.f;
    
//    float cx = 0;
//    float cz = 10;
//    float radius = 5;
//    for (int i = 0; i < 12; ++i) {
//        float a = DEG_TO_RAD(i * 30);
//        float ax = cx + radius * sinf(a);
//        float az = cz + radius * cosf(a);
//        auto& model = m_rootNode->AttachChild(ml.CreateModelInstance("teapot"s + to_string(i), "teapot", true, false));
//        model.GetLocalTransform().Position = { ax, 1.2f, az };
//        model.GetLocalTransform().UniformScale(0.4f);
//        //model.GetLocalTransform().Scale = {0.1, 0.1, 0.1};
//    }
    
     ml.LoadModel("../models/testman/test_B3D.b3d"s, "lara");
     //ml.LoadModel("../models/testman/test_Collada_DAE.DAE"s, "lara");
     auto& lara = m_rootNode->AttachChild(ml.CreateModelInstance("lara", "lara", true, false));
     lara.GetLocalTransform().Position = { 15.f, 1.2f, 4.f};
     lara.GetLocalTransform().Rotate({1.4, 0.f, 0.f}, -90.f);
     //lara.GetLocalTransform().UniformScale(0.1f);
    
//    auto debris = m_rootNode->AttachNewChild<GameObject>("debris"s);
//    auto debrismodel = debris->AttachNewComponent<Model>("debris"s, "../models/rocks/01/rock_01.obj"s, defaultShader, true, m_physicsWorld.get());
//    debrismodel->ColliderType = PHYSICS_COLLIDER_TYPE::None;
//    debrismodel->Setup(this);
//    debris->GetLocalTransform().Position = {0.f, 3.f, -20.f};
//    debris->GetLocalTransform().Scale = {0.02, 0.02, 0.02};
//    int x = 0;

//    auto teapot = m_rootNode->AttachNewChild<GforameObject>("teapot"s);
//    auto teapot_model = model->AttachNewComponent<Model>("teapot"s, "../models/teapot/teapot.obj"s, defaultShader, true, m_physicsWorld.get());
//    teapot_model->ColliderType = PHYSICS_COLLIDER_TYPE::Box;
//    teapot->GetLocalTransform().Scale = { 0.005f, 0.005f, 0.005f };
//    teapot->GetLocalTransform().Position = { 4.f, 4.f, -6.5f };
//    //model->GetLocalTransform().Rotate( { 1.f, 0.f, 0.f }, -90.f);
//    teapot_model->Setup(this);
//
//    auto teapot_collider = teapot->GetComponentByType<PhysicsCollider>();
//    if (teapot_collider) {
//        teapot_collider->GetCollisionShape()->setLocalScaling( { 0.005f, 0.005f, 0.005f });
//        teapot_collider->Mass = 2.f;
//    }
//    ////model->getlocaltransform().uniformscale(0.02f);
//    //if (collider) {
//    //	//collider->GetCollisionShape()->setLocalScaling({ 0.015f, 0.015f, 0.015f });
//    //}
//    auto teapot_unused = model->DetachComponent(*teapot_model);
    //model->AttachNewComponent<RotateInPlaceScript>("rotate_in_place_script", Vector3(0.f, 1.f, 0.f), 90);

    auto light0 = m_rootNode->AttachNewChild<PointLight>("p_light"s);
    light0->GetLocalTransform().Translate( { 0.0f, 10.f, 15.0f });
    light0->Attenuation = 0.f;
    light0->Color = { 1.f, 1.f, 1.f };
    light0->AmbientIntensity = 0.3f;
    light0->DiffuseIntensity = 0.85f;
    light0->Radius = 20.0f;
    auto lightMesh = m_meshManager.CreateInstance<Cube>("point_light_mesh"s, Vector3(0.1f, 0.1f, 0.1f));
    auto lightMaterial = m_materialManager.CreateInstance<Material>("point_light_material"s);
    lightMaterial->SetTextureDiffuse("../textures/white.jpg"s);
    lightMesh->SetMaterial(lightMaterial);
    //lightMesh->Initialize();    
    light0->AttachNewComponent<MeshRenderer>("point_light_renderer"s, lightMesh, lightMaterial);
    //light0->AttachNewComponent<RotateAboutPointScript>("Rotate_Light_Script"s, Vector3(0.f, 7.5f, 0.f), Vector3(0.f, 1.f, 0.f), 45);
    m_pointLights.push_back(light0);
    //
    
    auto light1 = m_rootNode->AttachNewChild<PointLight>("light1");
    light1->GetLocalTransform().Position = {5.f, 10.f, 5.f};
    light1->Radius = 15.f;
    m_pointLights.push_back(light1);
    
    auto light2 = m_rootNode->AttachNewChild<PointLight>("light2");
    light2->GetLocalTransform().Position = {-5.f, 10.f, 5.f};
    light2->Radius = 15.f;
    m_pointLights.push_back(light2);
    
    auto light3 = m_rootNode->AttachNewChild<PointLight>("light3");
    light3->GetLocalTransform().Position = {0.f, 10.f, 10.f};
    light3->Radius = 15.f;
    m_pointLights.push_back(light3);

    auto dlight = m_rootNode->AttachNewChild<DirectionalLight>("d_light"s);
    dlight->Direction = Normalize(Vector3(-0.5, -1.f,-0.5f));
    dlight->AmbientIntensity = 0.005f;
    dlight->Diffuseintensity = 0.2f;
    m_directionalLight = dlight;
}



void Scene::Initialize()
{
    //printf("GameObject: %d\n", sizeof(GameObject));
    //printf("std::vector<GameObject>: %d\n", sizeof(std::vector<GameObject>));
    //printf("std::vector<Component>: %d\n", sizeof(std::vector<Component>));
    srand(time(nullptr));
    m_physicsWorld = make_unique<PhysicsWorld>(this);
    m_physicsWorld->Initialize();

    m_renderer = make_unique<Renderer>(this);

    Resize(m_windowWidth, m_windowHeight);

    auto debugShader = m_shaderManager.CreateInstance<BasicShader>();
    debugShader->SetColor(Vector4(0.f, 1.f, 0.f, 1.0f));
    m_physicsWorld->debugDrawer->SetShader(debugShader);
    m_physicsWorld->debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb);
    m_physicsWorld->debugDrawer->Initialize();

    InitializeManual();
    //Deserialize("../scenes/scenedata.scene");
    for (const auto& m : m_meshManager.GetCache()){
        m->Initialize();
    }
    m_renderer->Initialize();    
    m_rootNode->Initialize();
    //Serialize("../scenes/scenedata.scene");
}

Shader* Scene::GetShaderByName(const std::string& name)
{
    return m_shaderManager.GetResourceByName(name);
}

Material* Scene::GetMaterialByName(const std::string& name)
{

    return m_materialManager.GetResourceByName(name);
}

void Scene::AddGameObject(std::unique_ptr<GameObject> go)
{
    m_rootNode->AttachChild(move(go));
}

GameObject* Scene::GetGameObjectByName(const string & name)
{
    const auto gp = m_rootNode->FindChildByName(name);
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
    auto updateMili = duration_cast<nanoseconds>(updateEnd - updateStart);
    double ut = (double)updateMili.count() / 1000000.0;
    this->UpdateFrameTime = ut;
    // after update the scene is ready for rendering...
    m_rootNode->LateUpdate();
    const auto renderStart = high_resolution_clock::now();
    //m_renderer->RenderScene();
    m_renderer->RenderScene();        
    m_renderer->RenderSkybox();

    const auto renderEnd = high_resolution_clock::now();
    const auto renderTime = duration_cast<nanoseconds>(renderEnd - renderStart);
    long long rt = renderTime.count();
    this->RendererFrameTime = (double)rt / 1000000.0;
    
    if (debug_draw_physics) {
        DebugDrawPhysicsWorld();
    }

}

GameObject* Scene::MousePickGameObject(int mouse_x, int mouse_y, Vector3& hit_point, Vector3& hit_normal)
{
    float winx = (2.0f * mouse_x) / m_windowWidth - 1.0f;
    float winy = 1.0f - (2.0f * mouse_y) / m_windowHeight;

    Vector4 ray_start_ndc = {winx, winy, -1.f, 1.f};
    Vector4 ray_end_ndc = {winx, winy, 0.f, 1.f};

    auto inverseView = GetCamera().GetViewMatrix().Inverted();
    auto inverseProj = GetCamera().GetProjectionMatrix().Inverted();

    auto ray_start_camera = inverseProj * ray_start_ndc;
    ray_start_camera /= ray_start_camera.w;
    auto ray_end_camera = inverseProj * ray_end_ndc;
    ray_end_camera /= ray_end_camera.w;

    auto ray_start_world = inverseView * ray_start_camera;
    ray_start_world /= ray_start_world.w;

    auto ray_end_world = inverseView * ray_end_camera;
    ray_end_world /= ray_end_world.w;

    auto ray_direction = ray_end_world.AsVector3() - ray_start_world.AsVector3();
    auto ray_origin = ray_start_world.AsVector3();
    ray_direction = Normalize(ray_direction);

    auto from = ray_origin.AsBtVector3();
    auto to = (ray_origin + ray_direction * 1000.f).AsBtVector3();

    m_pickRay.start = Vector3(from);
    m_pickRay.end = Vector3(to);

    btCollisionWorld::ClosestRayResultCallback cb(from, to);
    m_physicsWorld->GetBtWorld()->rayTest(from, to, cb);
    if (cb.hasHit()) {
        hit_point = Vector3(cb.m_hitPointWorld);
        hit_normal = Vector3(cb.m_hitNormalWorld);
        auto object = cb.m_collisionObject;
        auto hit_object = object->getUserPointer();
        auto go = static_cast<GameObject*>(hit_object);
        if (go) {
            return go;
        }
    }
    return nullptr;
}

void Scene::MouseSelectGameObject(int x, int y)
{
    Vector3 hit_point, hit_normal;
    GameObject* go = MousePickGameObject(x, y, hit_point, hit_normal);
    if (go) {
        if (m_selected_game_object != go && m_selected_game_object != nullptr) {
            if (auto cc = m_selected_game_object->GetComponentByType<PhysicsCollider>()) {
                cc->ColliderFlags ^= PhysicsCollider::DRAW_COLLISION_SHAPE;
                cc->SetDebugColor( {1.f, 0.f, 1.f, 0.9f});
            }
        }
        if (auto col = go->GetComponentByType<PhysicsCollider>()) {
            col->SetDebugColor( {0.f, 1.f, 0.f, 1.f});
            col->ColliderFlags |= PhysicsCollider::DRAW_COLLISION_SHAPE;
        }
        m_selected_game_object = go;
    }
}

void Scene::MouseMoveSelectedGameObject(int xrel, int yrel)
{

    float x = (float)xrel / 40;
    float y = (float)yrel / 40;
    if (m_selected_game_object) {
        if (auto collider = m_selected_game_object->GetComponentByType<PhysicsCollider>()) {
            if (collider->IsEnabled()) {
                collider->ToggleEnabled(false);
            }
        }
        m_selected_game_object->GetLocalTransform().Translate(x, -y, 0.f);
    }
}

void Scene::ShootMouse(int x, int y)
{
    Vector3 hit_point, hit_normal;
    GameObject* go = MousePickGameObject(x, y, hit_point, hit_normal);
    if (go) {
        if (auto cc = go->GetComponentByType<PhysicsCollider>()) {
            btRigidBody* rb = cc->GetRigidBody();
            //Transform t = cc->GetCurrentWorldTransform();
            btVector3 offset = hit_point.AsBtVector3() - rb->getWorldTransform().getOrigin();
            rb->activate();
            rb->applyImpulse(-(hit_normal.AsBtVector3() * 100.f), offset);

        }
    }
}

void Scene::Awake()
{

    m_rootNode->Awake();

//    for (auto& mesh : m_meshManager.GetCache()) {
//        //mesh->UnloadClientData();
//    }
    


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
    //if (!m_)
    //auto sh = m_player->GetComponentByType<ShooterScript>();
    //sh->ShootRay();
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
