#ifndef _JASPER_LIT_SHADER_H_
#define _JASPER_LIT_SHADER_H_

#include "Shader.h"
#include "Transform.h"

namespace Jasper {

class LitShader :
	public Shader
{
public:
	LitShader();
	~LitShader();

	void Initialize() override;
	
	virtual void GetDirectionalLightUniformLocations() override;
	virtual void SetDirectionalLightUniforms(const DirectionalLight* dl) override;

	virtual void GetMaterialUniformLocations()override;
	virtual void SetMaterialUniforms(const Material* m) override;

	virtual void GetPointLightUniformLocations() override;
	virtual void SetPointLightUniforms(const PointLight* dl, const Vector3& eslp) override;

	void SetTransformUniforms(const Transform& trans) override;
};
}
#endif //_LIT_SHADER_H_