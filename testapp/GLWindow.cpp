#include <iostream>
#include "GLWindow.h"
#include "GLError.h"
#include "Scene.h"
#include "GuiShader.h"

#include "imgui.h"

#include <typeinfo>

//#define GLFW_EXPOSE_NATIVE_WIN32
//#define GLFW_EXPOSE_NATIVE_WGL
//#include <GLFW\glfw3native.h>

#include <chrono>

#include <unistd.h>

int main(int argc, char** argv)
{

    char pathBuff[1024];
    getcwd(pathBuff, 1024);
    printf("CWD is: %s\n", pathBuff);

    std::unique_ptr<Jasper::GLWindow> window = std::make_unique<Jasper::GLWindow>(1280, 720, "Jasper");
    window->Init();
    window->InitializeScene();
    window->RunLoop();

    SDL_Quit();
    return 0;
}


namespace Jasper
{

using namespace std;
using namespace std::chrono;

bool MOVING_FORWARD = 0;
bool MOVING_BACKWARD = 0;
bool STRAFING_RIGHT = 0;
bool STRAFING_LEFT = 0;
bool ROTATING_RIGHT = 0;
bool ROTATING_LEFT = 0;
bool ROTATING_UP = 0;
bool ROTATING_DOWN = 0;

bool MOUSE_MOVE = 0;


GLWindow* g_glWindow;


static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
    auto& vector = *static_cast<std::vector<std::string>*>(vec);
    if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
    *out_text = vector.at(idx).c_str();
    return true;
};
 
bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
{
    if (values.empty()) { return false; }
    return ImGui::Combo(label, currIndex, vector_getter,
        static_cast<void*>(&values), values.size());
}
 
bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
{
    if (values.empty()) { return false; }
    return ImGui::ListBox(label, currIndex, vector_getter,
        static_cast<void*>(&values), values.size());
}
 



bool ProcessSDLEvent(SDL_Event evt, Scene* scene, double deltaTime);
bool ProcessSDLEventGui(SDL_Event* evt, Scene* scene);

void DoMovement(Scene* scene, double deltaTime);

void RotateCameraX(Scene* scene, double deltaTime, float degrees);

void RotateCameraY(Scene* scene, double deltaTime, float degrees);

void RenderImGuiDrawLists(ImDrawData* drawData);




GLWindow::GLWindow(int w, int h, string title)
{
    Height = h;
    Width = w;
    Title = title;
}

GLWindow::~GLWindow()
{
}

bool GLWindow::Init()
{

    printf("Inside window initialization...\n");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }

    //m_window = glfwCreateWindow(Width, Height, Title.c_str(), nullptr, nullptr);
    m_window = SDL_CreateWindow(Title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    m_context = SDL_GL_CreateContext(m_window);

    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        printf("GLEW Initialization Failed\n");
        return false;
    } else {
        printf("GLEW Initialized successfully\n");

    }

    glViewport(0, 0, Width, Height);

    m_guiMatrix.CreateOrthographicProjection(0.0f, Width, Height, 0.0f, 1.0f, -1.0f);
    //m_guiMatrix = m_guiMatrix.Translate({0.1, 0.0, 0.0});
    //glfwSetFramebufferSizeCallback(m_window, framebuffer_resize_callback);

    printGLInfo();

    SetupGL();

    InitializeGui();

    return true;
}

void GLWindow::RunLoop()
{

    bool loop = true;
    high_resolution_clock::time_point previousTime = high_resolution_clock::now();

    while (loop) {


        high_resolution_clock::time_point currentTime = high_resolution_clock::now();
        auto timeDiff = duration_cast<nanoseconds>(currentTime - previousTime);
        previousTime = currentTime;
        double timeDelta = timeDiff.count();
        double dt = timeDelta / 1000000000;

        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            ProcessSDLEventGui(&evt, m_scene.get());
            if (ProcessSDLEvent(evt, m_scene.get(), dt)) {
                loop = false;
            }
        }


        DoMovement(m_scene.get(), dt);

        GuiNewFrame();

        // DO ALL THE THINGS HERE!!:)
        //ProcessInput(m_window, m_scene.get(), deltaTime);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_scene->Update(dt);
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
            DrawGui();
        }

        ImGui::Render();
        SDL_GL_SwapWindow(m_window);

    }
    m_scene->Destroy();
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();

