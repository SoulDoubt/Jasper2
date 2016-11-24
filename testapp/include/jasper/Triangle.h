#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "Mesh.h"

namespace Jasper
{

class Triangle : public Mesh
{
public:
    Triangle(const std::string& name);
    ~Triangle();

    void Initialize() override;
    void Destroy() override;
    ComponentType GetComponentType() override {
        return ComponentType::Triangle;
    }

};
}
#endif
