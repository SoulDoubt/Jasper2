#ifndef _JASPER_SKYBOX_SHADER_H_
#define _JASPER_SKYBOX_SHADER_H_


#include "Shader.h"

namespace Jasper {

class SkyboxShader :
	public Shader
{
public:
	SkyboxShader();
	~SkyboxShader();

	void Initialize() override;

	virtual void SetProjectionMatrix(const Matrix4& proj) override;
	virtual void SetViewMatrix(const Matrix4& view) override;

};

}

#endif // _JASPER_SKYBOX_SHADER_H_