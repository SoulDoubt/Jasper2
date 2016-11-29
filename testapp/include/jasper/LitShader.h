#ifndef _JASPER_LIT_SHADER_H_
#define _JASPER_LIT_SHADER_H_

#include "Shader.h"
#include "Transform.h"

namespace Jasper
{

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
}
#endif //_LIT_SHADER_H_
