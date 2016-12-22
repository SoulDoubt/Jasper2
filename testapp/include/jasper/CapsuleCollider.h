#pragma once

#include "PhysicsCollider.h"

namespace Jasper
{

class CapsuleCollider : public PhysicsCollider
{

public:

    //CapsuleCollider(std::string name, Mesh* mesh, PhysicsWorld* world);
    CapsuleCollider(std::string name, const Vector3& halfExtents, PhysicsWorld* world);
    ~CapsuleCollider();

    void Awake() override;


};

} // namespace Jasper

