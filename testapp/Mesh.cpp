#include <GL/glew.h>
#include "Mesh.h"
#include "Shader.h"
#include "GameObject.h"
#include "GLError.h"
#include <AssetSerializer.h>

namespace Jasper
{
using namespace std;


Mesh::Mesh(const std::string& name) : Component(name)
{
    Initialize();
    CalculateExtents();
}


Mesh::~Mesh()
{
    Destroy();
}

void Mesh::Initialize()
{

}

void Mesh::Serialize(std::ofstream& ofs) const {
	using namespace AssetSerializer;
	Component::Serialize(ofs);
	auto mt = GetMeshType();
	ofs.write(ConstCharPtr(&mt), sizeof(mt));
	if (GetMeshType() == MeshType::Arbitrary) {
		SerializeMesh(ofs, this);
	}
}

void Mesh::Destroy()
{
    //Vertices.clear();
    Positions.clear();
    Normals.clear();
    TexCoords.clear();
    Colors.clear();
    Tangents.clear();
    Bitangents.clear();
    Indices.clear();
    //Vertices.clear();
}

void Mesh::FlipTextureCoords()
{
    for (auto& tc : TexCoords) {
        tc.y *= -1;
    }
}

void Mesh::CalculateFaceNormals()
{
    for (unsigned int i = 0; i < Indices.size(); i += 3) {
        const unsigned index0 = Indices[i];
        const unsigned index1 = Indices[i + 1];
        const unsigned index2 = Indices[i + 2];

        const auto& v1 = Positions[index0];
        const auto& v2 = Positions[index1];
        const auto& v3 = Positions[index2];
		
        const auto& t1 = TexCoords[index0];
        const auto& t2 = TexCoords[index1];
        const auto& t3 = TexCoords[index2];

        // Tangent Space calculations adapted from
        // Lengyel, Eric. Computing Tangent Space Basis Vectors for an Arbitrary Mesh.
        // Terathon Software 3D Graphics Library, 2001. http://www.terathon.com/code/tangent.html

        const float x1 = v2.x - v1.x;
        const float x2 = v3.x - v1.x;
        const float y1 = v2.y - v1.y;
        const float y2 = v3.y - v1.y;
        const float z1 = v2.z - v1.z;
        const float z2 = v3.z - v1.z;

        const float s1 = t2.x - t1.x;
        const float s2 = t3.x - t1.x;
        const float tt1 = t2.y - t1.y;
        const float tt2 = t3.y - t1.y;

        const float r = 1.0F / (s1 * tt2 - s2 * tt1);
        const Vector3 sdir((tt2 * x1 - tt1 * x2) * r, (tt2 * y1 - tt1 * y2) * r,
                     (tt2 * z1 - tt1 * z2) * r);
        const Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                     (s1 * z2 - s2 * z1) * r);

        const Vector4 tangent = { sdir, 0.0f };
        const Vector3 bitangent = tdir;

        const Vector3 edge1 = v2 - v1;
        const Vector3 edge2 = v3 - v1;

        Vector3 normal = (edge1).Cross(edge2);
        normal = Normalize(normal);

        //const Vector3 tangent = Normalize((v0.Position * uv1.y - v1.Position * uv1.y) * c);
        Normals[index0] += normal;
        Normals[index1] += normal;
        Normals[index2] += normal;

        Tangents[index0] += tangent;
        Tangents[index1] += tangent;
        Tangents[index2] += tangent;

        Bitangents[index0] += bitangent;
        Bitangents[index1] += bitangent;
        Bitangents[index2] += bitangent;

    }

    for (auto index : Indices) {
        auto& norm = Normals[index];
        auto& tan = Tangents[index];
        const auto& bitan = Bitangents[index];

        norm = Normalize(norm);
        // Gram-Schmidt orthogonalize
        tan = { (tan.AsVector3() - norm * Dot(norm, tan.AsVector3())).Normalized(), 0.0f };
        tan.w = (Dot(Cross(norm, tan.AsVector3()), bitan) > 0.0f) ? -1.0f : 1.0f;
        tan = Normalize(tan);
    }

    printf("Calculated some normals and Tangents\n");
}

void Mesh::CalculateExtents()
{
    float xmin, ymin, zmin;
    xmin = ymin = zmin = 1000000.0f;
    float xmax, ymax, zmax;
    xmax = ymax = zmax = -1000000.0f;
    for (auto& vp : Positions) {
        //auto vp = v.Position;
        if (vp.x < xmin) xmin = vp.x;
        if (vp.y < ymin) ymin = vp.y;
        if (vp.z < zmin) zmin = vp.z;
        if (vp.x > xmax) xmax = vp.x;
        if (vp.y > ymax) ymax = vp.y;
        if (vp.z > zmax) zmax = vp.z;
    }
    const float x = (xmax - xmin) / 2.f;
    const float y = (ymax - ymin) / 2.f;
    const float z = (zmax - zmin) / 2.f;
    m_Extents = Vector3(x, y, z);
    m_minExtents = Vector3(xmin, ymin, zmin);
    m_maxExtents = Vector3(xmax, ymax, zmax);
  
}

} // namespace Jasper
