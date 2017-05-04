#include <InputSystem.h>

namespace Jasper {

InputSystem* InputSystem::m_instance = nullptr;

InputSystem::InputSystem() {}

InputSystem::~InputSystem()
{
	if (m_instance != nullptr) {
		delete m_instance;
	}
}

InputSystem& Jasper::InputSystem::GetInstance()
{
	if (m_instance == nullptr) {
		m_instance = new InputSystem();
	}
	return *m_instance;
}

}
