#pragma once

#include "Shader.h"

namespace Jasper
{
class ShadowMapShader : public Shader
{

public:
    ShadowMapShader();
    ~ShadowMapShader();

    void Initialize() override;

};

}
