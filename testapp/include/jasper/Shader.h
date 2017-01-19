#ifndef _JASPER_SHADER_H_
#define _JASPER_SHADER_H_

#include "Common.h"
#include "matrix.h"
#include <GL/glew.h>
#include <vector>
#include "Lights.h"
#include <string>



/*

*/
namespace Jasper
{

enum class ShaderClassType
{
    LitShader,
    SkyboxShader,
    BasicShader,
    TextureShader,
    FontShader,
    GuiShader,
    GeometryPassShader,
    DirectionalLightPassShader,
    PointLightPassShader,
};

class Material;

struct DirectionalLightUniformLocations {
    bool isPopulated = false;
    int Color;
    int Direction;
    int AmbientIntensity;
    int DiffuseIntensity;
};


struct MaterialUniformLocations {
    bool isPopulated = false;
    int Ka;
    int Kd;
    int Ks;
    int Ns;
};

struct PointLightUniformLocations {
    bool isPopulated = false;
    int Color;
    int Position;
    int AmbientIntensity;
    int DiffuseIntensity;
    int ConstAttenuation;
    int LinearAtten;
    int ExpAtten;
    int Radius;
};

class Shader
{
public:
    enum ShaderType {
        VERTEX,
        FRAGMENT,
        GEOMETRY
    };

    Shader();
    Shader(std::string name);
    ~Shader();

    void PrintAttribsAndUniforms();

    bool ShowGui();

    virtual ShaderClassType GetShaderClassType() const = 0;

    std::string GetName() const {
        return m_name;
    }

    void AddShader(std::string filename, ShaderType t);
    void LinkShaderProgram();

    const GLuint ProgramID() const;

    bool Bind();
    void Release();

    int GetAttributeLocation(const char* name);
    void SetAttributeArray(const char* name, GLenum type, const void* values, int tupleSize, int stride);
    void SetAttributeArray(int location, GLenum num_type, void* offset, int num_components, int stride);
    void SetAttributeArray(int location, GLenum num_type, void* offset, int num_components, int stride, bool normalize);

    virtual void Initialize();
    virtual void Destroy();

    // attribute lookups
    virtual int PositionAttributeLocation();
    virtual int NormalAttributeLocation();
    virtual int TexCoordAttributeLocation();
    virtual int ColorsAttributeLocation();
    virtual int TangentAttributeLocation();
    virtual int BitangentAttributeLocation();

    //
    virtual void SetModelViewMatrix(const Matrix4& mvm);
    virtual void SetModelViewProjectionMatrix(const Matrix4& mvp);
    virtual void SetNormalMatrix(const Matrix3& normal);
    virtual void SetModelMatrix(const Matrix4& model);
    virtual void SetViewMatrix(const Matrix4& view);
    virtual void SetProjectionMatrix(const Matrix4& projection) {}

    virtual void GetDirectionalLightUniformLocations();
    virtual void SetDirectionalLightUniforms(const DirectionalLight* dl);

    virtual void GetPointLightUniformLocations();
    virtual void SetPointLightUniforms(const PointLight* pl, const Vector3& eslp);

    virtual void GetMaterialUniformLocations();
    virtual void SetMaterialUniforms(const Material* m);

    virtual void SetTransformUniforms(const Transform & trans);

    void SetBoundMaterial(Material* m) {
        m_material = m;
    }

    virtual void SetCameraPosition(const Vector3& cp);



private:
    NON_COPYABLE(Shader);

    std::vector<uint> m_shaders;
    GLboolean m_transpose = GL_FALSE;



protected:
    uint m_programID;
    int m_positionsAttribute = -1;
    int m_normalsAttribute = -1;
    int m_texCoordsAttribute = -1;
    int m_colorsAttribute = -1;
    int m_tangentAttribute = -1;
    int m_bitangentAttribute = -1;
    Material* m_material;
    std::string m_name;

    DirectionalLightUniformLocations m_dlus;
    PointLightUniformLocations m_plus;
    MaterialUniformLocations m_mus;

};

inline const uint Shader::ProgramID() const
{
    return m_programID;
}

class GeometryPassShader : public Shader
{
public:
    GeometryPassShader();
    ~GeometryPassShader();

