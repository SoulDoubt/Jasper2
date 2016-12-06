#include "Camera.h"
#include "PhysicsWorld.h"
#include "Scene.h"


namespace Jasper
{

Vector3 Camera::WORLD_X_AXIS = Vector3(1.0f, 0.0f, 0.0f);
Vector3 Camera::WORLD_Y_AXIS = Vector3(0.0f, 1.0f, 0.0f);
Vector3 Camera::WORLD_Z_AXIS = Vector3(0.0f, 0.0f, 1.0f);


Camera::Camera(Camera::CameraType type) //: GameObject("camera")
{

    m_type = type;
    m_transform.SetIdentity();
    m_transform.Position = { 0.0f, 1.82f, 10.0f };
}

void Camera::Awake()
{

    GameObject::Awake();
    m_collider = GetComponentByType<CapsuleCollider>();
    /*if (!m_collider) {
    	m_collider = new CapsuleCollider("camera_collider", Vector3(1.0f, 2.0f, 1.0f), m_physicsWorld);
    }*/
    //btDefaultMotionState* ms = new btDefaultMotionState(m_transform.GetBtTransform());
    //btVector3 inertia;
    //m_collisionShape->calculateLocalInertia(mass, inertia);
    //btRigidBody::btRigidBodyConstructionInfo rbci(mass, ms, m_collisionShape.get(), inertia);
    //m_rigidBody = std::make_unique<btRigidBody>(rbci);
    //m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    //m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
    //m_physicsWorld->AddRigidBody(m_rigidBody.get());
    if (m_collider) {
        m_collider->GetRigidBody()->setCollisionFlags(m_collider->GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        m_collider->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);


    }
}


Camera::~Camera()
{

}

Matrix4 Camera::GetViewMatrix()
{

    //Transform t = Transform(m_position, m_orientation);
    //Matrix4 vm = Matrix4::FromTransform(t);
    Matrix4 vm = m_transform.TransformMatrix();
    m_localXAxis = Vector3(vm.mat[0].x, vm.mat[1].x, vm.mat[2].x);
    m_localYAxis = Vector3(vm.mat[0].y, vm.mat[1].y, vm.mat[2].y);
    m_localZAxis = Vector3(vm.mat[0].z, vm.mat[1].z, vm.mat[2].z);
    m_viewVector = -m_localZAxis;

    if (m_collider)
        m_collider->SetWorldTransform(m_transform.GetBtTransform());

#ifdef DEBUG_DRAW_PHYSICS

    //Matrix4 pm = m_scene->ProjectionMatrix();
    //Matrix4 mvp = pm * vm * vm;
    //m_physicsWorld->debugDrawer->mvpMatrix = mvp;
    //btTransform btt;
    //btt.setIdentity();
//	m_physicsWorld->DrawPhysicsShape(btt, m_collisionShape.get(), btVector3(1.0f, 0.0f, 0.0f));


#endif



    //m_rigidBody->getMotionState()->setWorldTransform(btt);
    //printf("Camera Position: %.3f, %.3f, %.3f Direction: %.3f, %.3f, %.3f \r", pos.x(), pos.y(), pos.z(), m_viewVector.x, m_viewVector.y, m_viewVector.z);
    return vm;
}

Matrix4 Camera::GetCubemapViewMatrix()
{
    // removes the translation components for skybox rendering
    //Transform t = Transform(m_position, m_orientation);
    //Matrix4 vm = Matrix4::FromTransform(t);
    //Matrix4 vm = FromBtTransform(btt);
    Matrix4 vm = m_transform.TransformMatrix();
    vm.mat[0].w = 0.0f;
    vm.mat[1].w = 0.0f;
    vm.mat[2].w = 0.0f;
    return vm;
}

void Camera::Rotate(float pitch, float roll, float yaw)
{
    m_accumPitch += pitch;
    //btQuaternion orientation = btt.getRotation();
    Quaternion orientation = m_transform.Orientation;
    //Quaternion qo = Quaternion(orientation.x(), orientation.y(), orientation.z(), orientation.w());


    if (m_accumPitch > 90.f) {
        pitch = 0.f;
        m_accumPitch = 90.f;
    }
    if (m_accumPitch < -90.f) {
        pitch = 0.f;
        m_accumPitch = -90.f;
    }

    //btQuaternion xrot, yrot;
    //Quaternion xr, yr;
    Quaternion xrot, yrot;
    if (pitch != 0.f) {
        //xrot = btQuaternion(WORLD_X_AXIS.AsBtVector3(), DEG_TO_RAD(pitch));
        xrot = Quaternion::FromAxisAndAngle(WORLD_X_AXIS, DEG_TO_RAD(pitch));
        orientation = orientation * xrot;
        //qo = qo * xr;
    }

    if (yaw != 0.f) {
        //yrot = btQuaternion(WORLD_Y_AXIS.AsBtVector3(), DEG_TO_RAD(yaw));
        yrot = Quaternion::FromAxisAndAngle(WORLD_Y_AXIS, DEG_TO_RAD(yaw));
        orientation = yrot * orientation;
        //qo = yr * qo;
    }
    //btt.setRotation(orientation);
    m_transform.Orientation = orientation;
    if (m_collider) {
        btTransform btt;
        btt.setRotation(m_transform.Orientation.AsBtQuaternion());
        btt.setOrigin(m_transform.Position.AsBtVector3());
        m_collider->SetWorldTransform(btt);
    }

    /* TEST */
    /*Transform tr;
    btVector3 abpos = btt.getOrigin();
    btQuaternion abq = btt.getRotation();
    tr.Position = Vector3(btt.getOrigin().x(), btt.getOrigin().y(), btt.getOrigin().z());
    tr.Orientation = qo;

    Matrix4 myMatrix = Matrix4::FromTransform(tr);
    Matrix4 btMatrix = FromBtTransform(btt);
    printf("ff");*/

}

void Camera::Translate(float x, float y, float z)
{
    Translate( { x, y, z });
}

void Camera::Translate(const Vector3& vec)
{
    Vector3 forwards;
    float z = vec.z * -1.0f;

    if (m_type == CameraType::FIRST_PERSON) {
        forwards = WORLD_Y_AXIS.Cross(m_localXAxis);
        forwards.Normalize();
    } else {
        forwards = m_viewVector;
    }
    auto& current = m_transform.Position;
    //auto& current = btt.getOrigin();

    if (m_collider) {
        //btVector3 after = current;
        Vector3 after = current;
        after += m_localXAxis.AsBtVector3() * vec.x;
        after += WORLD_Y_AXIS.AsBtVector3() * vec.y;
        after += forwards.AsBtVector3() * z;
        btTransform from, to;
        from.setIdentity();
        to.setIdentity();

        from.setOrigin(current.AsBtVector3());
        to.setOrigin(after.AsBtVector3());

        btCollisionWorld::ClosestConvexResultCallback cb(current.AsBtVector3(), after.AsBtVector3());
        cb.m_collisionFilterMask = btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter;

        btConvexShape* cs = static_cast<btConvexShape*>(m_collider->GetRigidBody()->getCollisionShape());
        m_physicsWorld->ConvexSweepTest(cs, from, to, cb);
        if (cb.hasHit() && cb.m_hitCollisionObject != m_collider->GetRigidBody()) {
            /*if () {
            	printf("\nHit Self");
            }*/
            float epsilon = 0.0001;
            float min = btMax(epsilon, cb.m_closestHitFraction);
            current += -forwards * 0.1;
            btVector3 newPos = current.AsBtVector3();
            //newPos.setInterpolate3(current.AsBtVector3(), after.AsBtVector3(), min);
            m_transform.Position = Vector3(newPos);
        } else {
            m_transform.Position = after;
        }
    } else {
        current += m_localXAxis.AsBtVector3() * vec.x;
        current += WORLD_Y_AXIS.AsBtVector3() * vec.y;
        current += forwards.AsBtVector3() * z;
        m_transform.Position = current;
    }
    if (m_collider) {
        btTransform btt;
        btt.setRotation(m_transform.Orientation.AsBtQuaternion());
        btt.setOrigin(m_transform.Position.AsBtVector3());
        m_collider->SetWorldTransform(btt);
    }

}
}
