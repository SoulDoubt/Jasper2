#ifndef _PHYSICS_DEBUG_DRAW_H_
#define _PHYSICS_DEBUG_DRAW_H_

#include <LinearMath/btIDebugDraw.h>
#include <GL/glew.h>
#include "matrix.h"
#include "GLBuffer.h"
#include <vector>


namespace Jasper
{

class Scene;
class Shader;
class Skeleton;

struct DebugVertex {
	Vector3 Position;
	Vector4 Color;
};

class PhysicsDebugDrawer : public btIDebugDraw
{
private:

	int m_debugMode;
	Scene* scene;
	GLBuffer vbo;
	GLBuffer ibo;
	uint vao;
	Matrix4 mvpMatrix;
	Shader* debugShader;
	std::vector<DebugVertex> m_verts;
	std::vector<uint> m_indices;

public:

	~PhysicsDebugDrawer() {
		Destroy();
	}

	void SetShader(Shader* s) {
		debugShader = s;
	}

	Shader* GetShader() const {
		return debugShader;
	}

	void SetMatrix(const Matrix4& mat) {
		mvpMatrix = mat;
	}

	int getDebugMode() const override {
		return m_debugMode;
	}

	void setDebugMode(int mode) override {
		m_debugMode = mode;
	}

	void Initialize();
	void Destroy();

	void Draw();

	void Reset() {
		m_verts.clear();
		m_indices.clear();
	}

	PhysicsDebugDrawer(Scene* scene)
		: scene(scene),
		vbo(GLBuffer::BufferType::VERTEX),
		ibo(GLBuffer::BufferType::INDEX) {

	}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {}

	void reportErrorWarning(const char* warningString) override { }

	virtual void draw3dText(const btVector3& location, const char* textString) {}

	void DrawSkeleton(Skeleton* skel);



};



class DebugDrawer {

public:
	Scene* scene;
	GLBuffer vbo;
	GLBuffer ibo;
	uint vao;
	Matrix4 mvpMatrix;
	Shader* debugShader;
	std::vector<DebugVertex> m_verts;
	std::vector<uint> m_indices;

public:
	DebugDrawer(Scene* scene) : scene(scene),
		vbo(GLBuffer::BufferType::VERTEX),
		ibo(GLBuffer::BufferType::INDEX) {}
	void Initialize();
	void DrawSkeleton(Skeleton* s);
	void SetShader(Shader* s) {
		debugShader = s;
	}

	void SetMvpMatrix(const Matrix4& mat) {
		mvpMatrix = mat;
	}
};


}

#endif // _PHYSICS_DEBUG_DRAW_H_
