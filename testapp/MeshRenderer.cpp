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
		if (wf)
		{
			m_polymode0 = GL_FRONT_AND_BACK;
			m_polymode1 = GL_LINE;
		}
		else {
			m_polymode0 = GL_FRONT;
			m_polymode1 = GL_FILL;
		}
	}
}

MeshRenderer::MeshRenderer(std::string name, Mesh* mesh, Material* material) : Component(name),
    m_vertexBuffer(GLBuffer::BufferType::VERTEX),
    m_texCoordBuffer(GLBuffer::BufferType::VERTEX),
    m_normalBuffer(GLBuffer::BufferType::VERTEX),
    m_tangentBuffer(GLBuffer::BufferType::VERTEX),
    m_bitangentBuffer(GLBuffer::BufferType::VERTEX),
    m_indexBuffer(GLBuffer::BufferType::INDEX)
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
    m_vertexBuffer.Destroy();
    m_indexBuffer.Destroy();
    m_texCoordBuffer.Destroy();
    m_normalBuffer.Destroy();
    m_tangentBuffer.Destroy();
    m_bitangentBuffer.Destroy();
    m_mesh->DecrementRendererCount();
}

void MeshRenderer::Initialize() {
	GLERRORCHECK;

	m_mesh->Initialize();
	// gather mesh data and create GL Buffers and such for future rendering...
	assert(m_material);
	assert(m_mesh);

	// create a VAO first
	glGenVertexArrays(1, &m_vaoID);
	glBindVertexArray(m_vaoID);
	//GLERRORCHECK;

	m_elementCount = m_mesh->Indices.size();
	auto shader = m_material->GetShader();

	m_vertexBuffer.Create();
	m_indexBuffer.Create();
	m_normalBuffer.Create();
	m_texCoordBuffer.Create();
	m_tangentBuffer.Create();
	m_bitangentBuffer.Create();

	const int positionLocation = shader->PositionAttributeLocation();
	const int normalLocation = shader->NormalAttributeLocation();
	const int texLocation = shader->TexCoordAttributeLocation();
	const int tangentLocation = shader->TangentAttributeLocation();
	const int bitangentLocation = shader->GetAttributeLocation("bitangent");

	// upload the data
	m_vertexBuffer.Bind();
	//m_vertexBuffer.Allocate(m_mesh->Vertices.data(), m_mesh->Vertices.size() * sizeof(Vertex));
	m_vertexBuffer.Allocate(m_mesh->Positions.data(), m_mesh->Positions.size() * sizeof(Vector3));
	m_normalBuffer.Bind();
	m_normalBuffer.Allocate(m_mesh->Normals.data(), m_mesh->Normals.size() * sizeof(Vector3));
	m_texCoordBuffer.Bind();
	m_texCoordBuffer.Allocate(m_mesh->TexCoords.data(), m_mesh->TexCoords.size() * sizeof(Vector2));
	m_tangentBuffer.Bind();
	m_tangentBuffer.Allocate(m_mesh->Tangents.data(), m_mesh->Tangents.size() * sizeof(Vector4));
	m_bitangentBuffer.Bind();
	m_bitangentBuffer.Allocate(m_mesh->Bitangents.data(), m_mesh->Bitangents.size() * sizeof(Vector3));
	m_indexBuffer.Bind();
	m_indexBuffer.Allocate(m_mesh->Indices.data(), m_mesh->Indices.size() * sizeof(GLuint));

	// we can now destroy the client-side vertex data of the mesh...  

	m_vertexBuffer.Bind();
	shader->SetAttributeArray(positionLocation, GL_FLOAT, (void*)0, 3, 0);
	m_normalBuffer.Bind();
	shader->SetAttributeArray(normalLocation, GL_FLOAT, (void*)0, 3, 0);
	m_texCoordBuffer.Bind();
	shader->SetAttributeArray(texLocation, GL_FLOAT, (void*)0, 2, 0);
	m_tangentBuffer.Bind();
	shader->SetAttributeArray(tangentLocation, GL_FLOAT, (void*)0, 4, 0);
	m_bitangentBuffer.Bind();
	shader->SetAttributeArray(bitangentLocation, GL_FLOAT, (void*)0, 3, 0);

	glBindVertexArray(0);
	GLERRORCHECK;
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
	return false;
}

void MeshRenderer::Start() {}

void MeshRenderer::FixedUpdate() {}

void MeshRenderer::Update(float dt)
{

}

void MeshRenderer::LateUpdate() {}


void MeshRenderer::Render()
{

    GLERRORCHECK;
	GLint poly_mode[2];
	glGetIntegerv(GL_POLYGON_MODE, poly_mode);	
    glBindVertexArray(m_vaoID);
	glPolygonMode(GL_FRONT, m_polymode1);
    glDrawElements(GL_TRIANGLES, m_elementCount, GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT, poly_mode[0]);
    GLERRORCHECK;
    glBindVertexArray(0);

}

} // namespace Jasper
