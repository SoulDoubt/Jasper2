#include "Scriptable.h"
#include "PhysicsCollider.h"

namespace Jasper
{

void RotateInPlaceScript::Update(double dt)
{
    auto go = GetGameObject();
    Transform& t = go->GetLocalTransform();
    t.Rotate(m_axis, dt * DEG_TO_RAD(m_degreesPerSecond));
    auto collider = go->GetComponentByType<PhysicsCollider>();
    if (collider) {
        btTransform btt;
        btt.setRotation(t.Orientation.AsBtQuaternion());
        btt.setOrigin(t.Position.AsBtVector3());
        collider->SetWorldTransform(btt);
    }
}

}
