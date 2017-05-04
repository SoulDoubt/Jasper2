#include "include\jasper\TaskSystem.h"

void TaskSystem::Initialize()
{
}

void TaskSystem::Destroy()
{
}

void TaskSystem::Process()
{
}

void TaskSystem::AddTask(std::unique_ptr<ITask> task)
{
}

std::future<bool> TaskSystem::AddTrackedTask(std::unique_ptr<ITask> task, ITask * parent)
{
	return std::future<bool>();
}
