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
    DeferredStencilPassShader
};

class Material;



class Shader
{
public:
    enum ShaderType {
        VERTEX,
        FRAGMENT,
        GEOMETRY
    };

    enum SHADER_FLAGS {
        USE_GBUFFER_TEXTURES = 0x01
    };

    struct MatrixUniformLocations {
        bool isPopulated = false;
        int model = -1;
        int view = -1;
        int projection = -1;
        int normal = -1;
        int modelView = -1;
        int mvp = -1;
    };

    struct DirectionalLightUniformLocations {
        bool isPopulated = false;
        int Color = -1;
        int Direction= -1;
        int AmbientIntensity = -1;
        int DiffuseIntensity = -1;
    };


    struct MaterialUniformLocations {
        bool isPopulated = false;
        int Ka = -1;
        int Kd = -1;
        int Ks = -1;
        int Ns = -1;
        int diffuseTexture = -1;
        int normalMapTexture = -1;
        int specularMapTexture = -1;
    };

    struct PointLightUniformLocations {
        bool isPopulated = false;
        int Color = -1;
        int Position = -1;
        int AmbientIntensity = -1;
        int DiffuseIntensity = -1;
        int Attenuation = -1;
        int Radius = -1;
    };

    struct TextureUnitLocations {
        struct _gBuffer {
            int position = -1;
            int diffuse = -1;
            int normal = -1;
            int specular = -1;
            int finalt = -1;
        };

        struct _maps {
            int has_color_map = -1;
            int has_normal_map = -1;
            int has_specular_mao = -1;
            int colormap = -1;
            int normalmap = -1;
            int specularmap = -1;
        };

        _gBuffer GBuffer;
        _maps Maps;
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

    // uniform setters
    virtual void SetModelViewMatrix(const Matrix4& mvm);
    virtual void SetModelViewProjectionMatrix(const Matrix4& mvp);
    virtual void SetNormalMatrix(const Matrix3& normal);
    virtual void SetModelMatrix(const Matrix4& model);
    virtual void SetViewMatrix(const Matrix4& view);
    virtual void SetProjectionMatrix(const Matrix4& projection) {}

    // uniform location lookups
    virtual void GetDirectionalLightUniformLocations();
    virtual void SetDirectionalLightUniforms(const DirectionalLight* dl);

    virtual void GetPointLightUniformLocations();
    virtual void SetPointLightUniforms(const PointLight* pl);

    virtual void GetMaterialUniformLocations();
    virtual void SetMaterialUniforms(const Material* m);

    virtual void GetMatrixUniformLocations();
    virtual void SetMatrixUniforms(const Matrix4& model, const Matrix4& view, const Matrix4& projection, const Matrix3& normal);

    virtual void CacheTextureUniformLocations() {}

    TextureUnitLocations GetTextureUniformLocations() {
        return m_textureLocations;
    }
    
    int GetBoneIndexAttributeLocation() {
        return glGetAttribLocation(m_programID, "boneIds");
    }
    
    int GetBoneWeightAttributeLocation();
    
    const static int MAX_BONES = 192;
    std::vector<int> boneTransformUniformLocations;
    
    void GetBoneTransformUniformLocations();
      
    
    void SetBoneTransform(int index, const Matrix4& transform);

    virtual void SetTransformUniforms(const Transform & trans);

    void SetBoundMaterial(Material* m) {
        m_material = m;
    }

    virtual void SetCameraPosition(const Vector3& cp);


    int PushAttribute(const std::string& name);
    int PushUniform(const std::string& name);

    uint Flags = 0x00;

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
    int m_cameraPositionUniformLocation = -1;
    Material* m_material;
    std::string m_name;

    DirectionalLightUniformLocations m_dlus;
    PointLightUniformLocations m_plus;
    MaterialUniformLocations m_mus;
    MatrixUniformLocations m_matLocs;

    std::unordered_map<int, std::string> m_attribs;
    std::unordered_map<int, std::string> m_uniforms;

    virtual void CacheUniformLocations();

    TextureUnitLocations m_textureLocations;



};

inline void Shader::CacheUniformLocations()
{
    GetMatrixUniformLocations();
    GetMaterialUniformLocations();


}

inline void Shader::GetMatrixUniformLocations()
{
    if (! m_matLocs.isPopulated) {
        m_matLocs.model = glGetUniformLocation(m_programID, "modelMatrix");
        m_matLocs.view = glGetUniformLocation(m_programID, "viewMatrix");
        m_matLocs.projection = glGetUniformLocation(ProgramID(), "projectionMatrix");
        m_matLocs.modelView = glGetUniformLocation(m_programID, "mvMatrix");
        m_matLocs.mvp = glGetUniformLocation(m_programID, "mvpMatrix");
        m_matLocs.normal = glGetUniformLocation(m_programID, "normalMatrix");
    }


}

inline const uint Shader::ProgramID() const
{
    return m_programID;
}

class GeometryPassShader final : public Shader
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

