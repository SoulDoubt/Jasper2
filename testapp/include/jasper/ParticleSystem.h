#pragma once


#include "Component.h"
#include <chrono>
#include "vector.h"
#include "GLBuffer.h"

namespace Jasper
{



struct Particle {
    Vector3   Position; // Center point of particle
    Vector3   Velocity; // Current particle velocity
    Vector4   Color;    // Particle color
    float     Rotate;   // Rotate the particle the center
    float     Size;     // Size of the particle
    std::chrono::high_resolution_clock::time_point TimeEmitted;
    uint      LifeTime; // in milliseconds
};


class ParticleSystem : public Component
{
public:
    ParticleSystem(std::string name);

    void Initialize() override;
    void Destroy() override;
    void Awake() override;
    void Update(float dt) override;



};

class Shader;

class ParticleRenderer : public Component
{
public:
    ParticleRenderer(std::string name, Shader* shader, ParticleSystem* system);
    
    void Initialize() override;
    void Update(float dt) override;
    

    private:
    uint m_vao;
    GLBuffer m_vertexBuffer;
    GLBuffer m_texCoordBuffer;
    GLBuffer m_indexBuffer;
    Shader* m_shader;
    ParticleSystem* m_particleSystem;
    void Render();
};

}
