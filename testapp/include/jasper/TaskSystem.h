#pragma once

#include <atomic>
#include <memory>
#include <future>

namespace TaskSystem {

struct ITask {

};

struct TrackedTask {

};

void Initialize();
void Destroy();
void Process();

void AddTask(std::unique_ptr<ITask> task);
std::future<bool> AddTrackedTask(std::unique_ptr<ITask> task, ITask* parent);

}