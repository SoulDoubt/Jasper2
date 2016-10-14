#include <iostream>
#include "GLWindow.h"
#include "GLError.h"
#include "Scene.h"

//#define GLFW_EXPOSE_NATIVE_WIN32
//#define GLFW_EXPOSE_NATIVE_WGL
//#include <GLFW\glfw3native.h>

#include <chrono>

#include <unistd.h>

int main(int argc, char** argv){
    
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


namespace Jasper {

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



//void ProcessInput(GLFWwindow* window, Scene* scene, float deltaTime);

//void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

bool ProcessSDLEvent(SDL_Event evt, Scene* scene, double deltaTime);

void DoMovement(Scene* scene, double deltaTime);

void RotateCameraX(Scene* scene, double deltaTime, float degrees);

void RotateCameraY(Scene* scene, double deltaTime, float degrees);


GLWindow::GLWindow(int w, int h, string title)
{
	Height = h;
	Width = w;
	Title = title;
}

GLWindow::~GLWindow()
{
}

bool GLWindow::Init() {
    
    printf("Inside window initialization...\n");

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return false;
	}

	//m_window = glfwCreateWindow(Width, Height, Title.c_str(), nullptr, nullptr);
	m_window = SDL_CreateWindow(Title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	m_context = SDL_GL_CreateContext(m_window);

	/*if (!m_window) {
		glfwTerminate();
		return false;
	}*/

	//glfwMakeContextCurrent(m_window);

	GLenum glewStatus = glewInit();
	if (glewStatus != GLEW_OK) {
        printf("GLEW Initialization Failed\n");
		return false;
	}
    else{
        printf("GLEW Initialized successfully\n");
    
    }

	glViewport(0, 0, Width, Height);
	//glfwSetFramebufferSizeCallback(m_window, framebuffer_resize_callback);

	printGLInfo();

	SetupGL();

	return true;
}

void GLWindow::RunLoop() {

	bool loop = true;
	high_resolution_clock::time_point previousTime = high_resolution_clock::now();

	while (loop) {
		//while (!glfwWindowShouldClose(m_window)) {
		

		high_resolution_clock::time_point currentTime = high_resolution_clock::now();
		auto timeDiff = duration_cast<nanoseconds>(currentTime - previousTime);
		previousTime = currentTime;
		double timeDelta = timeDiff.count();
		double dt = timeDelta / 1000000000;

		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			if (ProcessSDLEvent(evt, m_scene.get(), dt)) {
				loop = false;
			}
		}
		DoMovement(m_scene.get(), dt);
		// DO ALL THE THINGS HERE!!:)
		//ProcessInput(m_window, m_scene.get(), deltaTime);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_scene->Update(dt);
		SDL_GL_SwapWindow(m_window);
		//glfwSwapBuffers(m_window);
		//glfwPollEvents();

	}
	SDL_DestroyWindow(m_window);
	m_scene->Destroy();
	//glfwTerminate();
}

void GLWindow::InitializeScene()
{
	m_scene = make_unique<Scene>(Width, Height);
	m_scene->Awake();
	//glfwSetWindowUserPointer(m_window, m_scene.get());
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

void ResizeWindow(int w, int h, Scene* scene){
    //printf("Window resized\n");
    //printf("Width: %d, Height: %d \n", w, h);
    glViewport(0, 0, w, h);
    
    scene->Resize(w, h);
    
}

bool ProcessSDLEvent(SDL_Event evt, Scene* scene, double deltaTime) {
	if (evt.type == SDL_QUIT) {
		return true;
	}

	float mouseSensitivity = 0.17;

	switch (evt.type) {
    case SDL_WINDOWEVENT:
        if (evt.window.event == SDL_WINDOWEVENT_RESIZED){
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

void DoMovement(Scene* scene, double deltaTime) {
	Camera& cam = scene->GetCamera();
	auto player = scene->GetPlayer();
	float speed = 7.5f;
	float rotSpeed = 120.0f;
	float moveBy = speed * deltaTime;
	float rotBy = rotSpeed * deltaTime;


	if (MOVING_FORWARD) {
		//player->StepPlayer(deltaTime);
		cam.Translate({ 0.0f, 0.0f, -moveBy });
	}
	if (MOVING_BACKWARD) {
		cam.Translate({ 0.0f, 0.0f, moveBy });
	}
	if (STRAFING_LEFT) {
		cam.Translate({ -moveBy, 0.0f, 0.0f });
	}
	if (STRAFING_RIGHT) {
		cam.Translate({ moveBy, 0.0f, 0.0f });
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

void RotateCameraX(Scene* scene, double deltaTime, float degrees) {
	Camera& cam = scene->GetCamera();
	cam.Rotate(0.f, 0.f, -degrees);
}

void RotateCameraY(Scene* scene, double deltaTime, float degrees) {
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
