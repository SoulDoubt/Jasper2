#include "Material.h"
#include <string>
#include "imgui.h"
#include "AssetSerializer.h"

namespace Jasper
{


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

void Material::Serialize(std::ofstream& ofs) const {
    using namespace AssetSerializer;
    using namespace std;
    const size_t nameSize = m_name.size();
    ofs.write(ConstCharPtr(&nameSize), sizeof(nameSize));
    ofs.write(m_name.c_str(), m_name.size() * sizeof(char));
    ofs.write(ConstCharPtr(Ambient.AsFloatPtr()),  sizeof(Vector3));
    ofs.write(ConstCharPtr(Diffuse.AsFloatPtr()),  sizeof(Vector3));
    ofs.write(ConstCharPtr(Specular.AsFloatPtr()), sizeof(Vector3));
    ofs.write(ConstCharPtr(&Shine), sizeof(float));
    
    const size_t shaderNameLength = m_shaderName.size();
    ofs.write(ConstCharPtr(&shaderNameLength), sizeof(shaderNameLength));
    ofs.write(m_shaderName.c_str(), sizeof(char) * shaderNameLength);
    
    const size_t diffuseLength = m_diffuseTextureFileName.size();
    ofs.write(ConstCharPtr(&diffuseLength), sizeof(diffuseLength));
    ofs.write(m_diffuseTextureFileName.c_str(), sizeof(char) * diffuseLength);
    
    ofs.write(ConstCharPtr(&diffuseLength), sizeof(diffuseLength));
    ofs.write(m_diffuseTextureFileName.c_str(), sizeof(char) * diffuseLength);
    
    const bool hasNormalMap = m_normalMap != nullptr;
    const bool hasSpecularMap = m_specularMap != nullptr;
    
    ofs.write(ConstCharPtr(&hasNormalMap), sizeof(hasNormalMap));
    if (hasNormalMap){
        const string normalMapFile = m_normalMap->GetFilename();
        const size_t norlen = normalMapFile.size();
        ofs.write(ConstCharPtr(&norlen), sizeof(norlen));
        ofs.write(normalMapFile.c_str(), sizeof(char) * norlen);
    }
    
    ofs.write(ConstCharPtr(&hasSpecularMap), sizeof(hasSpecularMap));
    if (hasSpecularMap){
        const string specMapFile = m_specularMap->GetFilename();
        const size_t speclen = specMapFile.size();
        ofs.write(ConstCharPtr(&speclen), sizeof(speclen));
        ofs.write(specMapFile.c_str(), sizeof(char) * speclen);
    }
	
	if (m_cubemap) {
		bool isCubeMap = true;
		ofs.write(CharPtr(&isCubeMap), sizeof(isCubeMap));
		size_t filecount = m_cubemap->GetFileNames().size();
		ofs.write(CharPtr(&filecount), sizeof(filecount));
		for (const auto& file : m_cubemap->GetFileNames()) {
			auto fs = file.size();
			ofs.write(CharPtr(&fs), sizeof(fs));
			ofs.write(file.data(), fs);
		}
	}
	else {
		bool isCubeMap = false;
		ofs.write(CharPtr(&isCubeMap), sizeof(isCubeMap));
	}
}


Material::~Material()
{
}
}