//glfwTerminate();
}

void GLWindow::DrawGui()
{
    static bool handleButtonClick = 0;
    static bool showNewGameObjectWindow = false;
    static GameObject* goAddParent = nullptr;

    const Camera& camera = m_scene->GetCamera();
    Vector3 cameraPosition = camera.GetPosition();
    Vector3 cameraDirection = camera.GetViewDirection();
    const Renderer* renderer = m_scene->GetRenderer();

    ImGui::Begin("Debug");

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Camera Position: %s", cameraPosition.ToString().c_str());
    ImGui::Text("Camera Direction: %s", cameraDirection.ToString().c_str());
    ImGui::Text("Objects being Rendered: %d", (int)renderer->GetMeshRendererCount());
    ImGui::Text("Physics: %.6f ms", m_scene->PhysicsFrameTime);
    ImGui::Text("Update: %.6f ms", m_scene->UpdateFrameTime);
    ImGui::Text("Renderer: %.6f ms", m_scene->RendererFrameTime);    

    ImGui::End();

    auto rootNode = m_scene->GetRootNode();

    ImGui::Begin("Game Object Hierarchy");

    if (ImGui::TreeNode("Game Objects")) {
        if (ImGui::TreeNode(rootNode->GetName().data())) {
            if (ImGui::BeginPopupContextItem("item context menu")) {
                if (ImGui::Selectable("Add New Child")) {
                    showNewGameObjectWindow ^= 1;
                    goAddParent = rootNode;
                }
                ImGui::EndPopup();
            }
            const auto& children = rootNode->Children();
            unsigned i = 0;
            GameObject* goToDelete = nullptr;

            for (const auto& child : children) {
                if (ImGui::TreeNode((void*)(uintptr_t)i, child->GetName().data())) {
                    if (ImGui::BeginPopupContextItem("item context menu")) {
                        if (ImGui::Selectable("Delete")) {
                            goToDelete = child.get();
                        }
                        if (ImGui::Selectable("Add New Child")) {
                            showNewGameObjectWindow ^= 1;
                            goAddParent = child.get();
                        }
                        ImGui::EndPopup();
                    }
                    const auto& components = child->Components();
                    unsigned j = 0;
                    for (const auto& cmp : components) {
                        auto typ = typeid(cmp).name();
                        if (ImGui::TreeNode((void*)(uintptr_t)j, cmp->GetName().data())) {
                            ImGui::Text("Component Data");
                            ImGui::TreePop();
                        }
                        j++;
                    }
                    ImGui::TreePop();
                }
                i++;
            }
            if (goToDelete) {
                m_scene->DestroyGameObject(goToDelete);
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();

    }

    if (ImGui::TreeNode("Logging")) {
        ImGui::TextWrapped("The logging API redirects all text output so you can easily capture the content of a window or a block. Tree nodes can be automatically expanded. You can also call ImGui::LogText() to output directly to the log without a visual output.");
        ImGui::LogButtons();
        ImGui::TreePop();
    }
    ImGui::End();
    
    ImGui::Begin("Shaders & Materials");    
    auto& shaders = m_scene->GetShaderCache();
    vector<string> shaderNames;
    shaderNames.reserve(shaders.GetCache().size());
    static int shader_list_selected = 1;
    for (auto& shader : shaders.GetCache()){
        shaderNames.push_back(shader->GetName());
    }
    ListBox("Shaders", &shader_list_selected, shaderNames);
    //ImGui::ListBox("Shaders", &shader_list_selected, shaderNames.data(), shaderNames.size());
    vector<string> materialNames;    
    auto& materials = m_scene->GetMaterialCache();
    materialNames.reserve(materials.GetCache().size());
    static int material_list_selected;
    for (auto& mat : materials.GetCache()){
        materialNames.push_back(mat->GetName());
    }
    ListBox("Materials", &material_list_selected, materialNames);
    ImGui::End();

    if (showNewGameObjectWindow) {
        if (CreateAddGameObjectGui(goAddParent)){
            showNewGameObjectWindow ^= 1;
        }
    }
}

bool GLWindow::CreateAddGameObjectGui(GameObject* parent)
{
    if (parent) {
        static char nameBuff[128] = "";
        //memset(nameBuff, 0, 128);
        ImGui::Begin("New Game Object");
        ImGui::Text("Parent: %s", parent->GetName().data());
        ImGui::InputText("Name: ", nameBuff, 128);
        if (ImGui::Button("Add")){
            parent->AttachNewChild(string(nameBuff));
            ImGui::End();
            return true;
        }
        ImGui::End();
    }
    return false;

}

void GLWindow::InitializeScene()
{
    m_scene = make_unique<Scene>(Width, Height);
    m_scene->Awake();
    //glfwSetWindowUserPointer(m_window, m_scene.get());
}

void GLWindow::InitializeGui()
{
    g_glWindow = this;
    auto window = m_window;
    ImGuiIO& io = ImGui::GetIO();

    io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;                     // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = SDLK_a;
    io.KeyMap[ImGuiKey_C] = SDLK_c;
    io.KeyMap[ImGuiKey_V] = SDLK_v;
    io.KeyMap[ImGuiKey_X] = SDLK_x;
    io.KeyMap[ImGuiKey_Y] = SDLK_y;
    io.KeyMap[ImGuiKey_Z] = SDLK_z;

    io.RenderDrawListsFn = RenderImGuiDrawLists;//[self](ImDrawData* draw_data){
    //self->RenderGui(draw_data);   // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
    //};
    io.ClipboardUserData = nullptr;

#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    io.ImeWindowHandle = wmInfo.info.win.window;
#else
    (void)window;

#endif

    m_guiHandles.GuiShader = make_unique<GuiShader>();
    m_guiHandles.PositionBuffer = make_unique<GLBuffer>(GLBuffer::BufferType::VERTEX);
    m_guiHandles.PositionBuffer->SetUsage(GLBuffer::Usage::StreamDraw);
    m_guiHandles.IndexBuffer = make_unique<GLBuffer>(GLBuffer::BufferType::INDEX);
    m_guiHandles.IndexBuffer->SetUsage(GLBuffer::Usage::StreamDraw);

    GLint last_texture, last_array_buffer, last_vertex_array;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

    glGenVertexArrays(1, &m_guiHandles.vaoID);
    glBindVertexArray(m_guiHandles.vaoID);

    m_guiHandles.GuiShader->Initialize();
    m_guiHandles.PositionBuffer->Create();
    m_guiHandles.IndexBuffer->Create();

    m_guiHandles.PositionBuffer->Bind();
    int positionLocation = m_guiHandles.GuiShader->PositionAttributeLocation();
    int texCoordLocation = m_guiHandles.GuiShader->TexCoordAttributeLocation();
    int colorLocation = m_guiHandles.GuiShader->ColorsAttributeLocation();
    m_guiHandles.GuiShader->SetAttributeArray(positionLocation, GL_FLOAT, (void*)offsetof(ImDrawVert, pos), 2, sizeof(ImDrawVert));
    m_guiHandles.GuiShader->SetAttributeArray(texCoordLocation, GL_FLOAT, (void*)offsetof(ImDrawVert, uv), 2, sizeof(ImDrawVert));
    m_guiHandles.GuiShader->SetAttributeArray(colorLocation, GL_UNSIGNED_BYTE, (void*)offsetof(ImDrawVert, col), 4, sizeof(ImDrawVert), true);
    m_guiHandles.PositionBuffer->Release();

    io.Fonts->AddFontFromFileTTF("../fonts/Roboto-Medium.ttf", 18);
    GetGuiFontTexture();
    glBindVertexArray(0);

    // Restore modified GL state
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindVertexArray(last_vertex_array);


}

void GLWindow::GetGuiFontTexture()
{
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &m_guiHandles.fontTexture);
    glBindTexture(GL_TEXTURE_2D, m_guiHandles.fontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)m_guiHandles.fontTexture;

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);
}

