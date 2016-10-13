#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "Mesh.h"

namespace Jasper {

class Triangle : public Mesh
{
public:
	Triangle();
	~Triangle();

	virtual void Initialize() override;
	virtual void Destroy() override;

};
}
#endif