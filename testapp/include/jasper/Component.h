#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "Common.h"
#include <string>


namespace Jasper
{

class GameObject;
//class std::ofstream

enum class ComponentType{
    None,
    MeshRenderer,
    DestroyScript,
    DefaultScript,
    RotateAboutPointScript,
    RotateInPlaceScript,
    LauncherScript,
    Mesh,
    Cube,
    Quad,
    Model,
    PhysicsCollider,
    SkyboxRenderer,
    Triangle,
    Sphere,
    ScriptComponent
};

class Component
{
public:

    int ComponentID = 0;

    //Component();
    Component(std::string name);

    std::string GetName() const;
    void SetName(std::string name);
    bool IsEnabled() const {
        return m_isEnabled;
    }
    
    virtual void ToggleEnabled(bool enabled);
    
    void SetGameObject(GameObject* go);
    GameObject* GetGameObject() const;



    virtual ~Component();

    virtual void Initialize();
    virtual void Destroy();
    virtual void Awake();
    virtual void Start();
    virtual void FixedUpdate();
    virtual void Update(float dt);
    virtual void LateUpdate();
    virtual bool ShowGui();
    virtual void Serialize(std::ofstream& ofs) const;
    virtual ComponentType GetComponentType() const {
        return ComponentType::None;
    }



private:

    NON_COPYABLE(Component);

    std::string m_name;
    GameObject* m_gameObject;
    


    protected:
    
    bool m_isEnabled = true;
    bool m_isDestroyed = false;

};

//inline Component::Component() {
//	m_name = "Unnamed Component";
//	m_isEnabled = true;
//	m_gameObject = nullptr;
//}

inline Component::Component(std::string name)
{
    m_name = std::move(name);
    m_isEnabled = true;
    m_gameObject = nullptr;
}

inline std::string Component::GetName() const
{
    return m_name;
}

inline void Component::SetName(std::string name)
{
    m_name = std::move(name);
}


inline void Component::SetGameObject(GameObject* go)
{
    m_gameObject = go;
}

inline GameObject* Component::GetGameObject() const
{
    return m_gameObject;
}

}
#endif // _COMPONENT_H_
