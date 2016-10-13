#pragma once 

#include "Component.h"
#include "GameObject.h"
#include <chrono>


namespace Jasper{
    
class ScriptComponent : public Component{

    public:

    ScriptComponent() : Component() {}
    ScriptComponent(std::string name): Component(name){}
    
    ~ScriptComponent(){}

    virtual void Initialize() override {}
	virtual void Destroy() override {}
	virtual void Awake() override {}
	virtual void Start() override {}
	virtual void FixedUpdate() override {}
	virtual void Update(float dt) override {}
	virtual void LateUpdate() override {}

};

class DefaultScript: public ScriptComponent{    
    
public:
    
    DefaultScript() : ScriptComponent(){}
    DefaultScript(std::string name) : ScriptComponent(name){}
    ~DefaultScript(){}
    
    virtual void Initialize() override{}
	virtual void Destroy()override{}
	virtual void Awake()override{}
	virtual void Start()override{}
	virtual void FixedUpdate()override{}
	virtual void Update(float dt) override;            
	virtual void LateUpdate()override{}
    
};

class RotateLightScript : public ScriptComponent{

public:
    
    RotateLightScript() : ScriptComponent(){}
    RotateLightScript(std::string name) : ScriptComponent(name){}
    ~RotateLightScript(){}
    
    virtual void Initialize() override{}
	virtual void Destroy()override{}
	virtual void Awake()override{}
	virtual void Start()override{}
	virtual void FixedUpdate()override{}
	virtual void Update(float dt) override;            
	virtual void LateUpdate()override{}    
};

class LauncherScript: public ScriptComponent{

public:
    LauncherScript() : ScriptComponent(){}
    LauncherScript(std::string name) : ScriptComponent(name){}
    ~LauncherScript(){}
    
    virtual void Initialize() override{}
	virtual void Destroy()override{}
	virtual void Awake()override{}
	virtual void Start()override{}
	virtual void FixedUpdate()override{}
	virtual void Update(float dt) override;            
	virtual void LateUpdate()override{}

    Vector3 Force = {0.0f, 500.0f, 0.0f};
    
private:
    
    int launch_interval = 500;
    std::chrono::high_resolution_clock::time_point time_last_launch;
    int launch_count = 0;
    
    void LaunchTeapot();
    void LaunchSphere();
    void LaunchCube();
};

class DestroyScript: public ScriptComponent{
    public:
    DestroyScript() : ScriptComponent(){}
    DestroyScript(std::string name) : ScriptComponent(name){}
    ~DestroyScript(){}
    
    virtual void Initialize() override{}
	virtual void Destroy()override{}
	virtual void Awake()override{}
	virtual void Start()override{}
	virtual void FixedUpdate()override{}
	virtual void Update(float dt) override;
            
	virtual void LateUpdate()override{} 
};

}

