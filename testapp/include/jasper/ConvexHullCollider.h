#pragma once

#include "PhysicsCollider.h"

namespace Jasper {
class ConvexHullCollider : public PhysicsCollider
{
public:
	explicit ConvexHullCollider(const std::string& name, const Vector3& halfExtents, PhysicsWorld* world);
	virtual void Awake() override;
};

}
