#ifndef _JASPER_LIGHTS_H_
#define _JASPER_LIGHTS_H_

#include "GameObject.h"

namespace Jasper {

class PointLight : public GameObject
{
public:

	explicit PointLight(const std::string& name);
	~PointLight();

	Vector3 Color = { 1.0f, 1.0f, 1.0f };
	float AmbientIntensity = 0.5f;
	float DiffuseIntensity = 1.0f;
	float ConstAtten = 0.1f;
	float LinearAtten = 0.1f;
	float ExpAtten = 0.1f;
	float Radius = 20.0f;
};

class DirectionalLight : public GameObject {

public:
	Vector3 Color = { 1.f, 1.f, 1.f };
	Vector3 Direction = { 0.f, -1.f, 0.f };
	float AmbientIntensity = 0.5f;
	float Diffuseintensity = 1.0f;

	explicit DirectionalLight(const std::string& name);
	~DirectionalLight();

};


} // namespace Jasper

#endif // _POINT_LIGHT_H_