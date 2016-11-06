#include "Cube.h"
#include "GameObject.h"
#include "Shader.h"
#include <GL/glew.h>

namespace Jasper
{

Cube::Cube(const std::string& name) : Mesh(name)
{
    Dimensions = Vector3(0.5f, 0.5f, 0.5f);
    Initialize();
}

Cube::Cube(const std::string& name, const Vector3& dimensions) : Mesh(name)
{
    Dimensions = dimensions;
    Initialize();
}

Cube::Cube(const std::string& name, const Vector3& dimensions, bool reverseWinding) : Mesh(name)
{
    m_isCubemap = reverseWinding;
    Dimensions = dimensions;
    Initialize();
}


Cube::~Cube()
{
    Destroy();
}

void Cube::Initialize()
{
    // construct a unit cube centered on the axis.

    //float x = Dimensions.x;
    //float y = Dimensions.y;
    //float z = Dimensions.z;

    //// Front
    //Vertex f0 = { -x, -y, z, 0.0f, 0.0f }; // 0
    //Vertex f1 = { -x, y, z, 0.0f, 1.0f };  // 1
    //Vertex f2 = { x, y, z, 1.0f, 1.0f };   // 2
    //Vertex f3 = { x, -y, z, 1.0f, 0.0f };  // 3

    //AddVertex(f0);
    //AddVertex(f1);
    //AddVertex(f2);
    //AddVertex(f3);

    //Indices.push_back(0);
    //Indices.push_back(3);
    //Indices.push_back(2);
    //Indices.push_back(2);
    //Indices.push_back(1);
    //Indices.push_back(0);

    //// right side
    //Vertex r0 = { x, -y, z, 0.0f, 0.0f };
    //Vertex r1 = { x, y, z, 0.0f, 1.0f };
    //Vertex r2 = { x, y, -z, 1.0f, 1.0f };
    //Vertex r3 = { x, -y, -z, 1.0f, 0.0f };

    //AddVertex(r0); // 4
    //AddVertex(r1); // 5
    //AddVertex(r2); // 6
    //AddVertex(r3); // 7

    //Indices.push_back(4);
    //Indices.push_back(7);
    //Indices.push_back(6);
    //Indices.push_back(6);
    //Indices.push_back(5);
    //Indices.push_back(4);

    //// left side
    //Vertex l0 = { -x, -y, -z, 0.0f, 0.0f }; // 8
    //Vertex l1 = { -x, -y, z, 1.0f, 0.0f };  // 9
    //Vertex l2 = { -x, y, z, 1.0f, 1.0f };   // 10
    //Vertex l3 = { -x, y, -z, 0.0f, 1.0f };  // 11

    //AddVertex(l0);
    //AddVertex(l1);
    //AddVertex(l2);
    //AddVertex(l3);

    //Indices.push_back(8);
    //Indices.push_back(9);
    //Indices.push_back(10);
    //Indices.push_back(10);
    //Indices.push_back(11);
    //Indices.push_back(8);

    //// "Back"
    //Vertex b0 = { -x, -y, -z, 1.0f, 0.0f }; // 12
    //Vertex b1 = { x, -y, -z, 0.0f, 0.0f };  // 13
    //Vertex b2 = { x, y, -z, 0.0f, 1.0f };   // 14
    //Vertex b3 = { -x, y, -z, 1.0f, 1.0f };  // 15

    //AddVertex(b0);
    //AddVertex(b1);
    //AddVertex(b2);
    //AddVertex(b3);

    //Indices.push_back(13);
    //Indices.push_back(12);
    //Indices.push_back(15);
    //Indices.push_back(15);
    //Indices.push_back(14);
    //Indices.push_back(13);

    //// TOP
    //Vertex t0 = { -x, y, z, 0.0f, 0.0f };		// 16
    //Vertex t1 = { -x, y, -z, 0.0f, 1.0f };	// 17
    //Vertex t2 = { x, y, -z, 1.0f, 1.0f };		// 18
    //Vertex t3 = { x, y, z, 1.0f, 0.0f };		// 19

    //AddVertex(t0);
    //AddVertex(t1);
    //AddVertex(t2);
    //AddVertex(t3);

    //Indices.push_back(16);
    //Indices.push_back(19);
    //Indices.push_back(18);
    //Indices.push_back(18);
    //Indices.push_back(17);
    //Indices.push_back(16);

    //// Bottom
    //Vertex u0 = { -x, -y, z, 0.0f, 0.0f };	// 20
    //Vertex u1 = { -x, -y, -z, 0.0f, 1.0f };	// 21
    //Vertex u2 = { x, -y, -z, 1.0f, 1.0f };	// 22
    //Vertex u3 = { x, -y, z, 1.0f, 0.0f };		// 23

    //AddVertex(u0);
    //AddVertex(u1);
    //AddVertex(u2);
    //AddVertex(u3);

    //Indices.push_back(20);
    //Indices.push_back(21);
    //Indices.push_back(22);
    //Indices.push_back(22);
    //Indices.push_back(23);
    //Indices.push_back(20);

    //CalculateFaceNormals();
    //if (m_reverseWinding) {
    //	ReverseWinding();
    //}
    if (m_isCubemap) {
        LoadCubemapVerts();
    } else {
        LoadStandardVerts();
    }
}

void Cube::LoadCubemapVerts()
{
    float x = Dimensions.x;
    float y = Dimensions.y;
    float z = Dimensions.z;
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


    float x = Dimensions.x;
    float y = Dimensions.y;
    float z = Dimensions.z;
//
//	// Front
//	Vertex f0 = { -x, -y, z, 0.0f, 0.0f }; // 0
//	Vertex f1 = { -x, y, z, 0.0f, 1.0f };  // 1
//	Vertex f2 = { x, y, z, 1.0f, 1.0f };   // 2
//	Vertex f3 = { x, -y, z, 1.0f, 0.0f };  // 3
//
//	AddVertex(f0);
//	AddVertex(f1);
//	AddVertex(f2);
//	AddVertex(f3);
//
//	Indices.push_back(0);
//	Indices.push_back(3);
//	Indices.push_back(2);
//	Indices.push_back(2);
//	Indices.push_back(1);
//	Indices.push_back(0);
//
//	// right side
//	Vertex r0 = { x, -y, z, 0.0f, 0.0f };
//	Vertex r1 = { x, y, z, 0.0f, 1.0f };
//	Vertex r2 = { x, y, -z, 1.0f, 1.0f };
//	Vertex r3 = { x, -y, -z, 1.0f, 0.0f };
//
//	AddVertex(r0); // 4
//	AddVertex(r1); // 5
//	AddVertex(r2); // 6
//	AddVertex(r3); // 7
//
//	Indices.push_back(4);
//	Indices.push_back(7);
//	Indices.push_back(6);
//	Indices.push_back(6);
//	Indices.push_back(5);
//	Indices.push_back(4);
//
//	// left side
//	Vertex l0 = { -x, -y, -z, 0.0f, 0.0f }; // 8
//	Vertex l1 = { -x, -y, z, 1.0f, 0.0f };  // 9
//	Vertex l2 = { -x, y, z, 1.0f, 1.0f };   // 10
//	Vertex l3 = { -x, y, -z, 0.0f, 1.0f };  // 11
//
//	AddVertex(l0);
//	AddVertex(l1);
//	AddVertex(l2);
//	AddVertex(l3);
//
//	Indices.push_back(8);
//	Indices.push_back(9);
//	Indices.push_back(10);
//	Indices.push_back(10);
//	Indices.push_back(11);
//	Indices.push_back(8);
//
//	// "Back"
//	Vertex b0 = { -x, -y, -z, 1.0f, 0.0f }; // 12
//	Vertex b1 = { x, -y, -z, 0.0f, 0.0f };  // 13
//	Vertex b2 = { x, y, -z, 0.0f, 1.0f };   // 14
//	Vertex b3 = { -x, y, -z, 1.0f, 1.0f };  // 15
//
//	AddVertex(b0);
//	AddVertex(b1);
//	AddVertex(b2);
//	AddVertex(b3);
//
//	Indices.push_back(13);
//	Indices.push_back(12);
//	Indices.push_back(15);
//	Indices.push_back(15);
//	Indices.push_back(14);
//	Indices.push_back(13);
//
//	// TOP
//	Vertex t0 = { -x, y, z, 0.0f, 0.0f };		// 16
//	Vertex t1 = { -x, y, -z, 0.0f, 1.0f };	// 17
//	Vertex t2 = { x, y, -z, 1.0f, 1.0f };		// 18
//	Vertex t3 = { x, y, z, 1.0f, 0.0f };		// 19
//
//	AddVertex(t0);
//	AddVertex(t1);
//	AddVertex(t2);
//	AddVertex(t3);
//
//	Indices.push_back(16);
//	Indices.push_back(19);
//	Indices.push_back(18);
//	Indices.push_back(18);
//	Indices.push_back(17);
//	Indices.push_back(16);
//
//	// Bottom
//	Vertex u0 = { -x, -y, z, 0.0f, 0.0f };	// 20
//	Vertex u1 = { -x, -y, -z, 0.0f, 1.0f };	// 21
//	Vertex u2 = { x, -y, -z, 1.0f, 1.0f };	// 22
//	Vertex u3 = { x, -y, z, 1.0f, 0.0f };	// 23
//
//	AddVertex(u0);
//	AddVertex(u1);
//	AddVertex(u2);
//	AddVertex(u3);
//
//	Indices.push_back(20);
//	Indices.push_back(21);
//	Indices.push_back(22);
//	Indices.push_back(20);
//	Indices.push_back(21);
//	Indices.push_back(23);

//    for(int i = 0; i < 12 * 3; i++){
//        Indices.push_back(i);
//    }

    //CalculateFaceNormals();

// Outputting Mesh data for: mathias
// Position Data:
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
