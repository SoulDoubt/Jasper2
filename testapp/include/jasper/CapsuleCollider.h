#ifndef _JASPER_CAPSULE_COLLIDER_H_
#define _JASPER_CAPSULE_COLLIDER_H_

#include "PhysicsCollider.h"

namespace Jasper
{

class CapsuleCollider : public PhysicsCollider
{

public:

    explicit CapsuleCollider(const std::string& name, Mesh* mesh, PhysicsWorld* world);
    explicit CapsuleCollider(const std::string& name, const Vector3& halfExtents, PhysicsWorld* world);
    ~CapsuleCollider();

    void Awake() override;
    
};

} // namespace Jasper

#endif // _JASPER_CAPSULE_COLLIDER_H_
