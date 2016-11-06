#pragma once

#include "Shader.h"

namespace Jasper
{

using namespace std;

class GuiShader : public Shader
{
    void Initialize() override;
};

inline void GuiShader::Initialize()
{
    string vsFile = "../Shaders/texture_vert.glsl";
    string fsFile = "../Shaders/texture_fragment.glsl";

    AddShader(vsFile, ShaderType::VERTEX);
    AddShader(fsFile, ShaderType::FRAGMENT);

    LinkShaderProgram();
}

}
