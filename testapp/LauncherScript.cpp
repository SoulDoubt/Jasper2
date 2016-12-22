#include "Scriptable.h"
#include "GameObject.h"
#include "PhysicsCollider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "Scene.h"
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "Sphere.h"
#include "Cube.h"
#include <chrono>

namespace Jasper
{

using namespace std;
using namespace std::chrono;

bool LauncherScript::ShowGui(){
    return false;
}

void LauncherScript::Serialize(std::ofstream & ofs) const
{
	ScriptComponent::Serialize(ofs);

}

void LauncherScript::Update(float dt)
{
    auto gameObject = GetGameObject();
    auto awakeTime = gameObject->TimeAwakened;
    auto current = high_resolution_clock::now();


    if (launch_count == 0) {
        auto elapsed = duration_cast<milliseconds>(current - awakeTime).count();
        if (elapsed >= launch_interval) {
            LaunchCube();
            time_last_launch = current;
        }
    } else {
        auto elapsed = duration_cast<milliseconds>(current - time_last_launch).count();
        if (elapsed >= launch_interval) {
            LaunchCube();
            time_last_launch = current;
        }
    }
}



void LauncherScript::LaunchTeapot()
{
    auto gameObject = GetGameObject();
    //printf("%s has %d children", gameObject->GetName().c_str(), gameObject->Children().size());

    auto scene = gameObject->GetScene();
    auto pw = scene->GetPhysicsWorld();
    auto shader = scene->GetShaderByName("Lit_Shader");

    const auto transform = gameObject->GetWorldTransform();
    const auto rootNode = gameObject->GetScene()->GetRootNode();

    GameObject* model = rootNode->AttachNewChild<GameObject>("teapot_model");
    model->AttachNewComponent<Model>("teapot", "../models/teapot/teapot.obj", shader, true, pw);

    model->SetLocalTransform(transform);
    model->GetLocalTransform().UniformScale(0.2f);
    auto collider = model->GetComponentByType<PhysicsCollider>();
    collider->Mass = 1.0f;
    collider->Restitution = 1.2f;
    model->AttachNewComponent<DestroyScript>("Destroy_Script");
    model->Awake();

    scene->GetRenderer()->RegisterGameObject(model);

    const btVector3 offset = {0.f, 0.f, 0.f};
    collider->GetRigidBody()->applyForce(Force.AsBtVector3(), offset);

    launch_count++;
}

void LauncherScript::LaunchSphere()
{
    this->Force = {0.f, 800.f, 0.f};
    auto gameObject = GetGameObject();
    //printf("%s has %d children", gameObject->GetName().c_str(), gameObject->Children().size());

    auto scene = gameObject->GetScene();
    auto pw = scene->GetPhysicsWorld();
    auto transform = Transform();
    transform.SetIdentity();
    auto rootNode = gameObject->GetScene()->GetRootNode();
    auto mat = scene->GetMaterialCache().GetResourceByName("red_material");
    auto sphere = rootNode->AttachNewChild<GameObject>("Sphere");
    Mesh* mesh = nullptr;
    if ((mesh = scene->GetMeshCache().GetResourceByName("launch_sphere")) == nullptr) {
        // need to create the mesh instance
        mesh = scene->CreateMesh<Sphere>("sphere_mesh", 0.5f);
        mesh->SetName("launch_sphere");
    }

    sphere->AttachNewComponent<MeshRenderer>("launched_sphere_renderer", mesh, mat);
    sphere->SetLocalTransform(transform);

    auto collider = sphere->AttachNewComponent<SphereCollider>("Sphere_collider", mesh->GetHalfExtents(), pw);
    collider->Mass = 1.0f;
    collider->Restitution = 0.56f;
    sphere->AttachNewComponent<DestroyScript>("Destroy_Script");
    sphere->Awake();
    sphere->Children().clear();
    scene->GetRenderer()->RegisterGameObject(sphere);
    Vector3 center_offset = {0.f, 0.f, 0.f};
    collider->GetRigidBody()->applyForce(Force.AsBtVector3(), center_offset.AsBtVector3());
    launch_count++;
}

// launch a cube
void LauncherScript::LaunchCube()
{
    this->Force = {0.f, 10000.f, 0.f};
    auto gameObject = GetGameObject();
    //printf("%s has %d children", gameObject->GetName().c_str(), gameObject->Children().size());

    auto scene = gameObject->GetScene();
    auto pw = scene->GetPhysicsWorld();
    auto transform = gameObject->GetWorldTransform();
    //auto transform = Transform();
    //transform.SetIdentity();

    auto rootNode = gameObject->GetScene()->GetRootNode();
    auto mat = scene->GetMaterialCache().GetResourceByName("crate_material");
    auto sphere = rootNode->AttachNewChild<GameObject>("cube");
    Mesh* mesh = nullptr;
    if ((mesh = scene->GetMeshCache().GetResourceByName("launcher_cube_mesh")) == nullptr) {
        // need to create the mesh instance
        mesh = scene->CreateMesh<Cube>("launcher_cube_mesh", Vector3(0.25f, 0.25f, 0.25f));
        //mesh->CalculateFaceNormals();
    }

    sphere->AttachNewComponent<MeshRenderer>("launched_cube_renderer", mesh, mat);
    sphere->SetLocalTransform(transform);

    auto collider = sphere->AttachNewComponent<BoxCollider>("cube_collider", mesh->GetHalfExtents(), pw);
    collider->Mass = 10.0f;
    collider->Restitution = 0.96f;
    sphere->AttachNewComponent<DestroyScript>("Destroy_Script");
    sphere->Awake();
    sphere->Children().clear();
    scene->GetRenderer()->RegisterGameObject(sphere);
    Vector3 center_offset = {0.f, 0.f, 0.f};
    collider->GetRigidBody()->applyForce(Force.AsBtVector3(), center_offset.AsBtVector3());
    launch_count++;
}



}
