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
	btVector3 Position;
	Vector4 Color;
};

#define VERTEX_CAPACITY 50000
#define INDEX_CAPACITY 80000

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
	int usedVerts = 0;
	int usedIndices = 0;
	
	DebugVertex m_verts[VERTEX_CAPACITY];
	uint m_indices[INDEX_CAPACITY];
	//std::vector<DebugVertex> m_verts;
	//std::vector<uint> m_indices;

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
		//m_verts.clear();
		//m_indices.clear();
		usedVerts = 0;
		usedIndices = 0;
	}

	PhysicsDebugDrawer(Scene* scene)
		: scene(scene),
		vbo(GLBuffer::BufferType::VERTEX),
		ibo(GLBuffer::BufferType::INDEX) {

	}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {}

	void reportErrorWarning(const char* warningString) override { 
		printf("%s\n", warningString);
	}

	virtual void draw3dText(const btVector3& location, const char* textString) {}


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
