#include "FontShader.h"

namespace Jasper
{

using namespace std;

FontShader::FontShader() : Shader("Font_Shader")
{
    Initialize();
}


FontShader::~FontShader()
{
    Shader::Destroy();
}

void FontShader::Initialize()
{
    const string vsFile = "../Shaders/font_vert.glsl";
    const string fsFile = "../Shaders/font_frag.glsl";

    AddShader(vsFile, ShaderType::VERTEX);
    AddShader(fsFile, ShaderType::FRAGMENT);

    LinkShaderProgram();
}

}
