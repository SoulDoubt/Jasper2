#include "Scriptable.h"
#include "Scriptable.h"
#include "GameObject.h"
#include "PhysicsCollider.h"
#include "Scene.h"
#include "imgui.h"
#include <chrono>

namespace Jasper
{

void RotateAboutPointScript::Update(float dt)
{
    auto go = GetGameObject();
    if (go != nullptr) {
        go->GetLocalTransform().RotateAround(m_point, m_axis, dt * m_degreesPerSec);
    }
}

bool RotateAboutPointScript::ShowGui(){
    using namespace ImGui;
    
    InputFloat3("Point", m_point.AsFloatPtr());
    InputFloat3("Axis", m_point.AsFloatPtr());
    InputFloat("Rate", &m_degreesPerSec);
    
    return false;
}

}
