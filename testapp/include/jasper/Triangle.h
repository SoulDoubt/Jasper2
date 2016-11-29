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
    ComponentType GetComponentType() const override {
        return ComponentType::Triangle;
    }

};
}
#endif
