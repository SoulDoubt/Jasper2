#ifndef _JASPER_BOX_COLLIDER_H_
#define _JASPER_BOX_COLLIDER_H_

#include "PhysicsCollider.h"

namespace Jasper
{

class BoxCollider :
    public PhysicsCollider
{
public:
    //explicit BoxCollider(std::string name, Mesh* mesh, PhysicsWorld* world);
    explicit BoxCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world);
    ~BoxCollider();

    void Awake() override;
    
    
    void Serialize(std::ofstream& ofs){
        PhysicsCollider::Serialize(ofs);
    }
};

}

# endif // _JASPER_PHYSICS_COLLIDER_H_
