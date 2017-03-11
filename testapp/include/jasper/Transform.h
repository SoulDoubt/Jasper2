#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Common.h"
#include "matrix.h"
#include "quaternion.h"
#include <btBulletDynamicsCommon.h>

namespace Jasper
{

class Transform
{

public:

    Vector3 Position = { 0.0f, 0.0f, 0.0f };
    Quaternion Orientation = { 0.0f, 0.0f, 0.0f, 1.0f };
    Vector3 Scale = { 1.0f, 1.0f, 1.0f };

    Transform(const Vector3& position, const Quaternion& orientation) {
        Position = position;
        Orientation = orientation;
        Scale = { 1.0f, 1.0f, 1.0f };
    }
    
    Transform(const Vector3& position, const Quaternion& orientation, const Vector3& scale){
        Position = position;
        Orientation = orientation;
        Scale = scale;
    }

    Transform(const Transform& o) {
        Position = o.Position;
        Orientation = o.Orientation;
        Scale = o.Scale;
    }

    Transform& operator=(const Transform& o) {
        Position = o.Position;
        Orientation = o.Orientation;
        Scale = o.Scale;
        return *this;
    }


    Transform(const btTransform& btt) {
        Position = Vector3(btt.getOrigin());
        Orientation = Quaternion(btt.getRotation());
        Scale = { 1.0f, 1.0f, 1.0f };
    }

    btTransform AsBtTransform() const {
        btTransform btt;
        btVector3 pos = { Position.x, Position.y, Position.z };
        btQuaternion q = btQuaternion(Orientation.x, Orientation.y, Orientation.z, Orientation.w);
        btt.setOrigin(pos);
        btt.setRotation(q);        
        return btt;
    }

    Transform() {}

    Matrix4 TransformMatrix() const;

    Matrix3 NormalMatrix(Matrix4 mat) const;

    Transform& Translate(const Vector3& tv);
    Transform& Translate(const float x, const float y, const float z);

    Transform& UniformScale(const float s);

    Transform& Rotate(const Vector3& axis, float angle);

    Transform& RotateAround(const Vector3& point, const Vector3& axis, const float degrees);

    Transform& PositionLerp(const Vector3& start, const Vector3& end, float pct);
    
    Transform Inverted() const;

    void SetIdentity();
};

inline Transform Transform::Inverted() const{
    const Quaternion invOrientation = Inverse(this->Orientation);

	Transform invTransform;

	invTransform.Position    = (invOrientation * -this->Position) / this->Scale;
	invTransform.Orientation = invOrientation;
	invTransform.Scale       = invOrientation * (Vector3{1, 1, 1} / this->Scale);

	return invTransform;
}


inline Transform operator*(const Transform& ps, const Transform& ls)
{
    Transform ws;
    ws.Position    = ps.Position + ps.Orientation * (ps.Scale * ls.Position);
    ws.Orientation = ps.Orientation * ls.Orientation;
    ws.Scale       = ps.Scale * ls.Scale;
    return ws;
}

inline Transform& operator*=(Transform& ps, const Transform& ls)
{
    ps = ps * ls;
    return ps;
}

inline Transform operator/(const Transform& ws, const Transform& ps){
    Transform ls;
    
    const Quaternion psConjugate = Conjugate(ps.Orientation);

	ls.Position    = (psConjugate * (ws.Position - ps.Position)) / ps.Scale;
	ls.Orientation = psConjugate * ws.Orientation;
	ls.Scale       = psConjugate * (ws.Scale / ps.Scale);

	return ls;
}

inline Transform& operator/=(Transform& ws, const Transform& ps){
    ws = ws / ps;
    return ws;
}

inline Transform Inverse(const Transform& t)
{
	const Quaternion invOrientation = Inverse(t.Orientation);

	Transform invTransform;

	invTransform.Position    = (invOrientation * -t.Position) / t.Scale;
	invTransform.Orientation = invOrientation;
	invTransform.Scale       = invOrientation * (Vector3{1, 1, 1} / t.Scale);

	return invTransform;
}


inline Vector3 operator*(const Transform& t, Vector3& vec){
    return (Conjugate(t.Orientation) * (t.Position - vec)) / t.Scale;
}

inline Matrix4 Transform::TransformMatrix() const
{
    return Matrix4::FromTransform(*this);
}

inline Matrix3 Transform::NormalMatrix(Matrix4 mat) const
{
    return mat.NormalMatrix();
}

inline Transform& Transform::Translate(const Vector3& tv)
{
    Position += tv;
    return *this;
}

inline Transform& Transform::Translate(const float x, const float y, const float z)
{
    return Translate(Vector3(x, y, z));
}


inline void Transform::SetIdentity()
{
    Position = { 0.f, 0.f, 0.f };
    Orientation = { 0.f, 0.f, 0.f, 1.f };
    Scale = { 1.f, 1.f, 1.f };
}

inline Transform& Transform::UniformScale(const float s)
{
    Scale = { s, s, s };
    return *this;
}

}
#endif // _TRANSFORM_H_
