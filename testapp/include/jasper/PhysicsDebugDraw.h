#ifndef _PHYSICS_DEBUG_DRAW_H_
#define _PHYSICS_DEBUG_DRAW_H_

#include <bullet/LinearMath/btIDebugDraw.h>
#include <GL/glew.h>
#include "matrix.h"
#include "GLBuffer.h"


namespace Jasper {

class Scene;
class Shader;

class PhysicsDebugDrawer : public btIDebugDraw {

public:

	~PhysicsDebugDrawer() {
		Destroy();
	}

	GLBuffer vbo;
	GLBuffer ibo;

	void Initialize();
	void Destroy();

	PhysicsDebugDrawer(Scene* scene) : scene(scene), vbo(GLBuffer::BufferType::VERTEX), ibo(GLBuffer::BufferType::INDEX) {
		
	}

	Scene* scene;
	uint vao;

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {}

	virtual void reportErrorWarning(const char* warningString) {}

	virtual void draw3dText(const btVector3& location, const char* textString) {}

	virtual void setDebugMode(int debugMode) {}

	virtual int	getDebugMode() const {
		return btIDebugDraw::DBG_DrawWireframe;
	}

	Matrix4 mvpMatrix;
	Shader* debugShader;
};


}

#endif // _PHYSICS_DEBUG_DRAW_H_
