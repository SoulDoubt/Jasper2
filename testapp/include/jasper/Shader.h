#ifndef _JASPER_SHADER_H_
#define _JASPER_SHADER_H_

#include "Common.h"
#include "matrix.h"
#include <GL/glew.h>
#include <vector>
#include "Lights.h"


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
    LightingPassShader
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

class LightingPassShader : public Shader
{
public:
    LightingPassShader();
    ~LightingPassShader();

    void Initialize() override;

    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::LightingPassShader;
    }

    /*
        #define GBUFFER_TEXTURE_TYPE_DIFFUSE  1
        #define GBUFFER_TEXTURE_TYPE_NORMAL   2
        #define GBUFFER_TEXTURE_TYPE_TEXCOORD 3
        #define GBUFFER_TEXTURE_TYPE_SPECULAR 4
        #define GBUFFER_NUM_TEXTURES          5
    */

    void SetActiveTexture(uint i) {
        int loc;
        if (i == 0) {
            loc = glGetUniformLocation(m_programID, "positionTexture");
        } 
        else if (i == 1){
            loc = glGetUniformLocation(m_programID, "diffuseTexture");
        } 
        else if (i == 2){
            loc = glGetUniformLocation(m_programID, "normalTexture");
        }
        else if (i == 3){
            loc = glGetUniformLocation(m_programID, "texCoordTexture");
        }
        else if (i == 4){
            loc = glGetUniformLocation(m_programID, "specularTexture");
        }
        if (loc > -1) {
            glUniform1i(loc, i);
        }
    }
    void GetDirectionalLightUniformLocations() override;
    void SetDirectionalLightUniforms(const DirectionalLight* dl) override;
    void SetCameraPosition(const Vector3& position);
    //void GetMaterialUniformLocations();
    //void SetMaterialUniforms(const Material* m);
};




}

#endif // _SHADER_H_
