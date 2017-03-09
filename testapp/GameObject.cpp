#include "GameObject.h"
#include "PhysicsCollider.h"
#include "Scriptable.h"
#include <chrono>
#include "imgui.h"

namespace Jasper
{

using namespace std;
using namespace std::chrono;

GameObject::~GameObject()
{
    //if (!m_isDestroyed) {
    Destroy();
    //}
}

bool GameObject::ShowGui()
{
    Vector3 scale = m_transform.Scale;
    ImGui::Separator();
    ImGui::Text(this->GetName().data());
    bool rotation_updated = false;
    ImGui::InputFloat3("Position", m_transform.Position.AsFloatPtr());
    ImGui::InputFloat4("Orientation", m_transform.Orientation.AsFloatPtr());
    if (ImGui::InputFloat3("Scale", scale.AsFloatPtr())) {
        if (scale.x > 0.f && scale.y > 0.f && scale.z > 0.f) {
            m_transform.Scale = scale;
            auto col = GetComponentByType<PhysicsCollider>();
            if (col) {
                col->SetScale(scale);
            }
        }
    }
    auto euler = m_transform.Orientation.ToEulerAngles();
    float roll_radians = euler.Roll;
    if (ImGui::SliderAngle("X", &roll_radians, -180, 180)) {
        rotation_updated = true;
    }
    float yaw_radians = euler.Yaw;
    if (ImGui::SliderAngle("Y", &yaw_radians, -180, 180)) {
        rotation_updated = true;
    }
    float pitch_radians = euler.Pitch;
    if (ImGui::SliderAngle("Z", &pitch_radians, -180, 180)) {
        rotation_updated = true;
    }
    if (rotation_updated) {
        Quaternion q(pitch_radians, roll_radians, yaw_radians);
        m_transform.Orientation = q;
    }
    if (ImGui::Button("Reset")) {
        pitch_radians = 0;
        roll_radians = 0;
        yaw_radians = 0;
        Quaternion q(pitch_radians, roll_radians, yaw_radians);
        m_transform.Orientation = q;
    }
    ImGui::Separator();

    //if (ImGui::CollapsingHeader("Components")){
    ImGui::Text("Components");
    int i = 0;
    for (auto& cmp : m_components) {
        if (ImGui::TreeNode((void*)(uintptr_t)i, cmp->GetName().data())) {
            if (ImGui::BeginPopupContextItem("item context menu")) {
                std::unique_ptr<Component> value;;
                if (ImGui::Selectable("Detatch")) {
                    value = this->DetachComponent(cmp.get());
                    printf("Detached game object: %s\n", value->GetName().data());
                    ImGui::TreePop();
                    ImGui::PopID();
                    return false;
                }
                if (ImGui::Selectable("Set to PI")) value = nullptr;
                ImGui::EndPopup();
            }
            cmp->ShowGui();
            ImGui::TreePop();
        }
        i++;
    }

    return false;
}


GameObject& GameObject::AttachChild(std::unique_ptr<GameObject> child)
{
    child->SetParent(this);
    child->SetScene(this->m_scene);
	for (auto& ch : child->Children()) {
		ch->SetScene(this->m_scene);
	}
    GameObject* ret = child.get();
    m_children.push_back(move(child));
    return *ret;
}

std::unique_ptr<Component> GameObject::DetachComponent(Component* comp)
{
    auto found = find_if(begin(m_components), end(m_components),
    [&](const unique_ptr<Component>& c) {
        printf("component address: %p\n", c.get());
        return c.get() == comp;
    });
    if (found != end(m_components)) {
        auto& goup = *found;
        auto result = std::move(goup);
        result->SetGameObject(nullptr);

        m_components.erase(remove(begin(m_components), end(m_components), *found));
        return result;
    } else {
        return nullptr;
    }
}

unique_ptr<GameObject> GameObject::DetachChild(const GameObject& child)
{
    auto found = remove_if(begin(m_children), end(m_children),
    [&child](const unique_ptr<GameObject>& ch) {
        return ch.get() == &child;
    });

    if (found != end(m_children)) {
        auto result = move(*found);
        result->SetParent(nullptr);
        m_children.erase(found);
        return result;
    } else {
        return nullptr;
    }
}

unique_ptr<GameObject> GameObject::DetachChild(const GameObject* child)
{
    auto found = remove_if(begin(m_children), end(m_children),
    [&child](const unique_ptr<GameObject>& ch) {
        return ch.get() == child;
    });
    if (found != end(m_children)) {
        auto result = move(*found);
        result->SetParent(nullptr);
        m_children.erase(found);
        return result;
    } else {
        return nullptr;
    }
}



GameObject& GameObject::AttachNewChild(const std::string& name)
{
    auto child = make_unique<GameObject>(name);
    child->SetParent(this);
    child->SetScene(this->m_scene);
    GameObject* ret = child.get();
    m_children.push_back(move(child));
    return *ret;
}

Transform GameObject::GetWorldTransform() const
{
    Transform result = m_transform;
    GameObject* p = this->m_parent;
    while (p != nullptr) {
        result = p->m_transform * result;
        p = p->m_parent;
    }
    return result;
}

Component* GameObject::FindComponentByName(const std::string& name)
{
    auto res = find_if(begin(m_components), end(m_components),
    [&name](const std::unique_ptr<Component>& c) {
        return c->GetName() == name;
    }
                      );
    if (res != end(m_components)) {
        return res->get();
    }
    return nullptr;
}

GameObject* GameObject::FindChildByName(const std::string& name)
{
    auto res = find_if(begin(m_children), end(m_children),
    [&name](const unique_ptr<GameObject>& c) {
        return c->GetName() == name;
    }
                      );
    if (res != end(m_children)) {
        return (*res).get();
    }
    return nullptr;
}

Component* GameObject::GetComponentByID(int id)
{
    return nullptr;
}

void GameObject::Initialize()
{
    InitializeCurrent();
    InitializeChildren();
}

void GameObject::InitializeCurrent()
{
    for (auto& comp : m_components) {
        comp->Initialize();
    }
}

void GameObject::InitializeChildren()
{
    for (auto& child : m_children) {
        child->Initialize();
    }
}

void GameObject::Awake()
{
    AwakeCurrent();
    AwakeChildren();
}

void GameObject::Start()
{
    StartCurrent();
    StartChildren();
}

void GameObject::FixedUpdate()
{

}

void GameObject::Update(float dt)
{
    UpdateCurrent(dt);
    UpdateChildren(dt);
}

void GameObject::Destroy()
{
    DestroyChildren();
    DestroyCurrent();
    m_components.clear();
    m_children.clear();
    //m_isDestroyed = true;
}


void GameObject::LateUpdate()
{
    LateUpdateCurrent();
    LateUpdateChildren();
}

void GameObject::AwakeCurrent()
{
    this->TimeAwakened = high_resolution_clock::now();
    for (auto& comp : m_components) {
        if (comp->IsEnabled()) {
            comp->Awake();
        }
    }
}

void GameObject::AwakeChildren()
{
    for (auto& child : m_children) {
        child->Awake();
    }
}

void GameObject::StartCurrent()
{

}

void GameObject::StartChildren()
{
    for (auto& child : m_children) {
        child->Start();
    }
}

void GameObject::UpdateCurrent(float dt)
{
//    for (int i = 0; i < m_components.size(); ++i) {
//        auto& c = m_components[i];
//        if (c.get()) {
//            if (c->IsEnabled()) {
//                c->Update(dt);
//            }
//        }
//
//    }
    for (auto& cmp : m_components) {
        if(cmp.get()) {
            if (cmp->IsEnabled()) {
                cmp->Update(dt);
            }
        }
    }

}

void GameObject::UpdateChildren(float dt)
{
    //printf("\nUpdating children for game object %s\n", this->GetName().c_str());
    //printf("%d children in collection\n", m_children.size());

    //for(auto childIter = begin(m_children); childIter != end(m_children); childIter++){
    //        (*childIter)->Update(dt);
    //}

//	for (int i = 0; i < m_children.size(); ++i) {
//		auto& child = m_children[i];
//		child->Update(dt);
//	}

    for (auto& child : m_children) {
        if (child != nullptr) {
            child->Update(dt);
        } else {

            printf("Detected child was nullptr in GameObject::UpdateChildren, size = %d\n", m_children.size());
        }
    }
}


void GameObject::DestroyCurrent()
{
    for (auto& comp : m_components) {
        comp->Destroy();
    }
//    for (int i = 0; i < m_components.size(); ++i) {
//        auto& comp = m_components[i];
//        auto dc = DetachComponent(*(comp.get()));
//    }
    m_components.clear();




}
void GameObject::DestroyChildren()
{
    for (auto& child : m_children) {
        child->Destroy();
    }
//    for (int i = 0; i < m_children.size(); ++i) {
//        auto& ch = m_children[i];
//        auto x = DetachChild(*(ch.get()));
//    }
    m_children.clear();
}
}
