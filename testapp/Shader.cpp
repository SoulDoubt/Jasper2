#include "Shader.h"
#include "Material.h"
#include "GLError.h"
#include "imgui.h"

namespace Jasper
{
using namespace std;

Shader::Shader(): m_name("unamed_shader")
{    
    m_transpose = true;
    Initialize();
}

Shader::Shader(std::string name)
{
    m_transpose = true;
    m_name = name;
    Initialize();
}


Shader::~Shader()
{
    Destroy();
}

void Shader::PrintAttribsAndUniforms()
{
    GLint i;
    GLint count;

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 24; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length
    printf("%s\n", m_name.c_str());
    glGetProgramiv(m_programID, GL_ACTIVE_ATTRIBUTES, &count);
    printf("Active Attributes: %d\n", count);

    for (i = 0; i < count; i++) {
        glGetActiveAttrib(m_programID, (GLuint)i, bufSize, &length, &size, &type, name);

        printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
    }

    glGetProgramiv(m_programID, GL_ACTIVE_UNIFORMS, &count);
    printf("Active Uniforms: %d\n", count);

    for (i = 0; i < count; i++) {
        glGetActiveUniform(m_programID, (GLuint)i, bufSize, &length, &size, &type, name);

        printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
    }
}

void Shader::Initialize()
{
    const string msg = "Initializing " + m_name + " shader...\n";
    printf("%s\n", msg.c_str());
    m_programID = 0;
    m_programID = glCreateProgram();
    printf("ProgramID is: %d", m_programID);
}

bool Shader::Bind()
{
    if (!m_programID) {
        return false;
    }
    glUseProgram(m_programID);
    return true;
}

void Shader::Release()
{
    glUseProgram(0);
}

int Shader::GetAttributeLocation(const char* name)
{
    return glGetAttribLocation(m_programID, name);
}

void Shader::SetAttributeArray(const char* name, GLenum type, const void* values, int tupleSize, int stride)
{
    const int location = GetAttributeLocation(name);
    if (location > -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, tupleSize, type, GL_TRUE, stride, values);
    }
}

void Shader::SetAttributeArray(int location, GLenum num_type, void * offset, int num_components, int stride)
{
    if (location > -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, num_components, num_type, GL_FALSE, stride, offset);
    }
}

void Shader::SetAttributeArray(int location, GLenum num_type, void* offset, int num_components, int stride, bool normalize){
    if (location > -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, num_components, num_type, normalize, stride, offset);
    }
}

void Shader::AddShader(std::string filename, ShaderType t)
{

    FILE* file;
    file = fopen(filename.c_str(), "r");
    if (file) {
        char* file_contents;
        long file_size;
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);
        file_contents = (char*)calloc((file_size + 1), sizeof(char));
        fread(file_contents, sizeof(char), file_size, file);
        fclose(file);
        file_contents[file_size] = 0; // null terminate the string
        //printf(file_contents);
        const char* source = file_contents;
        //printf_s("%s\n,", c_source);
        std::string logType = "";
        GLenum st = GL_VERTEX_SHADER;
        switch (t) {
        case VERTEX:
            st = GL_VERTEX_SHADER;
            logType = "VERTEX";
            break;
        case FRAGMENT:
            st = GL_FRAGMENT_SHADER;
            logType = "FRAGMENT";
            break;
        case GEOMETRY:
            st = GL_GEOMETRY_SHADER;
            logType = "GEOMETRY";
        }

        const GLuint shaderID = glCreateShader(st);
        glShaderSource(shaderID, 1, &source, NULL);
        free(file_contents);
        glCompileShader(shaderID);

        GLint status;
        std::string info;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint logLength;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
            GLchar* log = new GLchar[logLength + 1];
            glGetShaderInfoLog(shaderID, logLength, NULL, log);
            printf("%s\n", log);
            delete[] log;
        }
        printf("%s: %s shader compiled successfully\n", m_name.c_str(), logType.c_str());
        m_shaders.push_back(shaderID);
    }

}