void GLWindow::SetupGL()
{
    //auto hwnd = this->GetWindowHandle();
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_TEXTURE_3D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glfwWindowHint(GL_SAMPLES, 4);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnable(GL_MULTISAMPLE);
    GLERRORCHECK;

}

double gui_time = 0;
bool gui_mouse_pressed[3];
float gui_mouse_wheel = 0.0;

void GLWindow::GuiNewFrame()
{
    //   if (!g_FontTexture)
    //ImGui_ImplSdlGL3_CreateDeviceObjects();
    auto window = m_window;
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_GL_GetDrawableSize(window, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

    // Setup time step
    Uint32	time = SDL_GetTicks();
    double current_time = time / 1000.0;
    io.DeltaTime = gui_time > 0.0 ? (float)(current_time - gui_time) : (float)(1.0f / 60.0f);
    gui_time = current_time;

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from SDL_PollEvent())
    int mx, my;
    Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS)
        io.MousePos = ImVec2((float)mx, (float)my);   // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
    else
        io.MousePos = ImVec2(-1, -1);

    io.MouseDown[0] = gui_mouse_pressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;		// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    io.MouseDown[1] = gui_mouse_pressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    io.MouseDown[2] = gui_mouse_pressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    gui_mouse_pressed[0] = gui_mouse_pressed[1] = gui_mouse_pressed[2] = false;

    io.MouseWheel = gui_mouse_wheel;
    gui_mouse_wheel = 0.0f;

    // Hide OS mouse cursor if ImGui is drawing it
    SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);

    // Start the frame
    ImGui::NewFrame();
}


