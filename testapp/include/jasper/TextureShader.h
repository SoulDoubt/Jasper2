#ifndef _JASPER_TEXTURE_SHADER_H_
#define _JASPER_TEXTURE_SHADER_H_

#include "Shader.h"
namespace Jasper {

	using namespace std;

	class TextureShader : public Shader {
	public:

		TextureShader() : Shader("Texture_Shader") {
			Initialize();
		}

		void Initialize() override {
			string vsFile = "./Shaders/texture_vert.glsl";
			string fsFile = "./Shaders/texture_fragment.glsl";

			AddShader(vsFile, ShaderType::VERTEX);
			AddShader(fsFile, ShaderType::FRAGMENT);

			LinkShaderProgram();
		}
	};

}

#endif
