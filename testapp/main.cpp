#include "GLWindow.h"
#include <iostream>

#ifdef _WIN32 || _WIN64
#include <Windows.h>
#include <tchar.h>
#define BUFFSIZE MAX_PATH
#endif

using namespace Jasper;
#undef main // bypass SDL_main for now.
int main(int argc, char** argv) {
#ifdef _WIN32
	TCHAR buffer[BUFFSIZE];
	DWORD retChars;

	retChars = GetCurrentDirectory(BUFFSIZE, buffer);
	_tprintf(TEXT("Directory is: (%s)\n"), buffer);

#endif
	std::unique_ptr<GLWindow> window = std::make_unique<GLWindow>(1024, 768, "Jasper");
	window->Init();
	window->InitializeScene();
	window->RunLoop();

	SDL_Quit();
	return 0;
}

void printGLInfo() {
	printf("Using OpenGL Version: %s\n", glGetString(GL_VERSION));
}
