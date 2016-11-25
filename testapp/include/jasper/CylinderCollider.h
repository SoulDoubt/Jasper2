#pragma once

#include "PhysicsCollider.h"

namespace Jasper
{
class CylinderCollider : public PhysicsCollider
{
public:
    explicit CylinderCollider(const std::string& name, const Vector3& halfExtents, PhysicsWorld* world);
    void Awake() override;
    ComponentType GetComponentType() const override {
        return ComponentType::CylinderCollider;
    }
};

}
