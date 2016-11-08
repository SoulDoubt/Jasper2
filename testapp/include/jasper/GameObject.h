#ifndef _JASPER_GAME_OBJECT_H_
#define _JASPER_GAME_OBJECT_H_

#include "Common.h"
#include <string>
#include <algorithm>
#include <typeinfo>
#include <map>
#include <typeindex>
#include <functional>
#include <vector>
#include <memory>
#include "Component.h"
#include "Transform.h"
#include "Event.h"
#include <chrono>
#include "Scriptable.h"

namespace Jasper
{

class Scene;


class GameObject
{
public:


    using GroupedComponentMap = std::map<std::type_index, std::vector<std::unique_ptr<Component>>>;

    //GameObject();
    GameObject(const std::string& name);
    GameObject(GameObject&& oth) = default;
    virtual ~GameObject();

    Transform GetWorldTransform() const;

    std::string GetName() const;
    void SetName(std::string name);

    std::string GetTag() const;
    void SetTag(std::string tag);

    const std::vector<std::unique_ptr<GameObject>>& Children() const;

    std::vector<std::unique_ptr<GameObject>>& Children() {
        return m_children;
    }

    std::vector<std::unique_ptr<Component>>& Components();

    Transform GetLocalTransform() const;

    Transform& GetLocalTransform() {
        return m_transform;
    }

    std::chrono::high_resolution_clock::time_point TimeAwakened;

    std::chrono::high_resolution_clock::time_point GetTimeAwakened() const {
        return TimeAwakened;
    }

    void SetLocalTransform(const Transform& newTransform) {
        m_transform = newTransform;
    }

    virtual void Initialize();
    virtual void Destroy();
    virtual void Awake();
    virtual void Start();
    virtual void FixedUpdate();
    virtual void Update(float dt);
    virtual void LateUpdate();
    virtual bool ShowGui();

    GameObject& AttachComponent(std::unique_ptr<Component> component);

    std::unique_ptr<Component> DetachComponent(Component& copm);

    GameObject& AttachChild(std::unique_ptr<GameObject> child);
    GameObject& AttachNewChild(const std::string& name);

    std::unique_ptr<GameObject> DetachChild(const GameObject& child);


    Component* FindComponentByName(std::string name);
    GameObject* FindChildByName(std::string name);
    Component* GetComponentByID(int id);

    void Enable() {
        m_enabled = true;
    }
    void Disable() {
        m_enabled = false;
    }
    bool IsEnabled() const {
        return m_enabled;
    }
    void SetParemt(GameObject* parent);

    template <typename T>
    T* FindFirstComponentByType();

    Component* FindComponentByID(int id);

    void SetScene(Scene* scene);
    Scene* GetScene() const;

    template<typename T>
    std::vector<std::unique_ptr<Component>>* FindComponentsByType();

    template<typename T, typename... Args>
    T* AttachNewComponent(Args&&... args);

    template<typename T>
    T* GetComponentByType();

    template<typename T>
    std::vector<T*> GetComponentsByType();

    template<typename T, typename... Args>
    T* AttachNewChild(Args&&... args);

    Event<float> UpdateEvent;

    GameObject* GetParent() const {
        return m_parent;
    }


private:
    std::string m_name;
    std::string m_tag;
    GameObject* m_parent;

    bool m_enabled;

    std::vector<std::unique_ptr<Component>> m_components;
    std::vector<std::unique_ptr<GameObject>> m_children;

