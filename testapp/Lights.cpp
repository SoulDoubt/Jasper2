#include "Lights.h"
#include "imgui.h"

namespace Jasper
{

PointLight::PointLight(const std::string& name)
    : GameObject(name)
{

}


PointLight::~PointLight()
{
}

bool PointLight::ShowGui()
{
    using namespace ImGui;
    GameObject::ShowGui();
    /*
    Vector3 Color = { 1.0f, 1.0f, 1.0f };
    float AmbientIntensity = 0.5f;
    float DiffuseIntensity = 1.0f;
    float ConstAtten = 1.0f;
    float LinearAtten = 0.09f;
    float ExpAtten = 0.032f;
    float Radius = 20.0f;
    */

    InputFloat3("Color", Color.AsFloatPtr());
    InputFloat("Ambient Intensity", &AmbientIntensity);
    InputFloat("Diffuse Intensity", &DiffuseIntensity);
    InputFloat("Radius", &Radius);
    return false;
}


DirectionalLight::DirectionalLight(const std::string & name)
    : GameObject(name)
{
}

DirectionalLight::~DirectionalLight()
{
}

bool DirectionalLight::ShowGui()
{
//    Vector3 Color = { 1.f, 1.f, 1.f };
//    Vector3 Direction = { 0.f, -1.f, 0.f };
//    float AmbientIntensity = 0.5f;
//    float Diffuseintensity = 1.0f;
    using namespace ImGui;
    
    InputFloat3("Color", Color.AsFloatPtr());
    InputFloat3("Direction", Direction.AsFloatPtr());
    InputFloat("Ambient Intensity", &AmbientIntensity);
    InputFloat("Diffuse Intensity", &Diffuseintensity);    
    
    return false;
}

}
