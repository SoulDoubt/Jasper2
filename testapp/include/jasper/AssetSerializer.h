#pragma once

#include <iostream>
#include <fstream>


namespace Jasper
{

class Material;
class Mesh;
class Shader;
class Scene;
class GameObject;

namespace AssetSerializer
{

void SerializeMesh(std::ofstream& ofs, const Mesh* mesh);
void ConstructMesh(std::ifstream& ifs, Scene* scene);
void SerializeMaterial(std::ofstream& ofs, const Material* mat);
void ConstructMaterial(std::ifstream& ifs, Scene* scene);
void SerializeShader(std::ofstream&, const Shader* shader);
void ConstructShader(std::ifstream& ifs, Scene* scene);
void ConstructMeshRenderer(std::ifstream& ifs, std::string name, GameObject* go, Scene* scene);
void ConstructSkyboxRenderer(std::ifstream& ifs, std::string name, GameObject* go, Scene* scene);
void ConstructPhysicsCollider(std::ifstream& ifs, std::string name, GameObject* go, Scene* scene);
void ConstructScriptComponent(std::ifstream& ifs, std::string name, GameObject* go, Scene* scene);


template<typename T>
constexpr char* CharPtr(T* val)
{
    return reinterpret_cast<char*>(val);
}
//
template<typename T>
constexpr const char* ConstCharPtr(T* val){
    return reinterpret_cast<const char*>(val);
}

}

}