void Shader::LinkShaderProgram()
{
    if (m_programID < 1) {
        //throw std::exception("No Shader Program created while linking shaders...\n");
    }
    for (auto shader : m_shaders) {
        glAttachShader(m_programID, shader);
    }

    glLinkProgram(m_programID);
    GLint status;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength = 256;
        //glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(m_programID, infoLogLength, NULL, strInfoLog);
        printf("Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    } else {
        printf("Shader Program linked successfully.\n");
    }

    for (auto shader : m_shaders) {
        glDetachShader(m_programID, shader);
        glDeleteShader(shader);
    }
    m_shaders.clear();

    //PrintAttribsAndUniforms();
}

void Shader::Destroy()
{
    for (auto sh : m_shaders) {
        glDetachShader(m_programID, sh);
        glDeleteShader(sh);
    }
    if (m_programID > 0)
        glDeleteProgram(m_programID);
}


unsigned Shader::PositionAttributeLocation()
{
    if (m_positionsAttribute > 0) {
        return m_positionsAttribute;
    } else {
        m_positionsAttribute = glGetAttribLocation(m_programID, "position");
        return m_positionsAttribute;
    }
}

unsigned Shader::NormalAttributeLocation()
{
    if (m_normalsAttribute > 0) {
        return m_normalsAttribute;
    } else {
        m_normalsAttribute = glGetAttribLocation(m_programID, "normal");
        return m_normalsAttribute;
    }
}

unsigned Shader::TexCoordAttributeLocation()
{
    if (m_texCoordsAttribute > 0) {
        return m_texCoordsAttribute;
    } else {
        m_texCoordsAttribute = glGetAttribLocation(ProgramID(), "texCoords");
        return m_texCoordsAttribute;
    }
}

uint Shader::ColorsAttributeLocation()
{
    if (m_colorsAttribute > 0) {
        return m_colorsAttribute;
    } else {
        m_colorsAttribute = glGetAttribLocation(ProgramID(), "color");
        return m_colorsAttribute;
    }
}

int Shader::TangentAttributeLocation()
{
    if (m_tangentAttribute > 0) {
        return m_tangentAttribute;
    } else {
        m_tangentAttribute = glGetAttribLocation(m_programID, "tangent");
        return m_tangentAttribute;
    }
}


inline void Shader::SetModelViewMatrix(const Matrix4 & mvm)
{
    //GLERRORCHECK;
    const int loc = glGetUniformLocation(m_programID, "mvMatrix");
    if (loc > -1)
        glUniformMatrix4fv(loc, 1, m_transpose, mvm.AsFloatPtr());
    //GLERRORCHECK;
}

inline void Shader::SetModelViewProjectionMatrix(const Matrix4 & mvp)
{
    //GLERRORCHECK;
    const int loc = glGetUniformLocation(m_programID, "mvpMatrix");
    if (loc > -1)
        glUniformMatrix4fv(loc, 1, m_transpose, mvp.AsFloatPtr());
    //GL/ERRORCHECK;
}

inline void Shader::SetNormalMatrix(const Matrix3 & normal)
{
    //GLERRORCHECK;
    const int loc = glGetUniformLocation(m_programID, "normalMatrix");
    if (loc > -1)
        glUniformMatrix3fv(loc, 1, m_transpose, normal.AsFloatPtr());
    //GLERRORCHECK;
}

inline void Shader::SetModelMatrix(const Matrix4& model)
{
    //GLERRORCHECK;
    const int location = glGetUniformLocation(m_programID, "modelMatrix");
    if (location > -1)
        glUniformMatrix4fv(location, 1, m_transpose, model.AsFloatPtr());
    //GLERRORCHECK;
}

inline void Shader::SetViewMatrix(const Matrix4 & view)
{
    //GLERRORCHECK;
    const int loc = glGetUniformLocation(m_programID, "viewMatrix");
    if (loc > -1) {
        glUniformMatrix4fv(loc, 1, m_transpose, view.AsFloatPtr());
    }
    //GLERRORCHECK;
}

