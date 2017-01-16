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

    void Initialize() override;
    void Destroy() override;

    ComponentType GetComponentType() const override {
        return ComponentType::Mesh;
    }

	MeshType GetMeshType() const override {
		return MeshType::Sphere;
	}

    unsigned m_latLines = 24;
    unsigned m_longLines = 24;
    float m_radius;
    
    static float icosPositions[];
    static float icosTexCoords[];
    static uint32_t icosIndices[];
    
    void Subdivide(int subdivisions);
    void RecalcTexCoords();
    void LoadIcosphere();
    void LoadUVSphere();
};

}

#endif // _JASPER_SPHERE_H_
