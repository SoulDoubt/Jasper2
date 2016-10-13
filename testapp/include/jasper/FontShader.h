#ifndef _JASPER_FONT_SHADER_H_
#define _JASPER_FONT_SHADER_H_


#include "Shader.h"

namespace Jasper {

class FontShader :
	public Shader
{
public:
	FontShader();
	~FontShader();

	void Initialize() override;
};

}

#endif // _JASPER_FONT_SHADER_H_