    void CacheUniformLocations() override {
        GetMatrixUniformLocations();
        GetMaterialUniformLocations();
        CacheTextureUniformLocations();
    }

    void CacheTextureUniformLocations() override {
        m_textureLocations.Maps.has_color_map = glGetUniformLocation(m_programID, "has_diffuse_map");
        m_textureLocations.Maps.has_normal_map = glGetUniformLocation(m_programID, "has_normal_map");
        m_textureLocations.Maps.has_specular_mao = glGetUniformLocation(m_programID, "has_specular_map");

        m_textureLocations.Maps.colormap = glGetUniformLocation(m_programID, "colorMap");
        m_textureLocations.Maps.normalmap = glGetUniformLocation(m_programID, "normalMap");
        m_textureLocations.Maps.specularmap = glGetUniformLocation(m_programID, "specularMap");
    }
};

class DirectionalLightPassShader final : public Shader
{
public:


    DirectionalLightPassShader();
    ~DirectionalLightPassShader();

    void Initialize() override;

    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::DirectionalLightPassShader;
    }

    void CacheTextureUniformLocations() override {
        m_textureLocations.GBuffer.position = glGetUniformLocation(m_programID, "positionTexture");
        m_textureLocations.GBuffer.diffuse = glGetUniformLocation(m_programID, "diffuseTexture");
        m_textureLocations.GBuffer.normal = glGetUniformLocation(m_programID, "normalTexture");
        m_textureLocations.GBuffer.specular = glGetUniformLocation(m_programID, "specularTexture");
        m_textureLocations.GBuffer.finalt = glGetUniformLocation(m_programID, "finalTexture");
    }

    void SetActiveTexture(uint i) {
        int loc;
        if (i == 0) {
            loc = m_textureLocations.GBuffer.position;
        } else if (i == 1) {
            loc = m_textureLocations.GBuffer.diffuse;
        } else if (i == 2) {
            loc = m_textureLocations.GBuffer.normal;
        } else if (i == 3) {
            loc = m_textureLocations.GBuffer.specular;
        } else if (i == 4) {
            loc = m_textureLocations.GBuffer.finalt;
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

    void CacheUniformLocations() override {
        GetMatrixUniformLocations();
        GetMaterialUniformLocations();
        GetDirectionalLightUniformLocations();
        CacheTextureUniformLocations();
        m_cameraPositionUniformLocation = glGetUniformLocation(ProgramID(), "cameraPosition");
    }


protected:

};

class PointLightPassShader final : public Shader
{
public:
    PointLightPassShader() : Shader("pointlightpass_shader") {
        Initialize();
    }

    ~PointLightPassShader() {}

    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::PointLightPassShader;
    }

    void SetScreenSize(const Vector2& ss) {
        int active;
        glGetIntegerv(GL_ACTIVE_PROGRAM, &active);
        glUseProgram(m_programID);
        glUniform2fv(m_screenSizeLocation, 1, ss.AsFloatPtr());
        glUseProgram(active);
    }

    void Initialize() override {
        const std::string vsFile = "../Shaders/pointlightpass_vert.glsl";
        const std::string fsFile = "../Shaders/pointlightpass_frag.glsl";

        AddShader(vsFile, ShaderType::VERTEX);
        AddShader(fsFile, ShaderType::FRAGMENT);

        LinkShaderProgram();
    }

    void CacheTextureUniformLocations() override {
        m_textureLocations.GBuffer.position = glGetUniformLocation(m_programID, "positionTexture");
        m_textureLocations.GBuffer.diffuse = glGetUniformLocation(m_programID, "diffuseTexture");
        m_textureLocations.GBuffer.normal = glGetUniformLocation(m_programID, "normalTexture");
        m_textureLocations.GBuffer.specular = glGetUniformLocation(m_programID, "specularTexture");
        m_textureLocations.GBuffer.finalt = glGetUniformLocation(m_programID, "finalTexture");
    }

    void CacheUniformLocations() override {
        GetMatrixUniformLocations();
        GetMaterialUniformLocations();
        GetPointLightUniformLocations();
        CacheTextureUniformLocations();
        m_cameraPositionUniformLocation = glGetUniformLocation(m_programID, "cameraPosition");
        m_screenSizeLocation = glGetUniformLocation(m_programID, "screenSize");
    }

    void SetActiveTexture(uint i) {
        int loc;
        if (i == 0) {
            loc = m_textureLocations.GBuffer.position;
        } else if (i == 1) {
            loc = m_textureLocations.GBuffer.diffuse;
        } else if (i == 2) {
            loc = m_textureLocations.GBuffer.normal;
        } else if (i == 3) {
            loc = m_textureLocations.GBuffer.specular;
        } else if (i == 4) {
            loc = m_textureLocations.GBuffer.finalt;
        }
        if (loc > -1) {
            glUniform1i(loc, i);
        }
    }


protected:
    int m_screenSizeLocation = -1;


};

