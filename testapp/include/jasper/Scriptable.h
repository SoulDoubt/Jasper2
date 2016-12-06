#pragma once

#include "Component.h"
#include "GameObject.h"
#include <chrono>


namespace Jasper
{

class ScriptComponent : public Component
{

public:

    ScriptComponent(std::string name): Component(std::move(name)) {}

    ~ScriptComponent() {}

    void Initialize() override {}
    void Destroy() override {}
    void Awake() override {}
    void Start() override {}
    void FixedUpdate() override {}
    void Update(float dt) override {}
    void LateUpdate() override {}
    void Serialize(std::ofstream& ofs) const override;
    ComponentType GetComponentType() const override {
        return ComponentType::ScriptComponent;
    }

};

inline void ScriptComponent::Serialize(std::ofstream& ofs) const {
    // just write the component name and type for now, the
    
}

class DefaultScript final : public ScriptComponent
{

public:

    DefaultScript() : ScriptComponent("default_script") {}
    DefaultScript(std::string name) : ScriptComponent(name) {}
    ~DefaultScript() {}

    void Initialize() override {}
    void Destroy()override {}
    void Awake()override {}
    void Start()override {}
    void FixedUpdate()override {}
    void Update(float dt) override;
    void LateUpdate()override {}
    ComponentType GetComponentType() const override {
        return ComponentType::DefaultScript;
    }

};

class RotateAboutPointScript final: public ScriptComponent
{

public:

    RotateAboutPointScript(const Vector3& point, const Vector3& axis, const float degsPerSecond)
        : ScriptComponent("rotate_light_script"), m_point(point), m_axis(axis), m_degreesPerSec(degsPerSecond) {

    }

    RotateAboutPointScript(std::string name, const Vector3& point, const Vector3& axis, const float degsPerSecond)
        : ScriptComponent(name), m_point(point), m_axis(axis), m_degreesPerSec(degsPerSecond) {
    }
    ~RotateAboutPointScript() {}

    void Initialize() override {}
    void Destroy()override {}
    void Awake()override {}
    void Start()override {}
    void FixedUpdate()override {}
    void Update(float dt) override;
    void LateUpdate()override {}
    bool ShowGui() override;
    ComponentType GetComponentType() const override {
        return ComponentType::RotateAboutPointScript;
    }


private:

    Vector3 m_point;
    Vector3 m_axis;
    float m_degreesPerSec;


};

class LauncherScript final : public ScriptComponent
{

public:
    LauncherScript() : ScriptComponent("launcher_script") {}
    LauncherScript(std::string name) : ScriptComponent(name) {}
    ~LauncherScript() {}

    void Initialize() override {}
    void Destroy()override {}
    void Awake()override {}
    void Start()override {}
    void FixedUpdate()override {}
    void Update(float dt) override;
    void LateUpdate()override {}
    bool ShowGui() override;
    ComponentType GetComponentType() const override {
        return ComponentType::LauncherScript;
    }


    Vector3 Force = {0.0f, 500.0f, 0.0f};

private:

    int launch_interval = 500;
    std::chrono::high_resolution_clock::time_point time_last_launch;
    int launch_count = 0;

    void LaunchTeapot();
    void LaunchSphere();
    void LaunchCube();
};

class DestroyScript final : public ScriptComponent
{
public:
    DestroyScript() : ScriptComponent("destroy_script") {}
    DestroyScript(std::string name) : ScriptComponent(name) {}
    ~DestroyScript() {}

    void Initialize() override {}
    void Destroy()override {}
    void Awake()override {}
    void Start()override {}
    void FixedUpdate()override {}
    void Update(float dt) override;
    void LateUpdate()override {}
    ComponentType GetComponentType() const override {
     return ComponentType::DestroyScript;
    }
};

class RotateInPlaceScript final : public ScriptComponent
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
    ComponentType GetComponentType() const override {
     return ComponentType::RotateInPlaceScript;
    }

private:

    Vector3 m_axis;
    float m_degreesPerSecond;
};

}
