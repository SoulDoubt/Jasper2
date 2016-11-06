#pragma once

#include "PhysicsCollider.h"

namespace Jasper
{
class CylinderCollider : public PhysicsCollider
{
public:
    explicit CylinderCollider(const std::string& name, const Vector3& halfExtents, PhysicsWorld* world);
    virtual void Awake() override;
};

}
