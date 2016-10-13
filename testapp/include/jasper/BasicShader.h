#ifndef _BASIC_SHADER_H_
#define _BASIC_SHADER_H_

#include "Shader.h"

namespace Jasper{

class BasicShader :
	public Shader
{
public:
	BasicShader();
	~BasicShader();

	virtual void Initialize() override;	

};

}

#endif // _BASIC_SHADER_H_