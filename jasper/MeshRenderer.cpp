#include "MeshRenderer.h"
#include "Material.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Shader.h"
#include "Scene.h"
#include "GLError.h"
#include "Texture.h"
#include "PhysicsCollider.h"
#include "Lights.h"
#include "AssetSerializer.h"
#include <iostream>
#include "imgui.h"

namespace Jasper
{

using namespace std;

void MeshRenderer::ToggleWireframe(bool wf)
{
    {
        if (wf) {
            m_polymode0 = GL_FRONT_AND_BACK;
            m_polymode1 = GL_LINE;
        } else {
            m_polymode0 = GL_FRONT;
            m_polymode1 = GL_FILL;
        }
    }
}

MeshRenderer::MeshRenderer(std::string name, Mesh* mesh, Material* material)
    : Component(name)
{
    m_mesh = mesh;
    m_mesh->IncrementRendererCount();
    m_material = material;
    mesh_name = mesh->GetName();
    material_name = material->GetName();
}

MeshRenderer::~MeshRenderer()
{
    Destroy();
}

void MeshRenderer::Destroy()
{

}

void MeshRenderer::Initialize()
{

}

void MeshRenderer::Awake()
{


}

void MeshRenderer::Serialize(std::ofstream& ofs) const
{
    // basically all we want to store is the component type, the mesh name, and the material name
    // when constructing from serialization data we can use those names to pull the mesh and materials out
    // of the scene's caches and use those to construct the mesh renderer.

    using namespace AssetSerializer;

    Component::Serialize(ofs);
    const size_t meshnamesize = mesh_name.size();
    ofs.write(ConstCharPtr(&meshnamesize), sizeof(meshnamesize));
    ofs.write(mesh_name.data(), meshnamesize);
    const size_t matnamesize = material_name.size();
    ofs.write(ConstCharPtr(&matnamesize), sizeof(matnamesize));
    ofs.write(material_name.data(), matnamesize);
}

bool MeshRenderer::ShowGui()
{
    Component::ShowGui();
    if (ImGui::Checkbox("Wireframe", &m_wireframe)) {
        ToggleWireframe(m_wireframe);
    }
	//const char* matname = ;
	ImGui::Text("Material: %s", this->m_material->GetName().c_str());
    return false;
}

void MeshRenderer::Start() {}

void MeshRenderer::FixedUpdate() {}

void MeshRenderer::Update(double dt)
{

}

void MeshRenderer::LateUpdate() {}


void MeshRenderer::Render()
{

    //GLERRORCHECK;
    GLint poly_mode[2];
    glGetIntegerv(GL_POLYGON_MODE, poly_mode);
    glBindVertexArray(m_mesh->VaoID());
    glPolygonMode(m_polymode0, m_polymode1);
    glDrawElements(GL_TRIANGLES, m_mesh->ElementCount(), GL_UNSIGNED_INT, 0);
    glPolygonMode(poly_mode[0], poly_mode[1]);
    GLERRORCHECK;
    glBindVertexArray(0);

}

} // namespace Jasper
