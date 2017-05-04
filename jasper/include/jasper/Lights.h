#pragma once

#include "GameObject.h"

namespace Jasper
{

class PointLight final : public GameObject
{
public:

	explicit PointLight(const std::string& name);
	~PointLight();

	Vector3 Color = { 1.0f, 1.0f, 1.0f };
	float AmbientIntensity = 0.05f;
	float DiffuseIntensity = 1.0f;
	float Attenuation = 1.0f;	
	
	float Radius = 20.0f;

	bool ShowGui() override;

	GameObjectType GetGameObjectType() const override {
		return GameObjectType::PointLight;
	}
    
    Vector3 Position() const {
        return GetWorldTransform().Position;
    }
    
    
};

class DirectionalLight final : public GameObject
{

public:
	Vector3 Color = { 1.f, 1.f, 1.f };
	Vector3 Direction = { 0.f, -1.f, 0.f };
	float AmbientIntensity = 0.5f;
	float Diffuseintensity = 1.0f;

	explicit DirectionalLight(const std::string& name);
	~DirectionalLight();

	GameObjectType GetGameObjectType() const override {
		return GameObjectType::DirectionalLight;
	}

	bool ShowGui() override;

};


} // namespace Jasper

