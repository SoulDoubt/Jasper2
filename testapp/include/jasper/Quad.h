#ifndef _QUAD_MESH_H_
#define _QUAD_MESH_H_

#include "Mesh.h"

namespace Jasper {

class Quad :
	public Mesh
{
public:

	enum class AxisAlignment {
		XY,
		YZ,
		XZ
	};

	Quad();
	explicit Quad(const Vector2& size, AxisAlignment align);
	Quad(const Vector2& size, int repeatU, int repeatV, AxisAlignment align);
	Quad(const Vector3& normal, const Vector3& point);
	~Quad();

	virtual void Initialize() override;
	virtual void Destroy() override;


private:
	Vector2 m_size;
	int m_repeatU;
	int m_repeatV;
	AxisAlignment m_alignment = AxisAlignment::XY;

};

}
#endif