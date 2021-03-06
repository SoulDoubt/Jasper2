#include "Component.h"
#include "imgui.h"
#include "AssetSerializer.h"

namespace Jasper
{

Component::~Component() {
	Destroy();
}

void Component::Initialize() {

}

void Component::Destroy() {
}

void Component::Awake() {
	
}

void Component::Start() {
}

void Component::FixedUpdate() {
}

void Component::Update(double dt) {

}

void Component::LateUpdate() {
}

void Component::ToggleEnabled(bool enabled) {
	m_isEnabled = enabled;
}

bool Component::ShowGui() {
	bool enabled = m_isEnabled;
	if (ImGui::Checkbox("Enabled", &enabled)) {
		ToggleEnabled(enabled);
	}
	return false;
}

void Component::Serialize(std::ofstream& ofs) const {
	using namespace AssetSerializer;
	WriteString(ofs, this->GetName());
	const auto comptype = GetComponentType();
	ofs.write(ConstCharPtr(&comptype), sizeof(comptype));

}

}
