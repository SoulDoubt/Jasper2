#include "PhysicsDebugDraw.h"
#include "Scene.h"

namespace Jasper {

void PhysicsDebugDrawer::Initialize()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	vbo.Create();
	vbo.SetUsage(GLBuffer::Usage::DynamicDraw);
	vbo.Bind();
	uint loc = debugShader->PositionAttributeLocation();
	debugShader->SetAttributeArray(loc, GL_FLOAT, 0, 3, 0);		
	glBindVertexArray(0);
}

void PhysicsDebugDrawer::Destroy()
{
	if (vbo.IsCreated()) {
		vbo.Destroy();
	}
}

void PhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
	
	Vector3 vecs[2] = { Vector3(from), Vector3(to) };

	glBindVertexArray(vao);
	debugShader->Bind();	
	vbo.Bind();	
	vbo.Allocate(&vecs, 2 * sizeof(Vector3));	
	debugShader->SetModelViewProjectionMatrix(mvpMatrix);		
	glPointSize(3.0f);
	glLineWidth(1.25f);
	glDrawArrays(GL_POINTS, 0, 2);
	glDrawArrays(GL_LINES, 0, 2);	
	debugShader->Release();
	vbo.Release();
	glBindVertexArray(0);
}
}