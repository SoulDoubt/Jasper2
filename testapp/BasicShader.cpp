#include "BasicShader.h"


using namespace std;

namespace Jasper
{

BasicShader::BasicShader() : Shader("basic_shader"s)
{
    Initialize();
}

BasicShader::~BasicShader()
{
}

void BasicShader::Initialize()
{
    const string vsFile = "../Shaders/basic_vert.glsl"s;
    const string fsFile = "../Shaders/basic_fragment.glsl"s;

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
