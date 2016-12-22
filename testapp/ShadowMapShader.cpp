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

    const string vsFile = ".\\Shaders\\shadow_map_vert.glsl";
    const string fsFile = ".\\Shaders\\shadow_map_frag.glsl";

    AddShader(vsFile, ShaderType::VERTEX);
    AddShader(fsFile, ShaderType::FRAGMENT);

    LinkShaderProgram();
}

} // namespace Jasper
