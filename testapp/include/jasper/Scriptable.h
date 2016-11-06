#pragma once

#include "Component.h"
#include "GameObject.h"
#include <chrono>


namespace Jasper
{

class ScriptComponent : public Component
{

public:

    ScriptComponent(std::string name): Component(name) {}

    ~ScriptComponent() {}

    virtual void Initialize() override {}
    virtual void Destroy() override {}
    virtual void Awake() override {}
    virtual void Start() override {}
    virtual void FixedUpdate() override {}
    virtual void Update(float dt) override {}
    virtual void LateUpdate() override {}

};

class DefaultScript: public ScriptComponent
{

public:

    DefaultScript() : ScriptComponent("default_script") {}
    DefaultScript(std::string name) : ScriptComponent(name) {}
    ~DefaultScript() {}

    virtual void Initialize() override {}
    virtual void Destroy()override {}
    virtual void Awake()override {}
    virtual void Start()override {}
    virtual void FixedUpdate()override {}
    virtual void Update(float dt) override;
    virtual void LateUpdate()override {}

};

class RotateAboutPointScript : public ScriptComponent
{

public:

    RotateAboutPointScript(const Vector3& point, const Vector3& axis, const float degsPerSecond)
        : ScriptComponent("rotate_light_script"), m_point(point), m_axis(axis), m_degreesPerSec(degsPerSecond) {

    }

    RotateAboutPointScript(std::string name, const Vector3& point, const Vector3& axis, const float degsPerSecond)
        : ScriptComponent(name), m_point(point), m_axis(axis), m_degreesPerSec(degsPerSecond) {
    }
    ~RotateAboutPointScript() {}

    virtual void Initialize() override {}
    virtual void Destroy()override {}
    virtual void Awake()override {}
    virtual void Start()override {}
    virtual void FixedUpdate()override {}
    virtual void Update(float dt) override;
    virtual void LateUpdate()override {}



private:

    Vector3 m_point;
    Vector3 m_axis;
    float m_degreesPerSec;


};

class LauncherScript: public ScriptComponent
{

public:
    LauncherScript() : ScriptComponent("launcher_script") {}
    LauncherScript(std::string name) : ScriptComponent(name) {}
    ~LauncherScript() {}

    virtual void Initialize() override {}
    virtual void Destroy()override {}
    virtual void Awake()override {}
    virtual void Start()override {}
    virtual void FixedUpdate()override {}
    virtual void Update(float dt) override;
    virtual void LateUpdate()override {}

    Vector3 Force = {0.0f, 500.0f, 0.0f};

private:

    int launch_interval = 500;
    std::chrono::high_resolution_clock::time_point time_last_launch;
    int launch_count = 0;

    void LaunchTeapot();
    void LaunchSphere();
    void LaunchCube();
};

class DestroyScript: public ScriptComponent
{
public:
    DestroyScript() : ScriptComponent("destroy_script") {}
    DestroyScript(std::string name) : ScriptComponent(name) {}
    ~DestroyScript() {}

    virtual void Initialize() override {}
    virtual void Destroy()override {}
    virtual void Awake()override {}
    virtual void Start()override {}
    virtual void FixedUpdate()override {}
    virtual void Update(float dt) override;

    virtual void LateUpdate()override {}
};

class RotateInPlaceScript : public ScriptComponent
{
public:
    RotateInPlaceScript(const Vector3& axis, float degsPerSecond)
        : ScriptComponent("rotate_in_place_script"), m_axis(axis), m_degreesPerSecond(degsPerSecond) {
    }
    RotateInPlaceScript(std::string name, const Vector3& axis, float degsPerSecond)
        : ScriptComponent(name), m_axis(axis), m_degreesPerSecond(degsPerSecond) {}
    ~RotateInPlaceScript() {}

    void Initialize() override {}
    void Destroy() override {}
    void Awake() override {}
    void Start() override {}
    void FixedUpdate() override {}
    void Update(float dt) override;
    void LateUpdate() override {}

private:

    Vector3 m_axis;
    float m_degreesPerSecond;
};

}
