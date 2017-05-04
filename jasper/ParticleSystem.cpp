#include "ParticleSystem.h"

namespace Jasper
{

void ParticleSystem::Initialize()
{
}

void ParticleSystem::Destroy()
{
}

void ParticleSystem::Awake()
{
}

void ParticleSystem::Update(double dt)
{
}

//  ----------- Particle Renderer -------------------------------

ParticleRenderer::ParticleRenderer(std::string name, Shader* shader, ParticleSystem* system)
    :Component(name),
     m_vertexBuffer(GLBuffer::VERTEX),
     m_texCoordBuffer(GLBuffer::VERTEX),
     m_indexBuffer(GLBuffer::INDEX),
     m_particleSystem(system)
{

}

void ParticleRenderer::Initialize()
{

}

void ParticleRenderer::Update(double dt)
{
    // update stuff
    // ...
    Render();
    
}

void ParticleRenderer::Render()
{
}



}
