#ifndef _CUBE_MESH_H_
#define _CUBE_MESH_H_

#include "Mesh.h"

namespace Jasper
{

class Cube : public Mesh
{
public:
    Cube(const std::string& name);
    explicit Cube(const std::string& name, const Vector3& dimensions);
    explicit Cube(const std::string& name, const Vector3& dimensions, float repeatU, float repeatV)
        : Mesh(name), Dimensions(dimensions), m_repeatU(repeatU), m_repeatV(repeatV) {
        Initialize();
    }
    Cube(const std::string& name, const Vector3& dimensions, bool reverseWinding);
    virtual ~Cube();

    void Initialize() override;
    void Destroy() override;
    ComponentType GetComponentType() override {
        return ComponentType::Cube;   
    }

    void SetTextureRepeat(float u, float v) {
        m_repeatU = u;
        m_repeatV = v;
    }

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
    float m_repeatU = 1.f;
    float m_repeatV = 1.f;
};
}
#endif // _CUBE_MESH_H_
