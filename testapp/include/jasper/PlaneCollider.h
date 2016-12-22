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
    PlaneCollider(std::string name, Vector3 normal, float constant, PhysicsWorld* world);
    ~PlaneCollider();
	void Serialize(std::ofstream& ofs) const override;

    void Awake() override;

	Vector3 Normal;
	float Constant;
    
};

}

#endif //_JASPER_PLANE_COLLIDER_H_
