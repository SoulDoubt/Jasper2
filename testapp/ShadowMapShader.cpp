#include "ShadowMapShader.h"
#include <string>

namespace Jasper
{

using namespace std;

ShadowMapShader::ShadowMapShader() : Shader("Shadow_map_shader")
{

}

void ShadowMapShader::Initialize()
{

    string vsFile = ".\\Shaders\\shadow_map_vert.glsl";
    string fsFile = ".\\Shaders\\shadow_map_frag.glsl";

    AddShader(vsFile, ShaderType::VERTEX);
    AddShader(fsFile, ShaderType::FRAGMENT);

    LinkShaderProgram();
}

} // namespace Jasper
