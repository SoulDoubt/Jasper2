#include "Material.h"
#include <string>
#include "imgui.h"
#include "AssetSerializer.h"

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
    m_shaderName = shader->GetName();
    // material defaults
    Ambient = { 0.5f, 0.5f, 0.5f };
    Diffuse = { 0.85f, 0.85f, 0.85f };
    Specular = { 0.25f, 0.25f, 0.26f };
    Shine = 32.0f;
    m_name = name;
}

void Material::Serialize(std::ofstream& ofs){
    using namespace AssetSerializer;
    using namespace std;
    size_t nameSize = m_name.size();
    ofs.write(CharPtr(&nameSize), sizeof(nameSize));
    ofs.write(m_name.c_str(), m_name.size() * sizeof(char));
    ofs.write(CharPtr(Ambient.AsFloatPtr()),  sizeof(Vector3));
    ofs.write(CharPtr(Diffuse.AsFloatPtr()),  sizeof(Vector3));
    ofs.write(CharPtr(Specular.AsFloatPtr()), sizeof(Vector3));
    ofs.write(CharPtr(&Shine), sizeof(float));
    
    size_t shaderNameLength = m_shaderName.size();
    ofs.write(CharPtr(&shaderNameLength), sizeof(shaderNameLength));
    ofs.write(m_shaderName.c_str(), sizeof(char) * shaderNameLength);
    
    size_t diffuseLength = m_diffuseTextureFileName.size();
    ofs.write(CharPtr(&diffuseLength), sizeof(diffuseLength));
    ofs.write(m_diffuseTextureFileName.c_str(), sizeof(char) * diffuseLength);
    
    ofs.write(CharPtr(&diffuseLength), sizeof(diffuseLength));
    ofs.write(m_diffuseTextureFileName.c_str(), sizeof(char) * diffuseLength);
    
    bool hasNormalMap = m_normalMap != nullptr;
    bool hasSpecularMap = m_specularMap != nullptr;
    
    ofs.write(CharPtr(&hasNormalMap), sizeof(hasNormalMap));
    if (hasNormalMap){
        string normalMapFile = m_normalMap->GetFilename();
        size_t norlen = normalMapFile.size();
        ofs.write(CharPtr(&norlen), sizeof(norlen));
        ofs.write(normalMapFile.c_str(), sizeof(char) * norlen);
    }
    
    ofs.write(CharPtr(&hasSpecularMap), sizeof(hasSpecularMap));
    if (hasSpecularMap){
        string specMapFile = m_specularMap->GetFilename();
        size_t speclen = specMapFile.size();
        ofs.write(CharPtr(&speclen), sizeof(speclen));
        ofs.write(specMapFile.c_str(), sizeof(char) * speclen);
    }
}


Material::~Material()
{
}
}
