#include "Scriptable.h"
#include "Scriptable.h"
#include "GameObject.h"
#include "PhysicsCollider.h"
#include "Scene.h"
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

}
