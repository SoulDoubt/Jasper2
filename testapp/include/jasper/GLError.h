#ifndef _GL_ERROR_H_
#define _GL_ERROR_H_

#include <GL/glew.h>
#include <stdio.h>

#define GLERRORCHECK checkGError(__FILE__, __LINE__)

inline void checkGError(const char* file, int line) {
	GLenum error = glGetError();
	if (error != 0) {
		std::string message = "";
		switch (error) {
		case 0x0500:
			message = "GL_INVALID_ENUM";
			break;
		case 0x0501:
			message = "GL_INVALID_VALUE";
			break;
		case 0x0502:
			message = "GL_INVALID_OPERATION";
			break;
		case 0x0503:
			message = "GL_STACK_OVERFLOW";
			break;
		}
		//printf("GL error detected at: %s, line: %d, message: %s\n", file, line, message.c_str());
	}
}

#endif
