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

    unsigned m_latLines = 16;
    unsigned m_longLines = 16;
    float m_radius;

    static float icosPositions[];
    static float icosTexCoords[];
    static uint32_t icosIndices[];

    void Subdivide(int subdivisions);
    void RecalcTexCoords();
    void LoadIcosphere();
    void LoadUVSphere();
};


class Cylinder : public Mesh
{
private:

    float m_height;
    float m_radius;    
    float m_heightSubdivisions = 24.f;
    float m_axisSubdivisions = 24.f;
    float m_capSubdivisions = 6.f;
    float m_radiusBase = 1.f;
    float m_radiusApex = 1.f;
    int m_segments = 24;
    int m_slices = 3;
    Vector3 m_direction = {0.f, 1.f, 0.f};    
    void LoadVerts();
    void GenerateCaps(bool flip, float height);

public:

    Cylinder(const std::string& name, float height, float radius);
    ~Cylinder();
    void Initialize() override;
};

}

#endif // _JASPER_SPHERE_H_