    GroupedComponentMap m_groupedComponents;


protected:
    Scene* m_scene;
    Transform m_transform;
    bool m_isDestroyed = false;
    bool m_isRenderable = false;
    virtual void InitializeCurrent();
    void InitializeChildren();
    virtual void AwakeCurrent();
    void AwakeChildren();
    virtual void StartCurrent();
    void StartChildren();
    virtual void UpdateCurrent(float dt);
    void UpdateChildren(float dt);
    virtual void DestroyCurrent();
    void DestroyChildren();


};
//
//inline GameObject::GameObject() : m_components(), m_children() {
//	m_name = "Unnamed Game object";
//	m_tag = "";
//	m_parent = nullptr;
//	//Initialize();
//}

inline GameObject::GameObject(const std::string& name) : m_components(), m_children(), m_transform()
{
    m_name = name;
    m_tag = "";
    m_parent = nullptr;
    m_transform.SetIdentity();
    m_transform.Position = { 0.0f, 0.0f, 0.0f };
    //Initialize();
}

inline std::string GameObject::GetName() const
{
    return m_name;
}

inline void GameObject::SetName(std::string name)
{
    m_name = name;
}

inline std::string GameObject::GetTag() const
{
    return m_tag;
}

inline void GameObject::SetTag(std::string tag)
{
    m_tag = tag;
}

inline const std::vector<std::unique_ptr<GameObject>>& GameObject::Children() const
{
    return m_children;
}

inline std::vector<std::unique_ptr<Component>>&  GameObject::Components()
{
    return m_components;
}

inline Transform GameObject::GetLocalTransform() const
{
    return m_transform;
}

inline void GameObject::SetParemt(GameObject* parent)
{
    m_parent = parent;
}

inline void GameObject::SetScene(Scene * scene)
{
    m_scene = scene;
}

inline Scene * GameObject::GetScene() const
{
    return m_scene;
}


template <typename T>
T* GameObject::FindFirstComponentByType()
{

    static_assert(std::is_base_of<Component, T>::value, "Type Paramater must derive from Component.");

    std::vector<std::unique_ptr<Component>>* c = FindComponentsByType<T>();
    if (c) {
        return (T*)(c->at(0).get());
    }
    return nullptr;
}

inline Component* GameObject::FindComponentByID(int id)
{
    auto found = std::find_if(begin(m_components), end(m_components),
    [&](const std::unique_ptr<Component>& c) {
        return c->ComponentID == id;
    });
    if (found != end(m_components)) {
        return found->get();
    }
    return nullptr;
}

template<typename T>
inline  std::vector<std::unique_ptr<Component>>* GameObject::FindComponentsByType()
{

    static_assert(std::is_base_of<Component, T>::value, "Type Paramater must derive from Component.");

    if (m_groupedComponents.size() == 0) {
        return nullptr;
    }
    const std::type_index id(typeid(T));
    for (auto& group : m_groupedComponents) {
        if (group.first == id) {
            return &m_groupedComponents[id];
        }
    }
    return nullptr;
}

template<typename T, typename... Args>
T* GameObject::AttachNewChild(Args&&... args)
{

    static_assert(std::is_base_of<GameObject, T>::value, "Type Paramater must derive from GameObject.");

    auto child = std::make_unique<T>(std::forward<Args>(args)...);
    child->SetParemt(this);
    child->SetScene(this->m_scene);
    T* ret = child.get();
    m_children.push_back(move(child));
    //printf("Current Child Collection...\n");
    //for (auto& ch : m_children){
    //    printf("%s\n", ch->GetName().c_str());
    //}
    return ret;
}


inline GameObject& GameObject::AttachComponent(std::unique_ptr<Component> component)
{

    component->SetGameObject(this);
    component->ComponentID = m_components.size();
    //m_groupedComponents[std::type_index(typeid(*component))].push_back(move(component));
    m_components.push_back(std::move(component));
    return *this;
}

template<typename T, typename... Args>
inline T* GameObject::AttachNewComponent(Args&&... args)
{

    static_assert(std::is_base_of<Component, T>::value, "Type Paramater must derive from Component.");

    auto upc = std::make_unique<T>(std::forward<Args>(args)...);
    T* p = upc.get();
    this->AttachComponent(std::move(upc));
    return p;
}

template<typename T>
inline T * GameObject::GetComponentByType()
{

    static_assert(std::is_base_of<Component, T>::value, "Type Paramater must derive from Component.");

    for (auto& c : m_components) {
        if (T* found = dynamic_cast<T*>(c.get())) {
            return found;
        }
    }
    return nullptr;
}

template<typename T>
inline std::vector<T*> GameObject::GetComponentsByType()
{

    static_assert(std::is_base_of<Component, T>::value, "Type Paramater must derive from Component.");

    std::vector<T*> ret;
    for (auto& c : m_components) {
        if (T* found = dynamic_cast<T*>(c.get())) {
            ret.push_back(found);
        }
    }
    for (const auto& child : m_children) {
        for (auto& c : child->m_components) {
            if (T* found = dynamic_cast<T*>(c.get())) {
                ret.push_back(found);
            }
        }
    }
    return ret;
}

}


#endif // _JASPER_GAME_OBJECT_H_
