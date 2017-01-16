#pragma once

#include <thread.h>
#include <atomic.h>


struct ITask {

    virtual void Execute() = 0;
    std::atomic<int> IsRunning;
};
