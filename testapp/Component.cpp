#include "Component.h"
#include "imgui.h"
#include "AssetSerializer.h"

namespace Jasper
{

Component::~Component() {
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
	const size_t namesize = m_name.size();
	ofs.write(ConstCharPtr(&namesize), sizeof(namesize));
	ofs.write(m_name.data(), sizeof(char) * namesize);
	const auto comptype = GetComponentType();
	ofs.write(ConstCharPtr(&comptype), sizeof(comptype));

}

}
