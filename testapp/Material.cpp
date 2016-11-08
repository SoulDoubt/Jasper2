#include "Material.h"
#include <string>
#include "imgui.h"

namespace Jasper
{

//	Material::Material(Shader* shader)
//	{
//		m_shader = shader;
//		m_shader->SetBoundMaterial(this);
//		// material defaults
//		Ambient = { 0.5f, 0.5f, 0.5f };
//		Diffuse = { 0.85f, 0.85f, 0.85f };
//		Specular = { 0.25f, 0.25f, 0.26f };
//		Shine = 32.0f;
//        m_name = "Unamed Material";
//	}

bool Material::ShowGui()
{
    using namespace std;
    string na = GetName();
    static vector<char> name_buffer;
    name_buffer.assign(begin(na), end(na));
    name_buffer.push_back('\0');
    ImGui::Separator();
    ImGui::Text("Edit Material");
    ImGui::InputText("Name: ", name_buffer.data(), name_buffer.size());
    ImGui::InputFloat3("Diffuse", Diffuse.AsFloatPtr());
    ImGui::InputFloat3("Ambient", Ambient.AsFloatPtr());
    ImGui::InputFloat3("Speculat", Specular.AsFloatPtr());
    ImGui::InputFloat("Shine", &Shine);
    SetName(string(name_buffer.data()));
    return false;
}

Material::Material(Shader* shader, const std::string& name)
{
    m_shader = shader;
    m_shader->SetBoundMaterial(this);
    // material defaults
    Ambient = { 0.5f, 0.5f, 0.5f };
    Diffuse = { 0.85f, 0.85f, 0.85f };
    Specular = { 0.25f, 0.25f, 0.26f };
    Shine = 32.0f;
    m_name = name;
}


Material::~Material()
{
}
}
