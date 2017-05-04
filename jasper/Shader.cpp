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
    const string msg = "Initializing " + m_name + " shader...";
    printf("%s\n", msg.c_str());
    m_programID = 0;
    m_programID = glCreateProgram();
    printf("ProgramID is: %d\n", m_programID);
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
        if (type == GL_INT) {
            glVertexAttribIPointer(location, tupleSize, type, stride, values);
        } else {
            glVertexAttribPointer(location, tupleSize, type, GL_TRUE, stride, values);
        }
    }
}

void Shader::SetAttributeArray(int location, GLenum num_type, void * offset, int num_components, int stride)
{
    if (location > -1) {
        glEnableVertexAttribArray(location);
        if (num_type == GL_INT) {
            glVertexAttribIPointer(location, num_components, num_type, stride, offset);
        } else {
            glVertexAttribPointer(location, num_components, num_type, GL_FALSE, stride, offset);
        }
    }
}

void Shader::SetAttributeArray(int location, GLenum num_type, void* offset, int num_components, int stride, bool normalize)
{
    if (location > -1) {
        glEnableVertexAttribArray(location);
        if (num_type == GL_INT) {
            glVertexAttribIPointer(location, num_components, num_type, stride, offset);
        } else {
            glVertexAttribPointer(location, num_components, num_type, normalize, stride, offset);
        }
    }
}

int Shader::GetBoneWeightAttributeLocation()
{
    return glGetAttribLocation(m_programID, "boneWeights");
}

void Shader::GetBoneTransformUniformLocations()
{
    for (int i = 0; i < MAX_BONES; ++i) {
        string name = "boneTransforms[" + std::to_string(i) + "]";
        int l = glGetUniformLocation(m_programID, name.data());
        boneTransformUniformLocations.push_back(l);
    }
}

