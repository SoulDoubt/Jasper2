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
    const string vsFile = "../Shaders/basic_vert.glsl";
    const string fsFile = "../Shaders/basic_fragment.glsl";

    AddShader(vsFile, ShaderType::VERTEX);
    AddShader(fsFile, ShaderType::FRAGMENT);

    LinkShaderProgram();
}

void BasicShader::SetColor(Vector4 color)
{
	int loc = glGetUniformLocation(this->ProgramID(), "vertColor");
	glUniform4f(loc, color.x, color.y, color.z, color.w);

}

}
