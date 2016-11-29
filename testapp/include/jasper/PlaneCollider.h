#ifndef _JASPER_PLANE_COLLIDER_H_
#define _JASPER_PLANE_COLLIDER_H_

#include "PhysicsCollider.h"
#include "imgui.h"

namespace Jasper
{

class PlaneCollider :
    public PhysicsCollider
{
public:
    PlaneCollider(std::string name, Mesh* mesh, PhysicsWorld* world);
    ~PlaneCollider();
    void Serialize(std::ofstream& ofs) const override {
        PhysicsCollider::Serialize(ofs);
    }

    void Awake() override;
    
};

}

#endif //_JASPER_PLANE_COLLIDER_H_
