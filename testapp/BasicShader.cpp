#include "BasicShader.h"

using namespace std;

namespace Jasper
{

BasicShader::BasicShader() : Shader(string("Basic_Shader"))
{
    Initialize();
}

BasicShader::~BasicShader()
{
}

void BasicShader::Initialize()
{
    string vsFile = "../Shaders/basic_vert.glsl";
    string fsFile = "../Shaders/basic_fragment.glsl";

    AddShader(vsFile, ShaderType::VERTEX);
    AddShader(fsFile, ShaderType::FRAGMENT);

    LinkShaderProgram();
}

}
