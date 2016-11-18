#include "Component.h"
#include "imgui.h"

namespace Jasper
{

Component::~Component()
{
}

void Component::Initialize()
{

}

void Component::Destroy()
{
}

void Component::Awake()
{

}

void Component::Start()
{
}

void Component::FixedUpdate()
{
}

void Component::Update(float dt)
{

}

void Component::LateUpdate()
{
}

void Component::ToggleEnabled(bool enabled)
{
    m_isEnabled = enabled;
}

bool Component::ShowGui()
{
    bool enabled = m_isEnabled;
    if (ImGui::Checkbox("Enabled", &enabled)) {
        ToggleEnabled(enabled);
    }
    return false;
}

}
