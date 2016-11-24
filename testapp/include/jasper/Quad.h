#ifndef _QUAD_MESH_H_
#define _QUAD_MESH_H_

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
    ComponentType GetComponentType() override {
        return ComponentType::Quad;
    }

    void Initialize() override;
    void Destroy() override;


private:
    Vector2 m_size;
    int m_repeatU;
    int m_repeatV;
    AxisAlignment m_alignment = AxisAlignment::XY;

};

}
#endif
