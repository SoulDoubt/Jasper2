#include "Scriptable.h"
#include "Scriptable.h"
#include "GameObject.h"
#include "PhysicsCollider.h"
#include "Scene.h"
#include "imgui.h"
#include "AssetSerializer.h"
#include <chrono>

namespace Jasper
{

void RotateAboutPointScript::Update(double dt)
{
    auto go = GetGameObject();
    if (go != nullptr) {
        go->GetLocalTransform().RotateAround(m_point, m_axis, dt * m_degreesPerSec);
    }
}

bool RotateAboutPointScript::ShowGui(){
    using namespace ImGui;
    
    Component::ShowGui();
    
    InputFloat3("Point", m_point.AsFloatPtr());
    InputFloat3("Axis", m_point.AsFloatPtr());
    InputFloat("Rate", &m_degreesPerSec);
    
    return false;
}

void RotateAboutPointScript::Serialize(std::ofstream & ofs) const
{
	using namespace AssetSerializer;
	ScriptComponent::Serialize(ofs);
	ofs.write(ConstCharPtr(m_point.AsFloatPtr()), sizeof(m_point));
	ofs.write(ConstCharPtr(m_axis.AsFloatPtr()), sizeof(m_axis));
	ofs.write(ConstCharPtr(&m_degreesPerSec), sizeof(m_degreesPerSec));
}

void RotateAboutPointScript::Deserialize(std::ifstream& ifs)
{
	using namespace AssetSerializer;
	ifs.read(CharPtr(&m_point), sizeof(m_point));
	ifs.read(CharPtr(&m_axis), sizeof(m_axis));
	ifs.read(CharPtr(&m_degreesPerSec), sizeof(m_degreesPerSec));
}

}
