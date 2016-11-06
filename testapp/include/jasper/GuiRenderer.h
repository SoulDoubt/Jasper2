#pragma once



namespace Jasper
{

class Shader;
class Scene;
class ImGuiIO;
class SDL_Window;

class GuiRenderer
{
private:
    ImGuiIO* m_io;
    Shader* m_shader;
    Scene* m_scene;
    SDL_Window* m_window;

public:
    GuiRenderer(Scene* scene);
    GuiRenderer() = delete;
    GuiRenderer(const GuiRenderer& o) = delete;
    GuiRenderer(GuiRenderer&& o) = default;

    void Render();
    void Initialize();
    
    private:
    bool CreateDeviceObjects();
    bool InitializeImGui();
    


};

}
