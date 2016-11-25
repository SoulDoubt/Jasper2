#pragma once

#include "PhysicsCollider.h"

namespace Jasper
{
class ConvexHullCollider : public PhysicsCollider
{
public:
    explicit ConvexHullCollider(const std::string& name, const Vector3& halfExtents, PhysicsWorld* world);
    void Awake() override;
    ComponentType GetComponentType() const override {
        return ComponentType::ConvexHullCollider;
    }
};

}
