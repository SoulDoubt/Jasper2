#include "GuiRenderer.h"
#include "imgui.h"
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "Scene.h"

namespace Jasper
{
GuiRenderer::GuiRenderer(Scene* scene)
: m_scene(scene){
   // m_window = m_scene->Get
}
void GuiRenderer::Render() {}
void GuiRenderer::Initialize() {}
bool GuiRenderer::CreateDeviceObjects() {}
bool GuiRenderer::InitializeImGui() {}
}