void Shader::SetBoneTransform(int index, const Matrix4& transform)
{
    //string name = "boneTrsnaforms[" + std::to_string(index) + "]";
	if (index < boneTransformUniformLocations.size()) {
		int location = boneTransformUniformLocations[index];
		glUniformMatrix4fv(location, 1, true, transform.AsFloatPtr());
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

    glUseProgram(m_programID);
    CacheUniformLocations();
    glUseProgram(0);
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


int Shader::PositionAttributeLocation()
{
    if (m_positionsAttribute > -1) {
        return m_positionsAttribute;
    } else {
        m_positionsAttribute = glGetAttribLocation(m_programID, "position");
        return m_positionsAttribute;
    }
}

int Shader::NormalAttributeLocation()
{
    if (m_normalsAttribute > -1) {
        return m_normalsAttribute;
    } else {
        m_normalsAttribute = glGetAttribLocation(m_programID, "normal");
        return m_normalsAttribute;
    }
}

int Shader::TexCoordAttributeLocation()
{
    if (m_texCoordsAttribute > -1) {
        return m_texCoordsAttribute;
    } else {
        m_texCoordsAttribute = glGetAttribLocation(ProgramID(), "texCoords");
        return m_texCoordsAttribute;
    }
}

int Shader::ColorsAttributeLocation()
{
    if (m_colorsAttribute > -1) {
        return m_colorsAttribute;
    } else {
        m_colorsAttribute = glGetAttribLocation(ProgramID(), "color");
        return m_colorsAttribute;
    }
}

int Shader::TangentAttributeLocation()
{
    if (m_tangentAttribute > -1) {
        return m_tangentAttribute;
    } else {
        m_tangentAttribute = glGetAttribLocation(m_programID, "tangent");
        return m_tangentAttribute;
    }
}



int Shader::BitangentAttributeLocation()
{
    if (m_bitangentAttribute > -1) {
        return m_bitangentAttribute;
    } else {
        m_bitangentAttribute = glGetAttribLocation(m_programID, "bitangent");
        return m_bitangentAttribute;
    }
}


inline void Shader::SetModelViewMatrix(const Matrix4 & mvm)
{
    const int loc = glGetUniformLocation(m_programID, "mvMatrix");
    if (loc > -1)
        glUniformMatrix4fv(loc, 1, m_transpose, mvm.AsFloatPtr());
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

}

void Shader::SetDirectionalLightUniforms(const DirectionalLight* dl)
{

}

void Shader::GetPointLightUniformLocations()
{
    m_plus.AmbientIntensity = glGetUniformLocation(m_programID, "plight0.AmbientIntensity");
    m_plus.DiffuseIntensity = glGetUniformLocation(m_programID, "plight0.DiffuseIntensity");
    m_plus.Attenuation = glGetUniformLocation(m_programID, "plight0.Attenuation");
    m_plus.Color = glGetUniformLocation(m_programID, "plight0.Color");
    m_plus.Position = glGetUniformLocation(m_programID, "plight0.Position");
    m_plus.Radius = glGetUniformLocation(m_programID, "plight0.Radius");

}

void Shader::SetPointLightUniforms(const PointLight* pl)
{
    glUniform3fv(m_plus.Color, 1, pl->Color.AsFloatPtr());
    glUniform1f(m_plus.Radius, pl->Radius);
    glUniform3fv(m_plus.Position, 1, pl->Position().AsFloatPtr());
    glUniform1f(m_plus.DiffuseIntensity, pl->DiffuseIntensity);
    glUniform1f(m_plus.AmbientIntensity, pl->AmbientIntensity);

}

void Shader::GetMaterialUniformLocations()
{

}

void Shader::SetMaterialUniforms(const Material* m)
{

}

void Shader::SetMatrixUniforms(const Matrix4& model, const Matrix4& view, const Matrix4& projection, const Matrix3& normal)
{
    Matrix4 modelView = view * model;
    Matrix4 mvp = projection * view * model;
    glUniformMatrix4fv(m_matLocs.model, 1, m_transpose, model.AsFloatPtr());
    glUniformMatrix4fv(m_matLocs.view, 1, m_transpose, view.AsFloatPtr());
    glUniformMatrix4fv(m_matLocs.projection, 1, m_transpose, projection.AsFloatPtr());
    glUniformMatrix4fv(m_matLocs.modelView, 1, m_transpose, modelView.AsFloatPtr());
    glUniformMatrix4fv(m_matLocs.mvp, 1, m_transpose, mvp.AsFloatPtr());
    glUniformMatrix3fv(m_matLocs.normal, 1, m_transpose, normal.AsFloatPtr());
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

bool Shader::ShowGui()
{
    static char name_buffer[] = "";
    return false;

}

int Shader::PushAttribute(const std::string& name)
{
    int l = -1;
    l = glGetAttribLocation(m_programID, name.c_str());
    if (l > -1) {
        m_attribs[l] = name;
    }
	return 0;
}

// ---------------------- GeometryPassShader ---------------------//

GeometryPassShader::GeometryPassShader() : Shader("geometry_pass_shader"s)
{
    Initialize();
}

void GeometryPassShader::Initialize()
{
    const string vsFile = "../Shaders/geometrypass_vert.glsl";
    const string fsFile = "../Shaders/geometrypass_frag.glsl";

    AddShader(vsFile, ShaderType::VERTEX);
    AddShader(fsFile, ShaderType::FRAGMENT);

//    glBindFragDataLocation(ProgramID(), 0, "WorldPosOut");
//    glBindFragDataLocation(ProgramID(), 1, "DiffuseOut");
//    glBindFragDataLocation(ProgramID(), 2, "NormalOut");
//    glBindFragDataLocation(ProgramID(), 3, "TexCoordOut");

    LinkShaderProgram();
}

GeometryPassShader::~GeometryPassShader()
{
}

void GeometryPassShader::GetMaterialUniformLocations()
{
    if (m_mus.isPopulated) {
        return;
    } else {
        GLuint id = m_programID;
        m_mus.Ka = glGetUniformLocation(id, "material0.ka");
        m_mus.Kd = glGetUniformLocation(id, "material0.kd");
        m_mus.Ks = glGetUniformLocation(id, "material0.ks");
        m_mus.Ns = glGetUniformLocation(id, "material0.ns");
        m_mus.isPopulated = true;
    }
}

void GeometryPassShader::SetMaterialUniforms(const Material* m)
{
    //if (!m_mus.isPopulated) {
    //    GetMaterialUniformLocations();
    //}
    glUniform3fv(m_mus.Ka, 1, m->Ambient.AsFloatPtr());
    glUniform3fv(m_mus.Kd, 1, m->Diffuse.AsFloatPtr());
    glUniform3fv(m_mus.Ks, 1, m->Specular.AsFloatPtr());
    glUniform1fv(m_mus.Ns, 1, &(m->Shine));
}

// ----------------- DirectionalLightPassShader -------------------//

DirectionalLightPassShader::DirectionalLightPassShader() : Shader("lighting_pass_shader")
{
    Initialize();
}

DirectionalLightPassShader::~DirectionalLightPassShader()
{
}

void DirectionalLightPassShader::Initialize()
{
    const string vsFile = "../Shaders/lightingpass_vert.glsl";
    const string fsFile = "../Shaders/lightingpass_frag.glsl";

    AddShader(vsFile, ShaderType::VERTEX);
    AddShader(fsFile, ShaderType::FRAGMENT);

    LinkShaderProgram();
}

void DirectionalLightPassShader::GetDirectionalLightUniformLocations()
{
    if (m_dlus.isPopulated) {
        return;
    } else {
        GLuint id = ProgramID();
        m_dlus.Color = glGetUniformLocation(id, "dlight0.Color");
        m_dlus.Direction = glGetUniformLocation(id, "dlight0.Direction");
        m_dlus.AmbientIntensity = glGetUniformLocation(id, "dlight0.AmbientIntensity");
        m_dlus.DiffuseIntensity = glGetUniformLocation(id, "dlight0.DiffuseIntensity");
        m_dlus.isPopulated = true;
    }
}

void DirectionalLightPassShader::SetDirectionalLightUniforms(const DirectionalLight* dl)
{
    //if (!m_dlus.isPopulated) {
    //	GetDirectionalLightUniformLocations();
    //}
    glUniform3fv(m_dlus.Color, 1, dl->Color.AsFloatPtr());
    glUniform3fv(m_dlus.Direction, 1, dl->Direction.AsFloatPtr());
    glUniform1fv(m_dlus.DiffuseIntensity, 1, &dl->Diffuseintensity);
    glUniform1fv(m_dlus.AmbientIntensity, 1, &dl->AmbientIntensity);
}

void DirectionalLightPassShader::SetCameraPosition(const Vector3& position)
{
    int loc = glGetUniformLocation(m_programID, "cameraPosition");
    glUniform3fv(loc, 1, position.AsFloatPtr());

}

void DirectionalLightPassShader::GetMaterialUniformLocations()
{
    if (m_mus.isPopulated) {
        return;
    } else {
        GLuint id = ProgramID();
        m_mus.Ka = glGetUniformLocation(id, "material0.ka");
        m_mus.Kd = glGetUniformLocation(id, "material0.kd");
        m_mus.Ks = glGetUniformLocation(id, "material0.ks");
        m_mus.Ns = glGetUniformLocation(id, "material0.ns");
        m_mus.isPopulated = true;
    }
}

void DirectionalLightPassShader::SetMaterialUniforms(const Material* m)
{
    //if (!m_mus.isPopulated) {
    //    GetMaterialUniformLocations();
    //}
    glUniform3fv(m_mus.Ka, 1, m->Ambient.AsFloatPtr());
    glUniform3fv(m_mus.Kd, 1, m->Diffuse.AsFloatPtr());
    glUniform3fv(m_mus.Ks, 1, m->Specular.AsFloatPtr());
    glUniform1fv(m_mus.Ns, 1, &(m->Shine));
}


// ---------- Lit Shader ----------------------//

LitShader::LitShader() : Shader("Lit_Shader")
{
    Initialize();
    CacheUniformLocations();
}


LitShader::~LitShader()
{
}

void LitShader::Initialize()
{
    const string vsFile = "../Shaders/lit_vert.glsl";
    const string fsFile = "../Shaders/lit_frag.glsl";

    AddShader(vsFile, ShaderType::VERTEX);
    AddShader(fsFile, ShaderType::FRAGMENT);

    LinkShaderProgram();
}

void LitShader::GetDirectionalLightUniformLocations()
{
    if (m_dlus.isPopulated) {
        return;
    } else {
        GLuint id = ProgramID();
        m_dlus.Color = glGetUniformLocation(id, "dlight0.Color");
        m_dlus.Direction = glGetUniformLocation(id, "dlight0.Direction");
        m_dlus.AmbientIntensity = glGetUniformLocation(id, "dlight0.AmbientIntensity");
        m_dlus.DiffuseIntensity = glGetUniformLocation(id, "dlight0.DiffuseIntensity");
        m_dlus.isPopulated = true;
    }
}

void LitShader::SetDirectionalLightUniforms(const DirectionalLight* dl)
{
    if (!m_dlus.isPopulated) {
        GetDirectionalLightUniformLocations();
    }
    glUniform3fv(m_dlus.Color, 1, dl->Color.AsFloatPtr());
    glUniform3fv(m_dlus.Direction, 1, dl->Direction.AsFloatPtr());
    glUniform1fv(m_dlus.DiffuseIntensity, 1, &dl->Diffuseintensity);
    glUniform1fv(m_dlus.AmbientIntensity, 1, &dl->AmbientIntensity);
}

void LitShader::GetMaterialUniformLocations()
{
    if (m_mus.isPopulated) {
        return;
    } else {
        GLuint id = ProgramID();
        m_mus.Ka = glGetUniformLocation(id, "material0.ka");
        m_mus.Kd = glGetUniformLocation(id, "material0.kd");
        m_mus.Ks = glGetUniformLocation(id, "material0.ks");
        m_mus.Ns = glGetUniformLocation(id, "material0.ns");
        m_mus.isPopulated = true;
    }
}

void LitShader::SetMaterialUniforms(const Material* m)
{
    if (!m_mus.isPopulated) {
        GetMaterialUniformLocations();
    }
    glUniform3fv(m_mus.Ka, 1, m->Ambient.AsFloatPtr());
    glUniform3fv(m_mus.Kd, 1, m->Diffuse.AsFloatPtr());
    glUniform3fv(m_mus.Ks, 1, m->Specular.AsFloatPtr());
    glUniform1fv(m_mus.Ns, 1, &(m->Shine));
}

void LitShader::SetTransformUniforms(const Transform & trans)
{
    uint id = ProgramID();
    Quaternion q = trans.Orientation;
    uint positionLoc = glGetUniformLocation(id, "transform.position");
    uint orientationLoc = glGetUniformLocation(id, "transform.orientation");
    uint scaleLoc = glGetUniformLocation(id, "transform.scale");

    auto pos = trans.Position;
    auto rot = trans.Orientation;
    auto scale = trans.Scale;

    glUniform3f(positionLoc, pos.x, pos.y, pos.z);
    glUniform4f(orientationLoc, rot.x, rot.y, rot.z, rot.w);
    glUniform3f(scaleLoc, scale.x, scale.y, scale.z);

}

void LitShader::GetPointLightUniformLocations()
{
    if (m_plus.isPopulated) {
        return;
    } else {
        uint id = ProgramID();
        m_plus.Color = glGetUniformLocation(id, "plight0.Color");
        m_plus.Position = glGetUniformLocation(id, "plight0.Position");
        m_plus.Attenuation = glGetUniformLocation(id, "plight0.ConstAtten");
        m_plus.AmbientIntensity = glGetUniformLocation(id, "plight0.AmbientIntensity");
        m_plus.DiffuseIntensity = glGetUniformLocation(id, "plight0.DiffuseIntensity");
        m_plus.Radius = glGetUniformLocation(id, "plight0.Radius");
        m_plus.isPopulated = true;
    }
}

void LitShader::SetPointLightUniforms(const PointLight* pl)
{
    if (!m_plus.isPopulated) {
        GetPointLightUniformLocations();
    }
    glUniform3fv(m_plus.Color, 1, pl->Color.AsFloatPtr());
    glUniform3fv(m_plus.Position, 1, pl->GetWorldTransform().Position.AsFloatPtr());
    glUniform1fv(m_plus.Attenuation, 1, &pl->Attenuation);
    glUniform1fv(m_plus.AmbientIntensity, 1, &pl->AmbientIntensity);
    glUniform1fv(m_plus.DiffuseIntensity, 1, &pl->DiffuseIntensity);
    glUniform1fv(m_plus.Radius, 1, &pl->Radius);
}

// ---------- Lit Shader ----------------------//

AnimatedLitShader::AnimatedLitShader() : Shader("animated_lit_shader")
{
    Initialize();
    CacheUniformLocations();
    GetBoneTransformUniformLocations();
}


AnimatedLitShader::~AnimatedLitShader()
{
}

void AnimatedLitShader::Initialize()
{
    const string vsFile = "../Shaders/animated_vert.glsl";
    const string fsFile = "../Shaders/animated_frag.glsl";

    AddShader(vsFile, ShaderType::VERTEX);
    AddShader(fsFile, ShaderType::FRAGMENT);

    LinkShaderProgram();
}

void AnimatedLitShader::GetDirectionalLightUniformLocations()
{
    if (m_dlus.isPopulated) {
        return;
    } else {
        GLuint id = ProgramID();
        m_dlus.Color = glGetUniformLocation(id, "dlight0.Color");
        m_dlus.Direction = glGetUniformLocation(id, "dlight0.Direction");
        m_dlus.AmbientIntensity = glGetUniformLocation(id, "dlight0.AmbientIntensity");
        m_dlus.DiffuseIntensity = glGetUniformLocation(id, "dlight0.DiffuseIntensity");
        m_dlus.isPopulated = true;
    }
}

void AnimatedLitShader::SetDirectionalLightUniforms(const DirectionalLight* dl)
{
    if (!m_dlus.isPopulated) {
        GetDirectionalLightUniformLocations();
    }
    glUniform3fv(m_dlus.Color, 1, dl->Color.AsFloatPtr());
    glUniform3fv(m_dlus.Direction, 1, dl->Direction.AsFloatPtr());
    glUniform1fv(m_dlus.DiffuseIntensity, 1, &dl->Diffuseintensity);
    glUniform1fv(m_dlus.AmbientIntensity, 1, &dl->AmbientIntensity);
}

void AnimatedLitShader::GetMaterialUniformLocations()
{
    if (m_mus.isPopulated) {
        return;
    } else {
        GLuint id = ProgramID();
        m_mus.Ka = glGetUniformLocation(id, "material0.ka");
        m_mus.Kd = glGetUniformLocation(id, "material0.kd");
        m_mus.Ks = glGetUniformLocation(id, "material0.ks");
        m_mus.Ns = glGetUniformLocation(id, "material0.ns");
        m_mus.isPopulated = true;
    }
}

void AnimatedLitShader::SetMaterialUniforms(const Material* m)
{
    if (!m_mus.isPopulated) {
        GetMaterialUniformLocations();
    }
    glUniform3fv(m_mus.Ka, 1, m->Ambient.AsFloatPtr());
    glUniform3fv(m_mus.Kd, 1, m->Diffuse.AsFloatPtr());
    glUniform3fv(m_mus.Ks, 1, m->Specular.AsFloatPtr());
    glUniform1fv(m_mus.Ns, 1, &(m->Shine));
}

void AnimatedLitShader::SetTransformUniforms(const Transform & trans)
{
    uint id = ProgramID();
    Quaternion q = trans.Orientation;
    uint positionLoc = glGetUniformLocation(id, "transform.position");
    uint orientationLoc = glGetUniformLocation(id, "transform.orientation");
    uint scaleLoc = glGetUniformLocation(id, "transform.scale");

    auto pos = trans.Position;
    auto rot = trans.Orientation;
    auto scale = trans.Scale;

    glUniform3f(positionLoc, pos.x, pos.y, pos.z);
    glUniform4f(orientationLoc, rot.x, rot.y, rot.z, rot.w);
    glUniform3f(scaleLoc, scale.x, scale.y, scale.z);

}

void AnimatedLitShader::GetPointLightUniformLocations()
{
    if (m_plus.isPopulated) {
        return;
    } else {
        uint id = ProgramID();
        m_plus.Color = glGetUniformLocation(id, "plight0.Color");
        m_plus.Position = glGetUniformLocation(id, "plight0.Position");
        m_plus.Attenuation = glGetUniformLocation(id, "plight0.ConstAtten");
        m_plus.AmbientIntensity = glGetUniformLocation(id, "plight0.AmbientIntensity");
        m_plus.DiffuseIntensity = glGetUniformLocation(id, "plight0.DiffuseIntensity");
        m_plus.Radius = glGetUniformLocation(id, "plight0.Radius");
        m_plus.isPopulated = true;
    }
}

void AnimatedLitShader::SetPointLightUniforms(const PointLight* pl)
{
    if (!m_plus.isPopulated) {
        GetPointLightUniformLocations();
    }
    glUniform3fv(m_plus.Color, 1, pl->Color.AsFloatPtr());
    glUniform3fv(m_plus.Position, 1, pl->GetWorldTransform().Position.AsFloatPtr());
    glUniform1fv(m_plus.Attenuation, 1, &pl->Attenuation);
    glUniform1fv(m_plus.AmbientIntensity, 1, &pl->AmbientIntensity);
    glUniform1fv(m_plus.DiffuseIntensity, 1, &pl->DiffuseIntensity);
    glUniform1fv(m_plus.Radius, 1, &pl->Radius);
}

// --------------------- Basic Shader --------------------//

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


// ----------------------- FontShader ----------------//

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

// ------------------------ ShadowMapShader ---------------------- //

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

// -------------------- SkyboxShader -------------------//

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


} // Jasper
