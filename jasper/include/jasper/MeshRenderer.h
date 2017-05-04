#pragma once

#include "Component.h"
#include "Transform.h"
#include "GLBuffer.h"
#include "Mesh.h"
#include <memory>
#include <vector>



namespace Jasper
{

class Material;
class Shader;

class MeshRenderer : public Component
{
private:

protected:

    Mesh* m_mesh;
    unsigned m_vaoID;
    int m_elementCount = 0;
    bool m_isVisible = true;
    Material* m_material;
    std::string mesh_name;
    std::string material_name;

    uint m_polymode0 = 0x0404;
    uint m_polymode1 = 0x1B02;
    bool m_wireframe = false;
    bool m_castsShadows = false;
    
public:

    bool IsVisible() const {
        return m_isVisible;
    }
    
    void SetIsVisible(bool v){
        m_isVisible = v;
    }

    void ToggleWireframe(bool wf);

    Material* GetMaterial() const {
        return m_material;
    }

    NON_COPYABLE(MeshRenderer);

    explicit MeshRenderer(std::string name, Mesh* mesh, Material* material);
    ~MeshRenderer();


    virtual void Render();

    void Destroy() override;
    void Initialize() override;
    void Awake() override;
    void Start() override;
    void FixedUpdate() override;
    void Update(double dt) override;
    void LateUpdate() override;


    void Serialize(std::ofstream& ofs) const override;

    ComponentType GetComponentType() const override {
        return ComponentType::MeshRenderer;
    }

    std::string GetMeshName() const {
        return mesh_name;
    }

    std::string GetMaterialName() const {
        return material_name;
    }

    Mesh* GetMesh() const {
        return m_mesh;
    }

    void SetMeshName(std::string name) {
        mesh_name = std::move(mesh_name);
    }

    void SetMaterialName(std::string name) {
        material_name = std::move(name);
    }

    bool ShowGui() override;

};

} // namespace Jasper
