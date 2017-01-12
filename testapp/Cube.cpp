#include "Cube.h"
#include "GameObject.h"
#include "Shader.h"
#include <GL/glew.h>
#include <AssetSerializer.h>

namespace Jasper
{

Cube::Cube(const std::string& name) : Mesh(name)
{
    Dimensions = Vector3(0.5f, 0.5f, 0.5f);
    //Initialize();
}

Cube::Cube(const std::string& name, const Vector3& dimensions) : Mesh(name)
{
    Dimensions = dimensions;
    //Initialize();
}

Cube::Cube(const std::string& name, const Vector3& dimensions, bool reverseWinding) : Mesh(name)
{
    m_isCubemap = reverseWinding;
    Dimensions = dimensions;
    //Initialize();
}


Cube::~Cube()
{
    Destroy();
}

void Cube::Serialize(std::ofstream& ofs) const {
	using namespace AssetSerializer;
	Mesh::Serialize(ofs);
	ofs.write(ConstCharPtr(Dimensions.AsFloatPtr()), sizeof(Dimensions));
	ofs.write(ConstCharPtr(&m_isCubemap), sizeof(m_isCubemap));
	ofs.write(ConstCharPtr(&m_repeatU), sizeof(m_repeatU));
	ofs.write(ConstCharPtr(&m_repeatV), sizeof(m_repeatV));	


}

void Cube::Initialize()
{   
    if (m_isCubemap) {
        LoadCubemapVerts();
    } else {
        LoadStandardVerts();
    }
    Mesh::Initialize();
}

void Cube::LoadCubemapVerts()
{
    const float x = Dimensions.x;
    const float y = Dimensions.y;
    const float z = Dimensions.z;
    AddVertex( { Vector3(-x, y, -z) });
    AddVertex( { Vector3(-x, -y, -z) });
    AddVertex( { Vector3(x, -y, -z) });
    AddVertex( { Vector3(x, -y, -z) });
    AddVertex( { Vector3(x, y, -z) });
    AddVertex( { Vector3(-x, y, -z) });

    AddVertex( { Vector3(-x, -y, z) });
    AddVertex( { Vector3(-x, -y, -z) });
    AddVertex( { Vector3(-x, y, -z) });
    AddVertex( { Vector3(-x, y, -z) });
    AddVertex( { Vector3(-x, y, z) });
    AddVertex( { Vector3(-x, -y, z) });

    AddVertex( { Vector3(x, -y, -z) });
    AddVertex( { Vector3(x, -y, z) });
    AddVertex( { Vector3(x, y, z) });
    AddVertex( { Vector3(x, y, z) });
    AddVertex( { Vector3(x, y, -z) });
    AddVertex( { Vector3(x, -y, -z) });

    AddVertex( { Vector3(-x, -y, z) });
    AddVertex( { Vector3(-x, y, z) });
    AddVertex( { Vector3(x, y, z) });
    AddVertex( { Vector3(x, y, z) });
    AddVertex( { Vector3(x, -y, z) });
    AddVertex( { Vector3(-x, -y, z) });

    AddVertex( { Vector3(-x, y, -z) });
    AddVertex( { Vector3(x, y, -z) });
    AddVertex( { Vector3(x, y, z) });
    AddVertex( { Vector3(x, y, z) });
    AddVertex( { Vector3(-x, y, z) });
    AddVertex( { Vector3(-x, y, -z) });

    AddVertex( { Vector3(-x, -y, -z) });
    AddVertex( { Vector3(-x, -y, z) });
    AddVertex( { Vector3(x, -y, -z) });
    AddVertex( { Vector3(x, -y, -z) });
    AddVertex( { Vector3(-x, -y, z) });
    AddVertex( { Vector3(x, -y, z) });

    Indices.push_back(0);
    Indices.push_back(1);
    Indices.push_back(2);
    Indices.push_back(3);
    Indices.push_back(4);
    Indices.push_back(5);
    Indices.push_back(6);
    Indices.push_back(7);
    Indices.push_back(8);
    Indices.push_back(0);
    Indices.push_back(10);
    Indices.push_back(11);
    Indices.push_back(12);
    Indices.push_back(13);
    Indices.push_back(14);
    Indices.push_back(15);
    Indices.push_back(16);
    Indices.push_back(17);
    Indices.push_back(18);
    Indices.push_back(19);
    Indices.push_back(20);
    Indices.push_back(21);
    Indices.push_back(22);
    Indices.push_back(23);
    Indices.push_back(24);
    Indices.push_back(25);
    Indices.push_back(26);
    Indices.push_back(27);
    Indices.push_back(28);
    Indices.push_back(29);
    Indices.push_back(30);
    Indices.push_back(31);
    Indices.push_back(32);
    Indices.push_back(33);
    Indices.push_back(34);
    Indices.push_back(35);
    //CalculateFaceNormals();
    CalculateExtents();
}

void Cube::LoadStandardVerts()
{


    const float x = Dimensions.x;
    const float y = Dimensions.y;
    const float z = Dimensions.z;

    Positions.reserve(24);
    Positions.push_back(Vector3(x  ,-y  ,z));
    Positions.push_back(Vector3(x   ,y  ,z));
    Positions.push_back(Vector3(-x  ,y  ,z));
    Positions.push_back(Vector3(-x ,-y  ,z));
    Positions.push_back(Vector3(-x ,-y ,-z));
    Positions.push_back(Vector3(-x  ,y ,-z));
    Positions.push_back(Vector3(x   ,y ,-z));
    Positions.push_back(Vector3(x  ,-y ,-z));
    Positions.push_back(Vector3(-x ,-y  ,z));
    Positions.push_back(Vector3(-x  ,y  ,z));
    Positions.push_back(Vector3(-x  ,y ,-z));
    Positions.push_back(Vector3(-x ,-y ,-z));
    Positions.push_back(Vector3(x  ,-y ,-z));
    Positions.push_back(Vector3(x   ,y ,-z));
    Positions.push_back(Vector3(x   ,y , z));
    Positions.push_back(Vector3(x  ,-y  ,z));
    Positions.push_back(Vector3(x   ,y  ,z));
    Positions.push_back(Vector3(x   ,y ,-z));
    Positions.push_back(Vector3(-x  ,y ,-z));
    Positions.push_back(Vector3(-x  ,y  ,z));
    Positions.push_back(Vector3(x  ,-y ,-z));
    Positions.push_back(Vector3(x  ,-y  ,z));
    Positions.push_back(Vector3(-x ,-y  ,z));
    Positions.push_back(Vector3(-x ,-y ,-z));
// TexCoord Data:
    TexCoords.reserve(24);
    TexCoords.push_back(Vector2(1.000000f * m_repeatU,1.000000f  * m_repeatV));
    TexCoords.push_back(Vector2(1.000000f * m_repeatU,0.000000f));
    TexCoords.push_back(Vector2(0.000000f,0.000000f));
    TexCoords.push_back(Vector2(0.000000f,1.000000f  * m_repeatV));
    TexCoords.push_back(Vector2(1.000000f * m_repeatU,1.000000f  * m_repeatV));
    TexCoords.push_back(Vector2(1.000000f * m_repeatU,0.000000f));
    TexCoords.push_back(Vector2(0.000000f,0.000000f));
    TexCoords.push_back(Vector2(0.000000f,1.000000f  * m_repeatV));
    TexCoords.push_back(Vector2(1.000000f * m_repeatU,1.000000f  * m_repeatV));
    TexCoords.push_back(Vector2(1.000000f * m_repeatU,0.000000f));
    TexCoords.push_back(Vector2(0.000000f,0.000000f));
    TexCoords.push_back(Vector2(0.000000f,1.000000f  * m_repeatV));
    TexCoords.push_back(Vector2(1.000000f * m_repeatU,1.000000f  * m_repeatV));
    TexCoords.push_back(Vector2(1.000000f * m_repeatU,0.000000f));
    TexCoords.push_back(Vector2(0.000000f,0.000000f));
    TexCoords.push_back(Vector2(0.000000f,1.000000f  * m_repeatV));
    TexCoords.push_back(Vector2(1.000000f * m_repeatU,1.000000f  * m_repeatV));
    TexCoords.push_back(Vector2(1.000000f * m_repeatU,0.000000f));
    TexCoords.push_back(Vector2(0.000000f,0.000000f));
    TexCoords.push_back(Vector2(0.000000f,1.000000f  * m_repeatV));
    TexCoords.push_back(Vector2(1.000000f * m_repeatU,1.000000f  * m_repeatV));
    TexCoords.push_back(Vector2(1.000000f * m_repeatU,0.000000f));
    TexCoords.push_back(Vector2(0.000000f,0.000000f));
    TexCoords.push_back(Vector2(0.000000f,1.000000f  * m_repeatV));
// Normal Data:
    Normals.reserve(24);
    Normals.push_back(Vector3(0.000000f,0.000000f,1.000000f));
    Normals.push_back(Vector3(0.000000f,0.000000f,1.000000f));
    Normals.push_back(Vector3(0.000000f,0.000000f,1.000000f));
    Normals.push_back(Vector3(0.000000f,0.000000f,1.000000f));
    Normals.push_back(Vector3(0.000000f,0.000000f,-1.000000f));
    Normals.push_back(Vector3(0.000000f,0.000000f,-1.000000f));
    Normals.push_back(Vector3(0.000000f,0.000000f,-1.000000f));
    Normals.push_back(Vector3(0.000000f,0.000000f,-1.000000f));
    Normals.push_back(Vector3(-1.000000f,0.000000f,0.000000f));
    Normals.push_back(Vector3(-1.000000f,0.000000f,0.000000f));
    Normals.push_back(Vector3(-1.000000f,0.000000f,0.000000f));
    Normals.push_back(Vector3(-1.000000f,0.000000f,0.000000f));
    Normals.push_back(Vector3(1.000000f,0.000000f,0.000000f));
    Normals.push_back(Vector3(1.000000f,0.000000f,0.000000f));
    Normals.push_back(Vector3(1.000000f,0.000000f,0.000000f));
    Normals.push_back(Vector3(1.000000f,0.000000f,0.000000f));
    Normals.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Normals.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Normals.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Normals.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Normals.push_back(Vector3(0.000000f,-1.000000f,0.000000f));
    Normals.push_back(Vector3(0.000000f,-1.000000f,0.000000f));
    Normals.push_back(Vector3(0.000000f,-1.000000f,0.000000f));
    Normals.push_back(Vector3(0.000000f,-1.000000f,0.000000f));
// Tangent Data:
    Tangents.reserve(24);
    Tangents.push_back(Vector4(1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(-1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(-1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(-1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(-1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(0.000000f, 0.000000f, 1.000000f, 1.000000f));
    Tangents.push_back(Vector4(0.000000f, 0.000000f, 1.000000f, 1.000000f));
    Tangents.push_back(Vector4(0.000000f, 0.000000f, 1.000000f, 1.000000f));
    Tangents.push_back(Vector4(0.000000f, 0.000000f, 1.000000f, 1.000000f));
    Tangents.push_back(Vector4(0.000000f, 0.000000f, -1.000000f, 1.000000f));
    Tangents.push_back(Vector4(0.000000f, 0.000000f, -1.000000f, 1.000000f));
    Tangents.push_back(Vector4(0.000000f, 0.000000f, -1.000000f, 1.000000f));
    Tangents.push_back(Vector4(0.000000f, 0.000000f, -1.000000f, 1.000000f));
    Tangents.push_back(Vector4(1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(1.000000f, 0.000000f, 0.000000f, 1.000000f));
    Tangents.push_back(Vector4(1.000000f, 0.000000f, 0.000000f, 1.000000f));
// BiTangent Data:
    Bitangents.reserve(24);
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,1.000000f,0.000000f));
    Bitangents.push_back(Vector3(0.000000f,0.000000f,-1.000000f));
    Bitangents.push_back(Vector3(0.000000f,0.000000f,-1.000000f));
    Bitangents.push_back(Vector3(0.000000f,0.000000f,-1.000000f));
    Bitangents.push_back(Vector3(0.000000f,0.000000f,-1.000000f));
    Bitangents.push_back(Vector3(0.000000f,0.000000f,1.000000f));
    Bitangents.push_back(Vector3(0.000000f,0.000000f,1.000000f));
    Bitangents.push_back(Vector3(0.000000f,0.000000f,1.000000f));
    Bitangents.push_back(Vector3(0.000000f,0.000000f,1.000000f));
// Index Data:
    Indices.reserve(36);
    Indices.push_back(0);
    Indices.push_back(1);
    Indices.push_back(2);
    Indices.push_back(0);
    Indices.push_back(2);
    Indices.push_back(3);
    Indices.push_back(4);
    Indices.push_back(5);
    Indices.push_back(6);
    Indices.push_back(4);
    Indices.push_back(6);
    Indices.push_back(7);
    Indices.push_back(8);
    Indices.push_back(9);
    Indices.push_back(10);
    Indices.push_back(8);
    Indices.push_back(10);
    Indices.push_back(11);
    Indices.push_back(12);
    Indices.push_back(13);
    Indices.push_back(14);
    Indices.push_back(12);
    Indices.push_back(14);
    Indices.push_back(15);
    Indices.push_back(16);
    Indices.push_back(17);
    Indices.push_back(18);
    Indices.push_back(16);
    Indices.push_back(18);
    Indices.push_back(19);
    Indices.push_back(20);
    Indices.push_back(21);
    Indices.push_back(22);
    Indices.push_back(20);
    Indices.push_back(22);
    Indices.push_back(23);

    CalculateExtents();
}


void Cube::Destroy()
{

}

}