void ResizeWindow(int w, int h, Scene* scene)
{
    //printf("Window resized\n");
    //printf("Width: %d, Height: %d \n", w, h);
    glViewport(0, 0, w, h);

    scene->Resize(w, h);

}

void GLWindow::RenderGui(ImDrawData* draw_data)
{
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = 1.0f;
    // Backup GL state
    GLint last_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_active_texture;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
    GLint last_array_buffer;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    GLint last_element_array_buffer;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    GLint last_vertex_array;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    GLint last_blend_src;
    glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
    GLint last_blend_dst;
    glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
    GLint last_blend_equation_rgb;
    glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
    GLint last_blend_equation_alpha;
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
    GLint last_viewport[4];
    glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4];
    glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    // Setup orthographic projection matrix
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);



    //glUseProgram(g_ShaderHandle);
    auto shader = m_guiHandles.GuiShader.get();
    shader->Bind();
    shader->SetModelViewProjectionMatrix(m_guiMatrix);
    int dl = glGetUniformLocation(shader->ProgramID(), "colorMap");
    if (dl > -1) {
        glUniform1i(dl, 0);
    }

    glBindVertexArray(m_guiHandles.vaoID);

    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        m_guiHandles.PositionBuffer->Bind();
        m_guiHandles.PositionBuffer->Allocate((GLvoid*)cmd_list->VtxBuffer.Data, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        m_guiHandles.IndexBuffer->Bind();
        m_guiHandles.IndexBuffer->Allocate((GLvoid*)cmd_list->IdxBuffer.Data, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback) {
                pcmd->UserCallback(cmd_list, pcmd);
            } else {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }

    // Restore modified GL state
    //glUseProgram(last_program);
    m_guiHandles.GuiShader->Release();
    glUseProgram(last_program);
    glActiveTexture(last_active_texture);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindVertexArray(last_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFunc(last_blend_src, last_blend_dst);
    if (last_enable_blend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST);
    else glDisable(GL_SCISSOR_TEST);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);


}

void RenderImGuiDrawLists(ImDrawData* draw_data)
{
    g_glWindow->RenderGui(draw_data);


}

bool ProcessSDLEventGui(SDL_Event* event, Scene* scene)
{
    ImGuiIO& io = ImGui::GetIO();
    switch (event->type) {
    case SDL_MOUSEWHEEL: {
        if (event->wheel.y > 0)
            gui_mouse_wheel = 1;
        if (event->wheel.y < 0)
            gui_mouse_wheel = -1;
        return true;
    }
    case SDL_MOUSEBUTTONDOWN: {
        if (event->button.button == SDL_BUTTON_LEFT) gui_mouse_pressed[0] = true;
        if (event->button.button == SDL_BUTTON_RIGHT) gui_mouse_pressed[1] = true;
        if (event->button.button == SDL_BUTTON_MIDDLE) gui_mouse_pressed[2] = true;
        return true;
    }
    case SDL_TEXTINPUT: {
        io.AddInputCharactersUTF8(event->text.text);
        return true;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
        int key = event->key.keysym.sym & ~SDLK_SCANCODE_MASK;
        io.KeysDown[key] = (event->type == SDL_KEYDOWN);
        io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
        io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
        io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
        io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
        return true;
    }
    }
    return false;
}

