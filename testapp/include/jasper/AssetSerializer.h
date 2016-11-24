#pragma once

#include <iostream>
#include <fstream>


namespace Jasper
{

class Material;
class Mesh;
class Shader;
class Scene;

namespace AssetSerializer
{

void SerializeMesh(std::ofstream& ofs, Mesh* Mesh);
void SerializeMaterial(std::ofstream& ofs, Material* mat);
void ConstructMaterial(std::ifstream& ifs, Scene* scene);
void SerializeShader(std::ofstream&, Shader* shader);
void ConstructShader(std::ifstream& ifs, Scene* scene);


template<typename T>
char* CharPtr(T* val)
{
    return reinterpret_cast<char*>(val);
}

}

}
