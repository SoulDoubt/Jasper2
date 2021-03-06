#include "SkyboxRenderer.h"
#include "Material.h"
#include "GameObject.h"
#include "Scene.h"
#include "Shader.h"

#include <GL/glew.h>

namespace Jasper
{
using namespace std;

SkyboxRenderer::SkyboxRenderer(std::string name, Mesh* mesh, Material* mat) :
    MeshRenderer(name, mesh, mat)
{

}

SkyboxRenderer::~SkyboxRenderer()
{
    Destroy();
}

void SkyboxRenderer::Update(double dt)
{
    Render();
}

void SkyboxRenderer::Render()
{
    //SkyboxShader* shader = (SkyboxShader*)m_material->GetShader();
    //const int shaderID = shader->ProgramID();
    

    glBindVertexArray(m_mesh->VaoID());
    glActiveTexture(GL_TEXTURE0 + 0);
    glDepthMask(GL_FALSE);
    const int texID = m_material->GetCubemapTexture()->TextureID();
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
    glDrawElements(GL_TRIANGLES, m_mesh->ElementCount(), GL_UNSIGNED_INT, 0);
    glDepthMask(GL_TRUE);
    //shader->Release();
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindVertexArray(0);

}
void SkyboxRenderer::Serialize(std::ofstream & ofs) const
{
	MeshRenderer::Serialize(ofs);
}
}
