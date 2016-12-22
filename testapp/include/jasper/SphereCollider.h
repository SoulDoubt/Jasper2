#ifndef _JASPER_SPHERE_COLLIDER_H_
#define _JASPER_SPHERE_COLLIDER_H_

#include "PhysicsCollider.h"

namespace Jasper
{

class SphereCollider :
    public PhysicsCollider
{
public:
    //SphereCollider(std::string name, Mesh* mesh, PhysicsWorld* world);
    SphereCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world);
    ~SphereCollider();

    void Awake() override;
//    ComponentType GetComponentType() const override{
//        return ComponentType::PhysicsCollider;
//    }
};

}

#endif //_JASPER_SPHERE_COLLIDER_H_
