#include "FontShader.h"

namespace Jasper {

	using namespace std;

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
		string vsFile = "../Shaders/font_vert.glsl";
		string fsFile = "../Shaders/font_frag.glsl";

		AddShader(vsFile, ShaderType::VERTEX);
		AddShader(fsFile, ShaderType::FRAGMENT);

		LinkShaderProgram();
	}

}
