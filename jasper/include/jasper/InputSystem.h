#pragma once


namespace Jasper {

class InputSystem {

private:
	InputSystem();
	static InputSystem* m_instance;

public:

	~InputSystem();

	static InputSystem& GetInstance();

	bool MOVING_FORWARD  = false;
	bool MOVING_BACKWARD = false;
	bool STRAFING_LEFT   = false;
	bool STRAFING_RIGHT  = false;

	bool ROTATING_RIGHT  = false;
	bool ROTATING_LEFT   = false;
	bool ROTATING_UP     = false;
	bool ROTATING_DOWN   = false;

	bool MOUSE_MOVE      = false;

	bool QUIT_REQUESTED  = false;

	bool SHOW_EDITOR     = false;

	bool MOUSE_MOTION    = false;

	int MOUSE_XREL = 0;
	int MOUSE_YREL = 0;

	float MouseSensitivity = 0.17f;

};

}