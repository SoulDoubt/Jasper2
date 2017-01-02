#ifndef _BASIC_SHADER_H_
#define _BASIC_SHADER_H_

#include "Shader.h"

namespace Jasper
{

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

}

#endif // _BASIC_SHADER_H_
