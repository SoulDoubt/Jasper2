#include "Scriptable.h"
#include "GameObject.h"
#include "PhysicsCollider.h"
#include "Scene.h"
#include <chrono>

using namespace std::chrono;

namespace Jasper
{

void DefaultScript::Update(float dt)
{
//        auto go = GetGameObject();
//        auto current = high_resolution_clock::now();
//        auto awakeTime = this->GetGameObject()->TimeAwakened;
//
//        auto elapsed = duration_cast<milliseconds>(current - awakeTime).count();
//
//        if (elapsed > 12000){
//            printf("Deleting Teapot.\n");
//            //go->Destroy();
//            go->GetScene()->DestroyGameObject(go);
//        }

//        if (elapsed > 8000){
//            force_start_apply = current;
//            printf("Moving teapot\n");
//            auto transform = go->GetLocalTransform();
//            transform.Position = Vector3(0.f, 10.f, 0.f);
//            go->TimeAwakened = current;
//            auto collider = go->GetComponentByType<PhysicsCollider>();
//            //collider->SetWorldTransform(transform.GetBtTransform());
//            btRigidBody* body = collider->GetRigidBody();
//            body->applyForce({0.f, 2000.0f, 0.0f}, {0.0f, 0.0f, 0.0f});
//            //collider->SetWorldTransform(transform.GetBtTransform());
//
//        }
}

void DestroyScript::Update(float dt)
{
    auto go = GetGameObject();
    auto current = std::chrono::high_resolution_clock::now();
    auto awakeTime = this->GetGameObject()->TimeAwakened;

    auto elapsed = std::chrono::duration_cast<milliseconds>(current - awakeTime).count();

    if (elapsed > 60000) {
        go->GetScene()->DestroyGameObject(go);
    }
}

}
