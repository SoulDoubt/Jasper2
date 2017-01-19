#include "PhysicsDebugDraw.h"
#include "Scene.h"
#include "Shader.h"


namespace Jasper
{

void PhysicsDebugDrawer::Initialize()
{
    m_verts.reserve(1000);
    m_indices.reserve(1000 * 2);
    m_colors.reserve(1000);    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    vbo.Create();
    cbo.Create();
    ibo.Create();
    vbo.SetUsage(GLBuffer::Usage::StreamDraw);
    cbo.SetUsage(GLBuffer::Usage::StreamDraw);
    ibo.SetUsage(GLBuffer::Usage::StreamDraw);
    vbo.Bind();    
    uint ploc = debugShader->PositionAttributeLocation();
    uint cloc = debugShader->ColorsAttributeLocation();
    debugShader->SetAttributeArray(ploc, GL_FLOAT, 0, 3, 0);
    cbo.Bind();    
    debugShader->SetAttributeArray(cloc, GL_FLOAT, 0, 4, 0);
    glBindVertexArray(0);
}

void PhysicsDebugDrawer::Destroy()
{
    if (vbo.IsCreated()) {
        vbo.Destroy();
        cbo.Destroy();
    }
}

void PhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{

    //Vector3 vecs[2] = { Vector3(from), Vector3(to) };
    //Vector4 cols[2] = { Vector4(color, 1.f), Vector4(color, 1.f)};
    
    m_verts.emplace_back(Vector3(from));
    m_verts.emplace_back(Vector3(to));
    
    m_colors.emplace_back(Vector4(color, 1.f));
    m_colors.emplace_back(Vector4(color, 1.f));
    
    int i = m_indices.size();
    m_indices.push_back(i);
    m_indices.push_back(i + 1);
    

    
}

void PhysicsDebugDrawer::Draw(){
    int count = m_indices.size();
    int vc = m_verts.size();
    int cc = m_colors.size();
    
    glBindVertexArray(vao);
    debugShader->Bind();	
    vbo.Bind();
    vbo.Allocate(m_verts.data(), vc * sizeof(Vector3));
    cbo.Bind();
    cbo.Allocate(m_colors.data(), cc * sizeof(Vector4));
    ibo.Bind();
    ibo.Allocate(m_indices.data(), count * sizeof (GLuint));
    debugShader->SetModelViewProjectionMatrix(mvpMatrix);
    glPointSize(3.0f);
    glLineWidth(2.25f);
    //glDrawArrays(GL_POINTS, 0, 2);
    glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, 0);
    debugShader->Release();
    cbo.Release();
    vbo.Release();
    glBindVertexArray(0);
}

} // Jasper
