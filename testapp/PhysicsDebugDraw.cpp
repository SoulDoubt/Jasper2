#include "PhysicsDebugDraw.h"
#include "Scene.h"
#include "Shader.h"


namespace Jasper
{



void PhysicsDebugDrawer::Initialize()
{
    //m_verts.reserve(25000);
    //m_indices.reserve(40000);

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
	//size_t vs = m_verts.size();
	//size_t is = m_indices.size();
    DebugVertex fromVert = { from, Vector4(color, 1.f) };
    DebugVertex toVert = { to, Vector4(color, 1.f) };

	if (usedVerts + 2 < VERTEX_CAPACITY) {

		m_verts[usedVerts] = fromVert;
		m_verts[usedVerts + 1] = toVert;

		usedVerts += 2;
	}
	if (usedIndices + 2 < INDEX_CAPACITY) {
		//int i = m_indices.size();
		m_indices[usedIndices] = usedIndices;
		m_indices[usedIndices + 1] = usedIndices + 1;

		usedIndices += 2;
	}
}

void PhysicsDebugDrawer::Draw()
{
    //int count = m_indices.size();
    //int vc = m_verts.size();    

    glBindVertexArray(vao);
    debugShader->Bind();
	glDisable(GL_DEPTH_TEST);
    vbo.Bind();
    vbo.Allocate(m_verts, usedVerts * sizeof(DebugVertex));
    
    ibo.Bind();
    ibo.Allocate(m_indices, usedIndices * sizeof (GLuint));
    debugShader->SetModelViewProjectionMatrix(mvpMatrix);
    //glPointSize(5.0f);
    glLineWidth(2.25f);
    
    glDrawElements(GL_LINES, usedIndices, GL_UNSIGNED_INT, 0);
    debugShader->Release();    
    vbo.Release();
    glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void DebugDrawer::Initialize()
{
}

void DebugDrawer::DrawSkeleton(Skeleton * s)
{
}

} // Jasper
