#include "Camera.h"
#include "PhysicsWorld.h"
#include "Scene.h"


namespace Jasper
{

struct ContactCallback : public btCollisionWorld::ContactResultCallback {

    btRigidBody* body;
    btTransform from, to;
    bool hit;

    bool HasHit() {
        return hit;
    }

    btVector3 ContactPoint;

    ContactCallback(btRigidBody* body, btTransform from, btTransform to) : body(body), from(from), to(to) {
        hit = false;
    }

    virtual btScalar addSingleResult(btManifoldPoint& cp,
                                     const btCollisionObjectWrapper* colObj0, int partId0, int index0,
                                     const btCollisionObjectWrapper* colObj1, int partId1, int index1) {
        btVector3 pt; // will be set to point of collision relative to body
        if (colObj0->m_collisionObject == body) {
            pt = cp.m_localPointA;
            ContactPoint = pt;
            hit = true;
        } else {
            assert(colObj1->m_collisionObject == body && "body does not match either collision object");
            pt = cp.m_localPointB;
        }
        // do stuff with the collision point
        return 0; // There was a planned purpose for the return value of addSingleResult, but it is not used so you can ignore it.
    }

};

Vector3 Camera::WORLD_X_AXIS = Vector3(1.0f, 0.0f, 0.0f);
Vector3 Camera::WORLD_Y_AXIS = Vector3(0.0f, 1.0f, 0.0f);
Vector3 Camera::WORLD_Z_AXIS = Vector3(0.0f, 0.0f, 1.0f);


Camera::Camera(Camera::CameraType type) : GameObject("camera")
{

    m_type = type;
    m_transform.SetIdentity();
    //m_transform.Position = { 0.0f, 1.82f, 7.0f };
}

Camera::Camera(CameraType type, float fov, float aspect, float nearZ, float farz)
    : GameObject("camera")
{
    m_type = type;
    m_fov = fov;
    m_aspectRatio = aspect;
    m_nearDistance = nearZ;
    m_farDistance = farz;
    m_transform.SetIdentity();
    m_projectionMatrix.SetToIdentity();
    m_projectionMatrix.CreatePerspectiveProjection(m_fov, m_aspectRatio, m_nearDistance, m_farDistance);
}

void Camera::Awake()
{

    GameObject::Awake();
    m_collider = GetComponentByType<PhysicsCollider>();
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

void Camera::Update(double dt){
    UpdateViewMatrix();
    UpdateFrustum();
}

void Camera::UpdateViewMatrix(){
    const Matrix4 vm = GetWorldTransform().TransformMatrix();
    m_localXAxis = Vector3(vm.mat[0].x, vm.mat[1].x, vm.mat[2].x);
    m_localYAxis = Vector3(vm.mat[0].y, vm.mat[1].y, vm.mat[2].y);
    m_localZAxis = Vector3(vm.mat[0].z, vm.mat[1].z, vm.mat[2].z);
    m_viewVector = -m_localZAxis;
    m_upVector = m_localYAxis;

    if (m_collider)
        m_collider->SetWorldTransform(m_transform.AsBtTransform());
        
    m_viewMatrix = vm.Inverted();
}

void Camera::UpdateFrustum(){
    // Some code about the derivation of the view frustum
    // was copied with name and type changes from Intel's Software Occlusion culling demo
    // and is Copyright 2012 Intel Corporation.
    Vector3 cam_position = GetPosition();
    Vector3 view_vector = Normalize(GetViewDirection());
    Vector3 up_vector = Normalize(GetUpVector());
    float aspect_ratio = GetAspectRatio();
    float fov = GetFov();
    float near = GetNearDistance();
    float far = GetFarDistance();
    Vector3 right = Cross(up_vector, view_vector);
    //up_vector = -up_vector;
    Vector3 frustum_verts[8];

    // Compute the position of the center of the near and far clip planes.
    Vector3 nearCenter = cam_position + view_vector * near;
    Vector3 farCenter  = cam_position + view_vector * far;

    // Compute the width and height of the near and far clip planes
    float tanHalfFov = tanf(0.5f*DEG_TO_RAD(fov));
    float halfNearWidth  = near * tanHalfFov;
    float halfNearHeight = halfNearWidth / aspect_ratio;

    float halfFarWidth   = far * tanHalfFov;
    float halfFarHeight  = halfFarWidth / aspect_ratio;

    // Create two vectors each for the near and far clip planes.
    // These are the scaled up and right vectors.
    Vector3 upNear      = up_vector    * halfNearHeight;
    Vector3 rightNear   = right * halfNearWidth;
    Vector3 upFar       = up_vector    * halfFarHeight;
    Vector3 rightFar    = right * halfFarWidth;
        
    // Use the center positions and the up and right vectors
    // to compute the positions for the near and far clip plane vertices (four each)
    frustum_verts[0] = nearCenter + upNear - rightNear; // near top left
    frustum_verts[1] = nearCenter + upNear + rightNear; // near top right
    frustum_verts[2] = nearCenter - upNear + rightNear; // near bottom right
    frustum_verts[3] = nearCenter - upNear - rightNear; // near bottom left
    frustum_verts[4] = farCenter  + upFar  - rightFar;  // far top left
    frustum_verts[5] = farCenter  + upFar  + rightFar;  // far top right
    frustum_verts[6] = farCenter  - upFar  + rightFar;  // far bottom right
    frustum_verts[7] = farCenter  - upFar  - rightFar;  // far bottom left
    
    memcpy(m_frustum.Vertices, frustum_verts, 8 * sizeof(Vector3));
    
    // Compute some of the frustum's edge vectors.  We will cross these
    // to get the normals for each of the six planes.
    Vector3 nearTop     = frustum_verts[1] - frustum_verts[0];
    Vector3 nearLeft    = frustum_verts[3] - frustum_verts[0];
    Vector3 topLeft     = frustum_verts[4] - frustum_verts[0];
    Vector3 bottomRight = frustum_verts[2] - frustum_verts[6];
    Vector3 farRight    = frustum_verts[5] - frustum_verts[6];
    Vector3 farBottom   = frustum_verts[7] - frustum_verts[6];

    //Vector3 frustum_plane_normals[6];    
    m_frustum.Planes[0].Normal   = Cross(nearTop,     nearLeft).Normalized();
    m_frustum.Planes[0].Distance = -Dot(m_frustum.Planes[0].Normal, frustum_verts[0]);
    m_frustum.Planes[1].Normal   = Cross(nearLeft,    topLeft).Normalized();
    m_frustum.Planes[1].Distance = -Dot(m_frustum.Planes[1].Normal, frustum_verts[0]);
    m_frustum.Planes[2].Normal    = Cross(topLeft,     nearTop).Normalized();
    m_frustum.Planes[2].Distance = -Dot(m_frustum.Planes[2].Normal, frustum_verts[0]);
    m_frustum.Planes[3].Normal = Cross(farBottom,   bottomRight).Normalized();
    m_frustum.Planes[3].Distance = -Dot(m_frustum.Planes[3].Normal, frustum_verts[6]);
    m_frustum.Planes[4].Normal  = Cross(bottomRight, farRight).Normalized();
    m_frustum.Planes[4].Distance = -Dot(m_frustum.Planes[4].Normal, frustum_verts[6]);
    m_frustum.Planes[5].Normal    = Cross(farRight,    farBottom).Normalized();
    m_frustum.Planes[5].Distance = -Dot(m_frustum.Planes[5].Normal, frustum_verts[6]);
    
//    frustum_plane_normals[0] = Cross(nearTop,     nearLeft).Normalized();    // near clip plane
//    frustum_plane_normals[1] = Cross(nearLeft,    topLeft).Normalized();     // left
//    frustum_plane_normals[2] = Cross(topLeft,     nearTop).Normalized();     // top
//    frustum_plane_normals[3] = Cross(farBottom,   bottomRight).Normalized(); // bottom
//    frustum_plane_normals[4] = Cross(bottomRight, farRight).Normalized();    // right
//    frustum_plane_normals[5] = Cross(farRight,    farBottom).Normalized();   // far clip plane
//    
//    Plane frustum_planes[6];
//    for (uint i = 0; i < 6; i++){
//        frustum_planes[i].Normal = frustum_plane_normals[i];
//        frustum_planes[i].Distance = -Dot(frustum_plane_normals[i], frustum_verts[(i < 3) ? 0 : 6]);
//    }
    
}


Camera::~Camera()
{

}

const Matrix4& Camera::GetViewMatrix() const
{
    return m_viewMatrix;
}

Matrix4 Camera::GetCubemapViewMatrix()
{
    // removes the translation components for skybox rendering    
    Matrix4 vm = GetWorldTransform().TransformMatrix();
    vm.mat[0].w = 0.0f;
    vm.mat[1].w = 0.0f;
    vm.mat[2].w = 0.0f;
    return vm.Inverted();
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

  
    Quaternion xrot, yrot;
    if (pitch != 0.f) {        
        xrot = Quaternion::FromAxisAndAngle(WORLD_X_AXIS, DEG_TO_RAD(pitch));
        orientation = orientation * xrot;        
    }

    if (yaw != 0.f) {        
        yrot = Quaternion::FromAxisAndAngle(WORLD_Y_AXIS, DEG_TO_RAD(yaw));
        orientation = yrot * orientation;        
    }    
    m_transform.Orientation = orientation;
	if (m_collider) {
		btTransform btt;
		btt.setRotation(m_transform.Orientation.AsBtQuaternion());
		btt.setOrigin(m_transform.Position.AsBtVector3());
		m_collider->SetWorldTransform(btt);
	}
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

    if (m_collider) {        
        Vector3 after = current;
        after += m_localXAxis.AsBtVector3() * vec.x;
        after += WORLD_Y_AXIS.AsBtVector3() * vec.y;
        after += forwards.AsBtVector3() * z;
        btTransform from, to;
        from.setIdentity();
        to.setIdentity();

        from.setOrigin(current.AsBtVector3());
        to.setOrigin(after.AsBtVector3());

        auto me = m_collider->GetRigidBody();

        ContactCallback conb = ContactCallback(me, from, to);

        m_physicsWorld->ContactTest(me, conb);

        if (conb.HasHit()) {
            const float epsilon = 0.0001;
            //const float min = btMax(epsilon, conb.ContactPoint);
            //current += -forwards * 0.1;
            btVector3 newPos = current.AsBtVector3();
            newPos.setInterpolate3(current.AsBtVector3(), after.AsBtVector3(), epsilon);
            m_transform.Position = Vector3(newPos);
        } else {
            m_transform.Position = after;
        }
        /*btCollisionWorld::ClosestConvexResultCallback cb(current.AsBtVector3(), after.AsBtVector3());
        cb.m_collisionFilterMask = btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter;*/

        //btConvexShape* cs = static_cast<btConvexShape*>(me);

        //m_physicsWorld->ConvexSweepTest(cs, from, to, cb);
        //if (cb.hasHit() && cb.m_hitCollisionObject != me) {
        //    /*if () {
        //    	printf("\nHit Self");
        //    }*/
        //    const float epsilon = 0.0001;
        //    const float min = btMax(epsilon, cb.m_closestHitFraction);
        //    //current += -forwards * 0.1;
        //    btVector3 newPos = current.AsBtVector3();
        //    newPos.setInterpolate3(current.AsBtVector3(), after.AsBtVector3(), min);
        //    m_transform.Position = Vector3(newPos);
        //} else {
        //    m_transform.Position = after;
        //}

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
