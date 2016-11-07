#include "SkyboxShader.h"

namespace Jasper
{

SkyboxShader::SkyboxShader(std::string name) : Shader(name)
{
    Initialize();
}


SkyboxShader::~SkyboxShader()
{
}

void SkyboxShader::Initialize()
{
    using namespace std;

    string vsFile = "../Shaders/cubemap_vert.glsl";
    string fsFile = "../Shaders/cubemap_frag.glsl";

    AddShader(vsFile, ShaderType::VERTEX);
    AddShader(fsFile, ShaderType::FRAGMENT);

    LinkShaderProgram();
}

void SkyboxShader::SetProjectionMatrix(const Matrix4 & proj)
{
    int location = glGetUniformLocation(ProgramID(), "projectionMatrix");
    glUniformMatrix4fv(location, 1, GL_TRUE, proj.AsFloatPtr());
}

void SkyboxShader::SetViewMatrix(const Matrix4 & view)
{
    int location = glGetUniformLocation(ProgramID(), "viewMatrix");
    glUniformMatrix4fv(location, 1, GL_TRUE, view.AsFloatPtr());
}

}
