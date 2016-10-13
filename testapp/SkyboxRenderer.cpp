#include "SkyboxRenderer.h"
#include "Material.h"
#include "GameObject.h"
#include "Scene.h"
#include "SkyboxShader.h"

#include <GL/glew.h>

namespace Jasper {
using namespace std;

SkyboxRenderer::SkyboxRenderer(Mesh* mesh, Material* mat) :
	MeshRenderer(mesh, mat)
{

}

SkyboxRenderer::~SkyboxRenderer()
{
	Destroy();
}

void SkyboxRenderer::Update(float dt)
{
	Render();
}

void SkyboxRenderer::Render()
{
	SkyboxShader* shader = (SkyboxShader*)m_material->GetShader();
	int shaderID = shader->ProgramID();
	shader->Bind();

	// we need the current view matrix and projection matrix of the scene
	auto projection = GetGameObject()->GetScene()->ProjectionMatrix();
	auto view = GetGameObject()->GetScene()->GetCamera().GetCubemapViewMatrix().Inverted();

	//trix4 view;
	//view.SetToIdentity();
	auto pv = projection * view;
	shader->SetProjectionMatrix(projection);
	shader->SetViewMatrix(view);

	glBindVertexArray(m_vaoID);
	glActiveTexture(GL_TEXTURE0 + 0);
	glDepthMask(GL_FALSE);
	int texID = m_material->GetCubemapTexture()->TextureID();
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	glDrawElements(GL_TRIANGLES, m_elementCount, GL_UNSIGNED_INT, 0);
	glDepthMask(GL_TRUE);
	shader->Release();
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindVertexArray(0);
	
}
}