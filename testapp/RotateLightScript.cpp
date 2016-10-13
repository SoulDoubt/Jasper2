#include "Scriptable.h"
#include "Scriptable.h"
#include "GameObject.h"
#include "PhysicsCollider.h"
#include "Scene.h"
#include <chrono>

namespace Jasper{

void RotateLightScript::Update(float dt){    
    auto point_light = GetGameObject();
	if (point_light != nullptr) {
		point_light->GetLocalTransform().RotateAround({ 0.f, 10.f, 0.f }, { 0.f, 1.f, 0.f }, 1.0f);
	}
}

}
