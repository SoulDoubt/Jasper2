#include "Sphere.h"

namespace Jasper {

#define PI 3.14159265359	


Sphere::Sphere(const std::string& name, float radius) : Mesh(name), m_radius(radius)
{
	Initialize();
}

Sphere::~Sphere()
{
}

void Sphere::Initialize()
{
	for (unsigned i = 0; i < m_latLines; ++i) {
		float v = (float)i / (float)m_latLines;
		float phi = v * PI;
		for (unsigned j = 0; j < m_longLines; ++j) {
			float u = (float)j / (float)m_longLines;
			float theta = u * PI * 2;
			float x = cosf(theta) * sinf(phi);
			float y = cosf(phi);
			float z = sinf(theta) * sinf(phi);
			Vector3 position = Vector3(x, y, z) * m_radius;
			Vertex v = Vertex(position);
			AddVertex(v);
		}
	}

	for (unsigned i = 0; i < m_longLines * m_latLines + m_longLines; ++i) {
		Indices.push_back(i);
		Indices.push_back(i + m_longLines + 1);
		Indices.push_back(i + m_longLines);

		Indices.push_back(i + m_longLines + 1);
		Indices.push_back(i);
		Indices.push_back(i + 1);
	}

	//CalculateFaceNormals();
	CalculateExtents();

}

void Sphere::Destroy()
{
}

}