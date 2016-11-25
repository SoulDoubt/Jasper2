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

    GLBuffer m_vertexBuffer;
    GLBuffer m_texCoordBuffer;
    GLBuffer m_normalBuffer;
    GLBuffer m_tangentBuffer;
    GLBuffer m_bitangentBuffer;
    GLBuffer m_indexBuffer;

protected:

    Mesh* m_mesh;
    unsigned m_vaoID;
    int m_elementCount = 0;
    Material* m_material;
    std::string mesh_name;
    std::string material_name;

public:

    Material* GetMaterial() const {
        return m_material;
    }

    NON_COPYABLE(MeshRenderer);

    explicit MeshRenderer(Mesh* mesh, Material* material);
    ~MeshRenderer();

    virtual void Render();

    void Destroy() override;
    void Awake() override;
    void Start() override;
    void FixedUpdate() override;
    void Update(float dt) override;
    void LateUpdate() override;
    ComponentType GetComponentType() override {
        return ComponentType::MeshRenderer;
    }
    
    std::string GetMeshName() const {
        return mesh_name;
    }
    
    std::string GetMaterialName() const {
        return material_name;
    }
    
    void SetMeshName(const std::string& name) {
        mesh_name = mesh_name;
    }
    
    void SetMaterialName(const std::string& name) {
        material_name = name;
    }

};

} // namespace Jasper