bool ProcessSDLEvent(SDL_Event evt, Scene* scene, double deltaTime)
{
    if (evt.type == SDL_QUIT) {
        return true;
    }

    float mouseSensitivity = 0.17;

    switch (evt.type) {
    case SDL_WINDOWEVENT:
        if (evt.window.event == SDL_WINDOWEVENT_RESIZED) {
            int width = evt.window.data1;
            int height = evt.window.data2;
            ResizeWindow(width, height, scene);
        }
        break;
    case SDL_KEYDOWN:
        if (evt.key.keysym.scancode == SDL_SCANCODE_W) {
            MOVING_FORWARD = true;
        }
        if (evt.key.keysym.scancode == SDL_SCANCODE_S) {
            MOVING_BACKWARD = true;
        }
        if (evt.key.keysym.scancode == SDL_SCANCODE_A) {
            STRAFING_LEFT = true;
        }
        if (evt.key.keysym.scancode == SDL_SCANCODE_D) {
            STRAFING_RIGHT = true;
        }
        break;
    case SDL_KEYUP:
        if (evt.key.keysym.scancode == SDL_SCANCODE_W) {
            MOVING_FORWARD = false;
        }
        if (evt.key.keysym.scancode == SDL_SCANCODE_S) {
            MOVING_BACKWARD = false;
        }
        if (evt.key.keysym.scancode == SDL_SCANCODE_A) {
            STRAFING_LEFT = false;
        }
        if (evt.key.keysym.scancode == SDL_SCANCODE_D) {
            STRAFING_RIGHT = false;
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (evt.button.button == SDL_BUTTON_RIGHT) {
            MOUSE_MOVE = true;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (evt.button.button == SDL_BUTTON_RIGHT) {
            MOUSE_MOVE = false;
            ROTATING_LEFT = false;
            ROTATING_RIGHT = false;
            ROTATING_UP = false;
            ROTATING_DOWN = false;
        }
        break;
    case SDL_MOUSEMOTION:
        if (MOUSE_MOVE) {
            if (evt.motion.xrel < 0) {
                RotateCameraX(scene, 0.1f, evt.motion.xrel * mouseSensitivity);
            }
            if (evt.motion.xrel > 0) {
                RotateCameraX(scene, 0.1f, evt.motion.xrel * mouseSensitivity);
            }
            if (evt.motion.yrel < 0) {
                RotateCameraY(scene, 0.1f, evt.motion.yrel * mouseSensitivity);
            }
            if (evt.motion.yrel > 0) {
                RotateCameraY(scene, 0.1f, evt.motion.yrel * mouseSensitivity);
            }
        }

        break;
    }

    return false;
}

void DoMovement(Scene* scene, double deltaTime)
{
    Camera& cam = scene->GetCamera();
    auto player = scene->GetPlayer();
    float speed = 7.5f;
    float rotSpeed = 120.0f;
    float moveBy = speed * deltaTime;
    float rotBy = rotSpeed * deltaTime;


    if (MOVING_FORWARD) {
        //player->StepPlayer(deltaTime);
        cam.Translate( { 0.0f, 0.0f, -moveBy });
    }
    if (MOVING_BACKWARD) {
        cam.Translate( { 0.0f, 0.0f, moveBy });
    }
    if (STRAFING_LEFT) {
        cam.Translate( { -moveBy, 0.0f, 0.0f });
    }
    if (STRAFING_RIGHT) {
        cam.Translate( { moveBy, 0.0f, 0.0f });
    }
    if (ROTATING_LEFT) {
        cam.Rotate(0.0f, 0.0f, rotBy);
    }
    if (ROTATING_RIGHT) {
        cam.Rotate(0.0f, 0.0f, -rotBy);
    }
    if (ROTATING_UP) {
        cam.Rotate(rotBy, 0.0f, 0.0f);
    }
    if (ROTATING_DOWN) {
        cam.Rotate(-rotBy, 0.0f, 0.0f);
    }
}

void RotateCameraX(Scene* scene, double deltaTime, float degrees)
{
    Camera& cam = scene->GetCamera();
    cam.Rotate(0.f, 0.f, -degrees);
}

void RotateCameraY(Scene* scene, double deltaTime, float degrees)
{
    Camera& cam = scene->GetCamera();
    cam.Rotate(-degrees, 0.f , 0.f);
}

//HWND GLWindow::GetWindowHandle() const
//{
//	return glfwGetWin32Window(m_window);
//}


//void ProcessInput(GLFWwindow* window, Scene* scene, float deltaTime) {
//
//	//glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
//
//	static double previousMouseX = 0;
//	static double previousMouseY = 0;
//
//	float speed = 7.5f;
//	float rotSpeed = 120.0f;
//	float moveBy = speed * deltaTime;
//	float rotBy = rotSpeed * deltaTime;
//
//	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
//		MOVING_FORWARD = true;
//	}
//	else {
//		MOVING_FORWARD = false;
//	}
//
//	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
//		MOVING_BACKWARD = true;
//	}
//	else {
//		MOVING_BACKWARD = false;
//	}
//	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
//		STRAFING_LEFT = true;
//	}
//	else {
//		STRAFING_LEFT = false;
//	}
//	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
//		STRAFING_RIGHT = true;
//	}
//	else {
//		STRAFING_RIGHT = false;
//	}
//	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
//		ROTATING_LEFT = true;
//	}
//	else {
//		ROTATING_LEFT = false;
//	}
//	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
//		ROTATING_RIGHT = true;
//	}
//	else {
//		ROTATING_RIGHT = false;
//	}
//
//	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
//		ROTATING_UP = true;
//	}
//	else {
//		ROTATING_UP = false;
//	}
//
//	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
//		ROTATING_DOWN = true;
//	}
//	else {
//		ROTATING_DOWN = false;
//	}
//
//	Camera& cam = scene->GetCamera();
//
//	int mb = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
//	if (mb == GLFW_PRESS) {
//		double mouseX, mouseY;
//		glfwGetCursorPos(window, &mouseX, &mouseY);
//		scene->DoLeftClick(mouseX, mouseY);
//	}
//	/*if (mb == GLFW_PRESS) {
//		double mouseX, mouseY;
//		glfwGetCursorPos(window, &mouseX, &mouseY);
//		double xDelta = (mouseX - previousMouseX) * deltaTime * 30.0f;
//		double yDelta = (mouseY - previousMouseY) * deltaTime * 30.0f;
//		previousMouseX = mouseX;
//		previousMouseY = mouseY;
//		cam.Rotate(-yDelta, 0.f, xDelta);
//	}*/
//
//
//	if (MOVING_FORWARD) {
//		cam.Translate({ 0.0f, 0.0f, -moveBy });
//	}
//	if (MOVING_BACKWARD) {
//		cam.Translate({ 0.0f, 0.0f, moveBy });
//	}
//	if (STRAFING_LEFT) {
//		cam.Translate({ -moveBy, 0.0f, 0.0f });
//	}
//	if (STRAFING_RIGHT) {
//		cam.Translate({ moveBy, 0.0f, 0.0f });
//	}
//	if (ROTATING_LEFT) {
//		cam.Rotate(0.0f, 0.0f, rotBy);
//	}
//	if (ROTATING_RIGHT) {
//		cam.Rotate(0.0f, 0.0f, -rotBy);
//	}
//	if (ROTATING_UP) {
//		cam.Rotate(rotBy, 0.0f, 0.0f);
//	}
//	if (ROTATING_DOWN) {
//		cam.Rotate(-rotBy, 0.0f, 0.0f);
//	}
//}
//
//
//void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
//	glViewport(0, 0, width, height);
//	Scene* scene = (Scene*)glfwGetWindowUserPointer(window);
//	scene->ProjectionMatrix().SetToIdentity();
//	scene->ProjectionMatrix().CreatePerspectiveProjection(80, (float)width / (float)height, 0.1f, 500.0f);
//
//	scene->OrthographicMatrix().CreateOrthographicProjection(0.0f, width, height, 0.0f, 1.0f, -1.0f);
//}

}
