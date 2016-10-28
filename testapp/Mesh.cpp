#include <GL/glew.h>
#include "Mesh.h"
#include "Shader.h"
#include "GameObject.h"
#include "GLError.h"

namespace Jasper {
using namespace std;

//Mesh::Mesh()
//{
//	Initialize();
//}

Mesh::Mesh(const std::string& name) : Component(name){
	Initialize();
	CalculateExtents();
}


Mesh::~Mesh()
{
	Destroy();
}

void Mesh::Initialize() {

}

void Mesh::Destroy() {
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
		unsigned index0 = Indices[i];
		unsigned index1 = Indices[i + 1];
		unsigned index2 = Indices[i + 2];
		//Vertex& v1 = Vertices[index0];
		//Vertex& v2 = Vertices[index1];
		//Vertex& v3 = Vertices[index2];
        
        auto& v1 = Positions[index0];
        auto& v2 = Positions[index1];
        auto& v3 = Positions[index2];
        
        auto& t1 = TexCoords[index0];
        auto& t2 = TexCoords[index1];
        auto& t3 = TexCoords[index2];

		// Tangent Space calculations adapted from
		// Lengyel, Eric. Computing Tangent Space Basis Vectors for an Arbitrary Mesh. 
        // Terathon Software 3D Graphics Library, 2001. http://www.terathon.com/code/tangent.html

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = t2.x - t1.x;
		float s2 = t3.x - t1.x;
		float tt1 = t2.y - t1.y;
		float tt2 = t3.y - t1.y;

		float r = 1.0F / (s1 * tt2 - s2 * tt1);
		Vector3 sdir((tt2 * x1 - tt1 * x2) * r, (tt2 * y1 - tt1 * y2) * r,
			(tt2 * z1 - tt1 * z2) * r);
		Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		Vector4 tangent = { sdir, 0.0f };
		Vector3 bitangent = tdir;
		
		Vector3 edge1 = v2 - v1;
		Vector3 edge2 = v3 - v1;

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
        
		//v1.Normal += normal;
		//v1.Tangent += tangent;		
		//v1.Bitangent += bitangent;
		//v2.Normal += normal;
		//v2.Tangent += tangent;
		//v2.Bitangent += bitangent;
		//v3.Normal += normal;
		//v3.Tangent += tangent;
		//v3.Bitangent += bitangent;
	}

	for (auto index : Indices) {
        auto& norm = Normals[index];
        auto& tan = Tangents[index];
        auto& bitan = Bitangents[index];
        
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
	float x = (xmax - xmin) / 2.f;
	float y = (ymax - ymin) / 2.f;
	float z = (zmax - zmin) / 2.f;
	m_Extents = Vector3(x, y, z);
	m_minExtents = Vector3(xmin, ymin, zmin);
	m_maxExtents = Vector3(xmax, ymax, zmax);

	/*float ox = (m_maxExtents.x + m_minExtents.x) / 2.0f;
	float oy = (m_maxExtents.y + m_minExtents.y) / 2.0f;
	float oz = (m_maxExtents.z + m_minExtents.z) / 2.0f;*/

	/*m_origin = Vector3(ox, oy, oz);
	float epsilon = 0.00001f;
	if (fabs(m_origin.x) > epsilon || fabs(m_origin.y) > epsilon || fabs(m_origin.z) > epsilon) {
		for (auto& v : Vertices) {
			v.Position -= m_origin;
		}		
		CalculateExtents();
	}*/

	
}

} // namespace Jasper