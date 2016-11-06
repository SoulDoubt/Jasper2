#pragma once

#include <iostream>
#include <fstream>
#include "Model.h"


namespace Jasper
{

namespace AssetSerializer
{

void SerializeMesh(std::ofstream& ofs, Mesh* Mesh);
void SerializeMaterial(std::ofstream& ofs, Material* mat);

template<typename T>
char* CharPtr(T* val)
{
    return reinterpret_cast<char*>(val);
}

}

}