void Shader::GetDirectionalLightUniformLocations()
{

    /*GLuint id = ProgramID();
    dlul.AmbientIntensity = glGetUniformLocation(id, "light0.AmbientIntensity");
    dlul.Color = glGetUniformLocation(id, "light0.Color");
    dlul.ConstAtten = glGetUniformLocation(id, "light0.ConstAtten");
    dlul.DiffuseIntensity = glGetUniformLocation(id, "light0.DiffuseIntensity");
    dlul.Direction = glGetUniformLocation(id, "light0.Direction");
    dlul.Exp = glGetUniformLocation(id, "light0.Exp");
    dlul.LinearAtten = glGetUniformLocation(id, "light0.LinearAtten");
    dlul.Position = glGetUniformLocation(id, "light0.Position");*/

    //return nullptr;
}

void Shader::SetDirectionalLightUniforms(const DirectionalLight* dl)
{
    /*auto ul = GetDirectionalLightUniformLocations();
    glUniform3fv(ul.Color, 1, dl->Color.AsFloatPtr());
    glUniform3fv(ul.Direction, 1, dl->Direction.AsFloatPtr());
    glUniform3fv(ul.Position, 1, eyeSpacePosition.AsFloatPtr());
    glUniform1fv(ul.AmbientIntensity, 1, &dl->AmbientIntensity);
    glUniform1fv(ul.DiffuseIntensity, 1, &dl->DiffuseIntensity);
    glUniform1fv(ul.ConstAtten, 1, &dl->ConstAtten);
    glUniform1fv(ul.LinearAtten, 1, &dl->LinearAtten);
    glUniform1fv(ul.Exp, 1, &dl->Exp);*/
}

void Shader::GetPointLightUniformLocations()
{

}

void Shader::SetPointLightUniforms(const PointLight* pl, const Vector3& eslp)
{
    /*const auto position = pl->GetWorldTransform().Position;
    auto ul = GetDirectionalLightUniformLocations();

    glUniform3fv(ul.Color, 1, pl->Color.AsFloatPtr());
    glUniform3fv(ul.Position, 1, position.AsFloatPtr());
    glUniform1fv(ul.AmbientIntensity, 1, &pl->AmbientIntensity);
    glUniform1fv(ul.ConstAtten, 1, &pl->ConstAtten);*/
}

void Shader::GetMaterialUniformLocations()
{
    //GLuint id = ProgramID();
    //auto mul = MaterialUniformLocations();
    //mul.Ka = glGetUniformLocation(id, "material0.ka");
    //mul.Kd = glGetUniformLocation(id, "material0.kd");
    //mul.Ks = glGetUniformLocation(id, "material0.ks");
    //mul.Ns = glGetUniformLocation(id, "material0.ns");
    //return mul;
}

void Shader::SetMaterialUniforms(const Material* m)
{
    /*auto ul = GetMaterialUniformLocations();
    glUniform3fv(ul.Ka, 1, m->Ambient.AsFloatPtr());
    glUniform3fv(ul.Kd, 1, m->Diffuse.AsFloatPtr());
    glUniform3fv(ul.Ks, 1, m->Specular.AsFloatPtr());
    glUniform1fv(ul.Ns, 1, &(m->Shine));*/
}

void Shader::SetCameraPosition(const Vector3& cp)
{
    //GLERRORCHECK;
    const int loc = glGetUniformLocation(ProgramID(), "cameraPosition");
    if (loc > -1)
        glUniform3fv(loc, 1, cp.AsFloatPtr());
    //GLERRORCHECK;
}

void Shader::SetTransformUniforms(const Transform & trans)
{
    const uint id = ProgramID();
    Quaternion q = trans.Orientation;
    uint positionLoc = glGetUniformLocation(id, "transform.position");
    uint orientationLoc = glGetUniformLocation(id, "transform.orientation");
    uint scaleLoc = glGetUniformLocation(id, "transform.scale");

    glUniform3fv(positionLoc, 1, trans.Position.AsFloatPtr());
    glUniform4fv(orientationLoc, 1, q.AsFloatPtr());
    glUniform3fv(scaleLoc, 1, trans.Scale.AsFloatPtr());

}

bool Shader::ShowGui(){
    static char name_buffer[] = "";
    return false;
    
}
}
