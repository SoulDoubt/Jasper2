#include "Quad.h"

namespace Jasper {

	Quad::Quad(const std::string& name) : Mesh(name)
	{
		m_size = { 0.5f, 0.5f };
		Initialize();
		FlipTextureCoords();
	}

	Quad::Quad(const std::string& name, const Vector2& size, Quad::AxisAlignment align) : Mesh(name)
	{
		m_alignment = align;
		m_size = size;
		m_repeatU = 1;
		m_repeatV = 1;
		Initialize();
		FlipTextureCoords();
	}

	Quad::Quad(const std::string& name, const Vector2& size, int repeatU, int repeatV, Quad::AxisAlignment align) :Mesh(name)
	{
		m_alignment = align;
		m_size = size;
		m_repeatU = repeatU;
		m_repeatV = repeatV;
		Initialize();
		FlipTextureCoords();
	}

	Quad::Quad(const std::string& name, const Vector3 & normal, const Vector3 & point) :Mesh(name)
	{

	}


	Quad::~Quad()
	{
		Destroy();
	}

	void Quad::Initialize() {

		float x = m_size.x;
		float y = m_size.y;		

		Vertex v0, v1, v2, v3;

		switch (m_alignment) {
		case AxisAlignment::XY:
			v0 = { -x, -y, 0.0f, 0.0f, 0.0f };
			v1 = { -x, y, 0.0f, 0.0f, 1.0f * (float)m_repeatV };
			v2 = { x, y, 0.0f, 1.0f * (float)m_repeatU, 1.0f * (float)m_repeatV };
			v3 = { x, -y, 0.0f, 1.0f * (float)m_repeatU, 0.0f };
			AddVertex(v0);
			AddVertex(v1);
			AddVertex(v2);
			AddVertex(v3);
			Indices.push_back(3);
			Indices.push_back(2);
			Indices.push_back(1);
			Indices.push_back(1);
			Indices.push_back(0);
			Indices.push_back(3);
			break;
		case AxisAlignment::XZ:
			v0 = { -x, 0.f, -y, 0.0f, 0.0f };
			v1 = { -x, 0.f, y, 0.0f, 1.0f * (float)m_repeatV };
			v2 = { x, 0.f, y, 1.0f * (float)m_repeatU, 1.0f * (float)m_repeatV };
			v3 = { x, 0.f, -y, 1.0f * (float)m_repeatU, 0.0f };
			AddVertex(v0);
			AddVertex(v1);
			AddVertex(v2);
			AddVertex(v3);
			Indices.push_back(2);
			Indices.push_back(3);
			Indices.push_back(0);
			Indices.push_back(0);
			Indices.push_back(1);
			Indices.push_back(2);

			break;

		case AxisAlignment::YZ:
			v0 = { 0.f, -x, -y, 0.0f, 0.0f };
			v1 = { 0.f, -x, y, 0.0f, 1.0f * (float)m_repeatV };
			v2 = { 0.f, x, y, 1.0f * (float)m_repeatU, 1.0f * (float)m_repeatV };
			v3 = { 0.f, x, -y, 1.0f * (float)m_repeatU, 0.0f };
			AddVertex(v0);
			AddVertex(v1);
			AddVertex(v2);
			AddVertex(v3);
			Indices.push_back(3);
			Indices.push_back(2);
			Indices.push_back(1);
			Indices.push_back(1);
			Indices.push_back(0);
			Indices.push_back(3);
			break;
		}

		CalculateFaceNormals();
		if (m_reverseWinding) {
			ReverseWinding();
		}
	}

	void Quad::Destroy() {}

}