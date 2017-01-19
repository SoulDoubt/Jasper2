#ifndef _JASPER_CAMERA_H_
#define _JASPER_CAMERA_H_

#include "Common.h"
#include "Transform.h"
#include "matrix.h"
#include "GameObject.h"
#include <memory>
#include "PhysicsCollider.h"

namespace Jasper
{

class PhysicsWorld;

class Camera : public GameObject
{
public:

    enum class CameraType
    {
        FIRST_PERSON,
        FLYING
    };


    explicit Camera(CameraType type);
    Camera(CameraType type, float fov, float aspect, float nearZ, float farz);
    virtual ~Camera();

    GameObjectType GetGameObjectType() const override {
        return GameObjectType::Camera;
    }

    void Awake() override;
    void Update(float dt) override;

    void SetPhysicsWorld(PhysicsWorld* world);

    const Matrix4& GetViewMatrix() const;
    Matrix4 GetCubemapViewMatrix();
    
    Matrix4 GetProjectionMatrix() const {
        return m_projectionMatrix;
    }
    
    const Frustum& GetFrustum() const {
        return m_frustum;
    }
    
    void Rotate(float pitch, float roll, float yaw);
    void Translate(float x, float y, float z);
    void Translate(const Vector3& vec);

    Vector3 GetPosition() const {
        return GetWorldTransform().Position;
    }

    Vector3 GetViewDirection() const {
        return m_viewVector;
    }

    Vector3 GetUpVector() const {
        return m_upVector;
    }

    float GetFov() const {
        return m_fov;
    }

    float GetAspectRatio() const {
        return m_aspectRatio;
    }

    float GetNearDistance() const {
        return m_nearDistance;
    }

    float GetFarDistance() const {
        return m_farDistance;
    }

    void SetFov(float fov) {
        m_fov = fov;
        m_projectionMatrix.SetToIdentity();
        m_projectionMatrix.CreatePerspectiveProjection(m_fov, m_aspectRatio, m_nearDistance, m_farDistance);
    }

    void SetAspectRatio(float aspect) {
        m_aspectRatio = aspect;
        m_projectionMatrix.SetToIdentity();
        m_projectionMatrix.CreatePerspectiveProjection(m_fov, m_aspectRatio, m_nearDistance, m_farDistance);
    }

    void SetNearDistance(float n) {
        m_nearDistance = n;
        m_projectionMatrix.SetToIdentity();
        m_projectionMatrix.CreatePerspectiveProjection(m_fov, m_aspectRatio, m_nearDistance, m_farDistance);
    }

    void SetFarDistance(float f) {
        m_farDistance = f;
        m_projectionMatrix.SetToIdentity();
        m_projectionMatrix.CreatePerspectiveProjection(m_fov, m_aspectRatio, m_nearDistance, m_farDistance);
    }



private:

    static Vector3 WORLD_X_AXIS;
    static Vector3 WORLD_Y_AXIS;
    static Vector3 WORLD_Z_AXIS;

    //Quaternion m_orientation;
    //Vector3 m_position;

    Matrix4 m_projectionMatrix;

    Vector3 m_localXAxis;
    Vector3 m_localYAxis;
    Vector3 m_localZAxis;

    Vector3 m_viewVector;
    Vector3 m_upVector;
    Vector3 m_rightVector;

    float m_fov;
    float m_aspectRatio;
    float m_nearDistance;
    float m_farDistance;

    float m_accumPitch = 0.f;
    
    Matrix4 m_viewMatrix;
    Frustum m_frustum;

    CameraType m_type;

    PhysicsWorld* m_physicsWorld = nullptr;
    PhysicsCollider* m_collider = nullptr;
    
    void UpdateViewMatrix();
    void UpdateFrustum();

};

inline void Camera::SetPhysicsWorld(PhysicsWorld* world)
{
    m_physicsWorld = world;
}

}
#endif
