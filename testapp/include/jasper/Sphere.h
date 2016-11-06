#ifndef _JASPER_SPHERE_H_
#define _JASPER_SPHERE_H_

#include "Mesh.h"

namespace Jasper
{

class Sphere :
    public Mesh
{
public:
    explicit Sphere(const std::string& name, float radius);
    ~Sphere();

    virtual void Initialize() override;
    virtual void Destroy() override;

    unsigned m_latLines = 24;
    unsigned m_longLines = 24;
    float m_radius;
};

}

#endif // _JASPER_SPHERE_H_
