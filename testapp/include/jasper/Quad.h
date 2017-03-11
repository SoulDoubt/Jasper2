#pragma once

#include "Mesh.h"

namespace Jasper
{

class Quad :
	public Mesh
{
public:

	enum class AxisAlignment
	{
		XY,
		YZ,
		XZ
	};

	Quad(const std::string& name);
	explicit Quad(const std::string& name, const Vector2& size, AxisAlignment align);
	Quad(const std::string& name, const Vector2& size, int repeatU, int repeatV, AxisAlignment align);
	Quad(const std::string& name, const Vector3& normal, const Vector3& point);
	~Quad();

	/*ComponentType GetComponentType() const override {
		return ComponentType::Mesh;
	}*/

	MeshType GetMeshType() const override {
		return MeshType::Quad;
	}

	void Serialize(std::ofstream& ofs) const override;
	void Initialize() override;
	void Destroy() override;

private:

	Vector2 m_size;
	float m_repeatU = 1.0f;
	float m_repeatV = 1.0f;
	AxisAlignment m_alignment = AxisAlignment::XY;
};

}

