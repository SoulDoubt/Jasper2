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

class PhysicsDebugDrawer : public btIDebugDraw
{
private:

    int m_debugMode;
    Scene* scene;
    GLBuffer vbo;
    GLBuffer cbo;
    GLBuffer ibo;
    uint vao;
    Matrix4 mvpMatrix;
    Shader* debugShader;
    std::vector<Vector3> m_verts;
    std::vector<Vector4> m_colors;
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
    
    void Reset(){
        m_verts.erase(std::begin(m_verts), std::end(m_verts));
        m_colors.erase(std::begin(m_colors), std::end(m_colors));
        m_indices.erase(std::begin(m_indices), std::end(m_indices));
    }

    PhysicsDebugDrawer(Scene* scene)
        : scene(scene),
          vbo(GLBuffer::BufferType::VERTEX),
          cbo(GLBuffer::VERTEX),
          ibo(GLBuffer::BufferType::INDEX) {

    }

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {}

    void reportErrorWarning(const char* warningString) override { }

    virtual void draw3dText(const btVector3& location, const char* textString) {}





};


}

#endif // _PHYSICS_DEBUG_DRAW_H_
