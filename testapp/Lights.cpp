#include "Lights.h"

namespace Jasper
{

PointLight::PointLight(const std::string& name)
    : GameObject(name)
{

}


PointLight::~PointLight()
{
}


DirectionalLight::DirectionalLight(const std::string & name)
    : GameObject(name)
{
}

DirectionalLight::~DirectionalLight()
{
}



}