    void Initialize() override;

    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::GeometryPassShader;
    }

    void GetMaterialUniformLocations() override;
    void SetMaterialUniforms(const Material* m) override;
};

class DirectionalLightPassShader : public Shader
{
public:
    DirectionalLightPassShader();
    ~DirectionalLightPassShader();

    void Initialize() override;

    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::DirectionalLightPassShader;
    }

    void SetActiveTexture(uint i) {
        int loc;
        if (i == 0) {
            loc = glGetUniformLocation(m_programID, "positionTexture");
        } else if (i == 1) {
            loc = glGetUniformLocation(m_programID, "diffuseTexture");
        } else if (i == 2) {
            loc = glGetUniformLocation(m_programID, "normalTexture");
        } else if (i == 3) {
            loc = glGetUniformLocation(m_programID, "texCoordTexture");
        } else if (i == 4) {
            loc = glGetUniformLocation(m_programID, "specularTexture");
        }
        if (loc > -1) {
            glUniform1i(loc, i);
        }
    }
    void GetDirectionalLightUniformLocations() override;
    void SetDirectionalLightUniforms(const DirectionalLight* dl) override;
    void SetCameraPosition(const Vector3& position);
    void GetMaterialUniformLocations() override;
    void SetMaterialUniforms(const Material* m) override;
    //void GetMaterialUniformLocations();
    //void SetMaterialUniforms(const Material* m);
};

class PointLightPassShader : public Shader
{
    PointLightPassShader() : Shader("pointlightpass_shader") {}
    ~PointLightPassShader() {}
    void Initialize() override {
        const std::string vsFile = "../Shaders/pointlightpass_vert.glsl";
        const std::string fsFile = "../Shaders/pointlightpass_frag.glsl";

        AddShader(vsFile, ShaderType::VERTEX);
        AddShader(fsFile, ShaderType::FRAGMENT);

        LinkShaderProgram();
    }

};

class BasicShader :
    public Shader
{
public:
    BasicShader();
    ~BasicShader();

    void Initialize() override;
    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::BasicShader;
    }

    void SetColor(Vector4 color);
};

class FontShader :
    public Shader
{
public:
    FontShader();
    ~FontShader();

    void Initialize() override;

    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::FontShader;
    }
};

class GuiShader : public Shader
{
    void Initialize() override;
    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::GuiShader;
    }
};

inline void GuiShader::Initialize()
{
    std::string vsFile = "../Shaders/texture_vert.glsl";
    std::string fsFile = "../Shaders/texture_frag.glsl";

    AddShader(vsFile, ShaderType::VERTEX);
    AddShader(fsFile, ShaderType::FRAGMENT);

    LinkShaderProgram();
}

class LitShader :
    public Shader
{
public:
    LitShader();
    ~LitShader();

    void Initialize() override;

    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::LitShader;
    }

    void GetDirectionalLightUniformLocations() override;
    void SetDirectionalLightUniforms(const DirectionalLight* dl) override;

    void GetMaterialUniformLocations()override;
    void SetMaterialUniforms(const Material* m) override;

    void GetPointLightUniformLocations() override;
    void SetPointLightUniforms(const PointLight* dl, const Vector3& eslp) override;

    void SetTransformUniforms(const Transform& trans) override;
};

class SkyboxShader :
    public Shader
{
public:
    SkyboxShader(std::string name);
    ~SkyboxShader();

    void Initialize() override;

    virtual void SetProjectionMatrix(const Matrix4& proj) override;
    virtual void SetViewMatrix(const Matrix4& view) override;

    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::SkyboxShader;
    }

};

class ShadowMapShader : public Shader
{

public:
    ShadowMapShader();
    ~ShadowMapShader();

    void Initialize() override;

};

class TextureShader : public Shader
{
public:

    TextureShader() : Shader("Texture_Shader") {
        Initialize();
    }

    void Initialize() override {
        std::string vsFile = "./Shaders/texture_vert.glsl";
        std::string fsFile = "./Shaders/texture_fragment.glsl";

        AddShader(vsFile, ShaderType::VERTEX);
        AddShader(fsFile, ShaderType::FRAGMENT);

        LinkShaderProgram();
    }

    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::TextureShader;
    }
};

}

#endif // _SHADER_H_