class BasicShader final :
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
    void CacheTextureUniformLocations() {}
};

class FontShader final :
    public Shader
{
public:
    FontShader();
    ~FontShader();

    void Initialize() override;

    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::FontShader;
    }

    void CacheTextureUniformLocations() {}
};

class GuiShader final : public Shader
{
    void Initialize() override;
    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::GuiShader;
    }

    void CacheTextureUniformLocations() {}
};

inline void GuiShader::Initialize()
{
    std::string vsFile = "../Shaders/texture_vert.glsl";
    std::string fsFile = "../Shaders/texture_frag.glsl";

    AddShader(vsFile, ShaderType::VERTEX);
    AddShader(fsFile, ShaderType::FRAGMENT);

    LinkShaderProgram();
}

class LitShader final :
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
    void SetPointLightUniforms(const PointLight* dl) override;

    void SetTransformUniforms(const Transform& trans) override;

    void CacheTextureUniformLocations() override {
        m_textureLocations.Maps.has_color_map = glGetUniformLocation(m_programID, "has_diffuse_map");
        m_textureLocations.Maps.has_normal_map = glGetUniformLocation(m_programID, "has_normal_map");
        m_textureLocations.Maps.has_specular_mao = glGetUniformLocation(m_programID, "has_specular_map");

        m_textureLocations.Maps.colormap = glGetUniformLocation(m_programID, "colorMap");
        m_textureLocations.Maps.normalmap = glGetUniformLocation(m_programID, "normalMap");
        m_textureLocations.Maps.specularmap = glGetUniformLocation(m_programID, "specularMap");
    }

    void CacheUniformLocations() override {
        GetMatrixUniformLocations();
        GetMaterialUniformLocations();
        
        CacheTextureUniformLocations();
    }
};

class AnimatedLitShader final :
    public Shader
{
public:
    AnimatedLitShader();
    ~AnimatedLitShader();

    void Initialize() override;

    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::LitShader;
    }
    
    

    void GetDirectionalLightUniformLocations() override;
    void SetDirectionalLightUniforms(const DirectionalLight* dl) override;

    void GetMaterialUniformLocations()override;
    void SetMaterialUniforms(const Material* m) override;

    void GetPointLightUniformLocations() override;
    void SetPointLightUniforms(const PointLight* dl) override;

    void SetTransformUniforms(const Transform& trans) override;

    void CacheTextureUniformLocations() override {
        m_textureLocations.Maps.has_color_map = glGetUniformLocation(m_programID, "has_diffuse_map");
        m_textureLocations.Maps.has_normal_map = glGetUniformLocation(m_programID, "has_normal_map");
        m_textureLocations.Maps.has_specular_mao = glGetUniformLocation(m_programID, "has_specular_map");

        m_textureLocations.Maps.colormap = glGetUniformLocation(m_programID, "colorMap");
        m_textureLocations.Maps.normalmap = glGetUniformLocation(m_programID, "normalMap");
        m_textureLocations.Maps.specularmap = glGetUniformLocation(m_programID, "specularMap");
    }

    void CacheUniformLocations() override {
        GetMatrixUniformLocations();
        GetMaterialUniformLocations();
        
        CacheTextureUniformLocations();
    }
};

class SkyboxShader final :
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

    void CacheTextureUniformLocations() {}

};

class ShadowMapShader final : public Shader
{

public:
    ShadowMapShader();
    ~ShadowMapShader();

    void Initialize() override;

    void CacheTextureUniformLocations() {}

};

class TextureShader final : public Shader
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
    void CacheTextureUniformLocations() {}
};

class DeferredStencilPassShader final : public Shader
{
public:

    DeferredStencilPassShader() : Shader("deferred_stencil_pass") {
        Initialize();
    }

    ~DeferredStencilPassShader() {}


    void Initialize() override {
        const std::string vsFile = "../Shaders/pointlightpass_vert.glsl";
        const std::string fsFile = "../Shaders/null_frag.glsl";

        AddShader(vsFile, ShaderType::VERTEX);
        AddShader(fsFile, ShaderType::FRAGMENT);

        LinkShaderProgram();
    }

    ShaderClassType GetShaderClassType() const override {
        return ShaderClassType::DeferredStencilPassShader;
    }

    void CacheTextureUniformLocations() {}
};

}

#endif // _SHADER_H_
