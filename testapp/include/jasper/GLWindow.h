#ifndef _GL_WINDOW_H_
#define _GL_WINDOW_H_

#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>

//#include <GLFW\glfw3.h>
#include <string>
#include <memory>
#include "Common.h"

//#include <SDL\SDL.h>

//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>


namespace Jasper {

class Scene;

class GLWindow
{
public:
	GLWindow(int h, int w, std::string title);
	~GLWindow();

	bool Init();

	int Height, Width;
	std::string Title;

	SDL_Window* m_window;
	SDL_GLContext m_context;
	//SDL_Window* m_sdlWindow;

	void RunLoop();

	void InitializeScene();

	void SetupGL();

	//HWND GetWindowHandle() const;

	void printGLInfo() {
		printf("Vendor: %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version: %s\n", glGetString(GL_VERSION));
		printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	std::unique_ptr<Scene> m_scene;

private:
	NON_COPYABLE(GLWindow);
	double m_previousFrameTime = 0;

};
}
#endif // _GL_WINDOW_H_

