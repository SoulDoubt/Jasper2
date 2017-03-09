#include "PhysicsDebugDraw.h"
#include "Scene.h"
#include "Shader.h"


namespace Jasper
{



void PhysicsDebugDrawer::Initialize()
{
    m_verts.reserve(20000);
    m_indices.reserve(20000);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    vbo.Create();    
    ibo.Create();

    vbo.SetUsage(GLBuffer::Usage::StreamDraw);    
    ibo.SetUsage(GLBuffer::Usage::StreamDraw);

    vbo.Bind();

    uint ploc = debugShader->PositionAttributeLocation();
    uint cloc = debugShader->ColorsAttributeLocation();

    debugShader->SetAttributeArray(ploc, GL_FLOAT, (void*)offsetof(DebugVertex, Position), 3, sizeof(DebugVertex));    
    debugShader->SetAttributeArray(cloc, GL_UNSIGNED_INT, (void*)offsetof(DebugVertex, Color), 4, sizeof(DebugVertex));
    glBindVertexArray(0);
}

void PhysicsDebugDrawer::Destroy()
{
    if (vbo.IsCreated()) {
        vbo.Destroy();        
        ibo.Destroy();
    }
}

void PhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    
    DebugVertex fromVert = { Vector3(from), Vector4(color, 1.f) };
    DebugVertex toVert = { Vector3(to), Vector4(color, 1.f) };

    m_verts.emplace_back(fromVert);
    m_verts.emplace_back(toVert);

    int i = m_indices.size();
    m_indices.push_back(i);
    m_indices.push_back(i + 1);

}

void PhysicsDebugDrawer::Draw()
{
    int count = m_indices.size();
    int vc = m_verts.size();    

    glBindVertexArray(vao);
    debugShader->Bind();
    
    vbo.Bind();
    vbo.Allocate(m_verts.data(), vc * sizeof(DebugVertex));
    
    ibo.Bind();
    ibo.Allocate(m_indices.data(), count * sizeof (GLuint));
    debugShader->SetModelViewProjectionMatrix(mvpMatrix);
    glPointSize(3.0f);
    glLineWidth(2.25f);
    
    glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, 0);
    debugShader->Release();    
    vbo.Release();
    glBindVertexArray(0);
}

void DebugDrawer::Initialize()
{
}

void DebugDrawer::DrawSkeleton(Skeleton * s)
{
}

} // Jasper
