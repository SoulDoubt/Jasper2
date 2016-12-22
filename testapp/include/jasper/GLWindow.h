#ifndef _GL_WINDOW_H_
#define _GL_WINDOW_H_

#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>

#include <string>
#include <memory>
#include "Common.h"

#include "Shader.h"
#include "GLBuffer.h"


struct ImDrawData;

namespace Jasper
{

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
    void InitializeGui();

    void SetupGL();
    void SetupGui(); // add gui components here.

    void GuiNewFrame();
    void GetGuiFontTexture();
    void DrawGui();
    bool DrawGameObjectGuiNode(GameObject* go);
    bool CreateAddGameObjectGui(GameObject* parent);
    bool CreateMaterialEditorGui(Material* material);
    void RenderGui(ImDrawData* draw_data);

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

    struct GuiGLHandles {
        std::unique_ptr<Shader>   GuiShader;
        std::unique_ptr<GLBuffer> PositionBuffer;
        std::unique_ptr<GLBuffer> IndexBuffer;
        unsigned vaoID;
        unsigned fontTexture;
    };

    GuiGLHandles m_guiHandles;
    Matrix4 m_guiMatrix;

	void DrawMainMenu();
	void DrawDebugWindow();
	void DrawGameObjectEditor();

	bool m_showGui;



};
}
#endif // _GL_WINDOW_H_
