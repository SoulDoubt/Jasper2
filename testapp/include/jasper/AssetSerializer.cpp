#include "AssetSerializer.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Scene.h"
#include "BasicShader.h"
#include "TextureShader.h"
#include "GuiShader.h"
#include "LitShader.h"
#include "FontShader.h"
#include "SkyboxShader.h"
#include <memory>


namespace Jasper
{
namespace AssetSerializer
{

using namespace std;

void SerializeMesh(ofstream& ofs, Mesh* mesh)
{
    // each mesh will write the following to the stream
    // 1) Count of Positions
    // 2) Position Data
    // 3) COunt of Normals
    // 4) Normal Data
    // and so on in that fashion


    int posc = mesh->Positions.size();
    int texc = mesh->TexCoords.size();
    int norc = mesh->Normals.size();
    int tanc = mesh->Tangents.size();
    int bitanc = mesh->Bitangents.size();
    // write positions
    ofs.write(CharPtr(&posc), sizeof(posc));
    ofs.write(CharPtr(&(mesh->Positions[0])), sizeof(Vector3) * posc);
    cout << mesh->Positions[0].ToString() << endl;
    cout << mesh->Positions[1].ToString() << endl;
    // write normals
    ofs.write(CharPtr(&norc), sizeof(norc));
    ofs.write(CharPtr(&(mesh->Normals[0])), sizeof(Vector3) * norc);
    // write tex coords
    ofs.write(CharPtr(&texc), sizeof(texc));
    ofs.write(CharPtr(&(mesh->TexCoords[0])), sizeof(Vector2) * texc);
    // write tangents
    ofs.write(CharPtr(&tanc), sizeof(tanc));
    ofs.write(CharPtr(&(mesh->Tangents[0])), sizeof(Vector4) * tanc);
    // write bitans
    ofs.write(CharPtr(&bitanc), sizeof(bitanc));
    ofs.write(CharPtr(&(mesh->Bitangents[0])), sizeof(Vector3) * bitanc);

}


void SerializeMaterial(std::ofstream& ofs, const Material* mat)
{
    using namespace AssetSerializer;

    string name = mat->GetName();
    size_t nameSize = name.size();
    ofs.write(CharPtr(&nameSize), sizeof(nameSize));
    ofs.write(name.c_str(), nameSize * sizeof(char));
    ofs.write(ConstCharPtr(mat->Ambient.AsFloatPtr()), sizeof(Vector3));
    ofs.write(ConstCharPtr(mat->Diffuse.AsFloatPtr()), sizeof(Vector3));
    ofs.write(ConstCharPtr(mat->Specular.AsFloatPtr()), sizeof(Vector3));
    ofs.write(ConstCharPtr(&(mat->Shine)), sizeof(float));

    string shaderName = mat->GetShaderName();
    size_t shaderNameLength = shaderName.size();
    ofs.write(CharPtr(&shaderNameLength), sizeof(shaderNameLength));
    ofs.write(shaderName.c_str(), sizeof(char) * shaderNameLength);

    size_t diffuseLength = mat->DiffuseTextureFilename().size();
    ofs.write(CharPtr(&diffuseLength), sizeof(diffuseLength));
    ofs.write(mat->DiffuseTextureFilename().c_str(), sizeof(char) * diffuseLength);

    bool hasNormalMap = mat->GetTextureNormalMap() != nullptr;
    bool hasSpecularMap = mat->GetTextureSpecularMap() != nullptr;

    ofs.write(CharPtr(&hasNormalMap), sizeof(hasNormalMap));
    if (hasNormalMap) {
        auto nm = mat->GetTextureNormalMap();
        string normalMapFile = nm->GetFilename();
        size_t norlen = normalMapFile.size();
        ofs.write(CharPtr(&norlen), sizeof(norlen));
        ofs.write(normalMapFile.c_str(), sizeof(char) * norlen);
    }

    ofs.write(CharPtr(&hasSpecularMap), sizeof(hasSpecularMap));
    if (hasSpecularMap) {
        auto sm = mat->GetTextureSpecularMap();
        string specMapFile = sm->GetFilename();
        size_t speclen = specMapFile.size();
        ofs.write(CharPtr(&speclen), sizeof(speclen));
        ofs.write(specMapFile.c_str(), sizeof(char) * speclen);
    }
}

void ConstructMaterial(std::ifstream& ifs, Scene* scene)
{
    // name size then name...
    size_t namelen;
    ifs.read(CharPtr(&namelen), sizeof(namelen));
    char namebuff[namelen];
    ifs.read(namebuff, namelen);
    string matName = string(namebuff);
    printf("Deserialized Material Name: %s\n", matName.c_str());


    Vector3 ambient, diffuse, specular;
    float shine;
    ifs.read(CharPtr(&ambient), sizeof(ambient));
    ifs.read(CharPtr(&diffuse), sizeof(diffuse));
    ifs.read(CharPtr(&specular), sizeof(specular));
    ifs.read(CharPtr(&shine), sizeof(shine));

    size_t sns;
    ifs.read(CharPtr(&sns), sizeof(sns));
    char snbuff[sns];
    ifs.read(snbuff, sns);
    string shaderName = string(snbuff);

    string diffuseMapPath = "";
    size_t dms;
    ifs.read(CharPtr(&dms), sizeof(dms));
    char dmbuff[dms];
    ifs.read(dmbuff, dms);
    diffuseMapPath = string(dmbuff);
    printf("Deserialized Diffuse Map Path: %s\n", diffuseMapPath.c_str());

    // read the boolean for normal map
    // if true, the next bytes are the normal map file path
    bool hasNormalMap;
    string normalMapPath = "";
    ifs.read(CharPtr(&hasNormalMap), sizeof(hasNormalMap));
    if (hasNormalMap) {
        size_t nms;
        ifs.read(CharPtr(&nms), sizeof(nms));
        char nmbuff[nms];
        ifs.read(nmbuff, nms);
        normalMapPath = string(nmbuff);
        printf("Deserialized Normal Map Path: %s\n", normalMapPath.c_str());
    }

    bool hasSpecMap;
    string specMapPath = "";
    ifs.read(CharPtr(&hasSpecMap), sizeof(hasSpecMap));
    if (hasSpecMap) {
        size_t sms;
        ifs.read(CharPtr(&sms), sizeof(sms));
        char smbuff[sms];
        ifs.read(smbuff, sms);
        specMapPath = string(smbuff);
        printf("Deserialized Specular Map Path: %s\n", specMapPath.c_str());
    }

    Material* cachedMaterial = scene->GetMaterialByName(matName);
    if (cachedMaterial) {
        printf("Material already in cache: %s\n", matName.c_str());
        return;
    }

    Shader* mShader = scene->GetShaderByName(shaderName);
    if (!mShader) {
        printf("Shader %s not found in cache, unable to load material %s\n", shaderName.c_str(), matName.c_str());
        return;
    }

    Material* mm = scene->GetMaterialCache().CreateInstance<Material>(mShader, matName);
    mm->Ambient = ambient;
    mm->Diffuse = diffuse;
    mm->Specular = specular;
    mm->Shine = shine;
    mm->SetTextureDiffuse(diffuseMapPath);
    if (hasNormalMap) {
        mm->SetTextureNormalMap(normalMapPath);
    }
    if (hasSpecMap) {
        mm->SetTextureSpecularMap(specMapPath);
    }

}

void SerializeShader(std::ofstream& ofs, const Shader* shader)
{
    // really all we need to store is the type enum and the name
    ShaderClassType ty = shader->GetShaderClassType();
    ofs.write(CharPtr(&ty), sizeof(ty));
    size_t namelen = shader->GetName().size();
    ofs.write(CharPtr(&namelen), sizeof(namelen));
    ofs.write(shader->GetName().c_str(), sizeof(char) * namelen);
}


void ConstructShader(std::ifstream& ifs, Scene* scene)
{
    ShaderClassType typ;
    ifs.read(CharPtr(&typ), sizeof(typ));
    size_t namelen;
    ifs.read(CharPtr(&namelen), sizeof(namelen));
    char namebuff[namelen];
    ifs.read(namebuff, namelen);
    string name = string(namebuff);
    printf("Deserializing shader name: %s\n", name.c_str());

    Shader* s = scene->GetShaderByName(name);
    if (s) {
        printf("Shader already in tha cache: %s\n", name.c_str());
        return;
    }

    switch (typ) {
    case ShaderClassType::BasicShader :
        scene->GetShaderCache().CreateInstance<BasicShader>();
        break;
    case (ShaderClassType::FontShader):
        scene->GetShaderCache().CreateInstance<FontShader>();
        break;
    case ShaderClassType::GuiShader:
        scene->GetShaderCache().CreateInstance<GuiShader>();
        break;
    case ShaderClassType::LitShader:
        scene->GetShaderCache().CreateInstance<LitShader>();
        break;
    case ShaderClassType::SkyboxShader:
        scene->GetShaderCache().CreateInstance<SkyboxShader>(name);
        break;
    case ShaderClassType::TextureShader:
        scene->GetShaderCache().CreateInstance<TextureShader>();
        break;
    }
}


}

}
