#ifndef _CUBE_MESH_H_
#define _CUBE_MESH_H_

#include "Mesh.h"

namespace Jasper {

class Cube : public Mesh
{
public:
	Cube();
	explicit Cube(const Vector3& dimensions);
	Cube(const Vector3& dimensions, bool reverseWinding);
	virtual ~Cube();

	virtual void Initialize() override;
	virtual void Destroy() override;

	void LoadCubemapVerts();
	void LoadStandardVerts();

	Vector3 Dimensions;

	void SetCubemap(bool cm) {
		m_isCubemap = cm;
	}

	bool IsCubemap() const {
		return m_isCubemap;
	}


protected:
	bool m_isCubemap = false;
};
}
#endif // _CUBE_MESH_H_

