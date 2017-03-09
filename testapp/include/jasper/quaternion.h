#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "Common.h"
#include "matrix.h"
#include <btBulletDynamicsCommon.h>

namespace Jasper
{

struct Angles {
	float Roll;
	float Pitch;
	float Yaw;

	Angles(float roll, float pitch, float yaw) : Roll(roll), Pitch(pitch), Yaw(yaw) {}
};


class Quaternion
{
public:
	float x, y, z, w;

	Quaternion(float x, float y, float z, float w);
	Quaternion(const Vector3& vector, float scalar);
	Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
	Quaternion(const btQuaternion& q) : x(q.x()), y(q.y()), z(q.z()), w(q.w()) {}
	Quaternion(const Quaternion& o) = default;
	Quaternion(float pitch, float roll, float yaw);
	Quaternion(const Matrix3& m);
	btQuaternion AsBtQuaternion() {
		return btQuaternion(x, y, z, w);
	}

	Matrix3 ToMatrix3() const;
	Matrix4 ToMatrix4() const;

	float operator[](int index) const;
	float& operator[](int index);
	Quaternion operator-() const;
	Quaternion& operator=(const Quaternion& o);
	//Quaternion operator+(const Quaternion& o) const;
	Quaternion& operator+=(const Quaternion& o);
	//Quaternion operator-(const Quaternion& o) const;
	Quaternion& operator-=(const Quaternion& o);
	Quaternion& operator/=(const float f);
	Quaternion operator/(const float f) const;
	Quaternion& operator*=(const float f);
	Quaternion operator*(const float f) const;
	Quaternion& operator*=(const Quaternion& o);


	float Scalar()const {
		return w;
	}
	float Pitch() const;
	float Roll() const;
	float Yaw() const;
	Angles ToEulerAngles() const;

	float Length() const;



	static Quaternion FromAxisAndAngle(const Vector3& axis, float angle);

	float* AsFloatPtr() {
		return &x;
	}

	const float* AsFloatPtr() const {
		return &x;
	}
};

inline Quaternion::Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

inline Quaternion::Quaternion(const Vector3 & vector, float scalar)
{
	x = vector.x;
	y = vector.y;
	z = vector.z;
	w = scalar;
}

inline Quaternion::Quaternion(float pitch, float roll, float yaw)
{
	// Assuming the angles are in radians.
	float c1 = cosf(yaw / 2);
	float s1 = sinf(yaw / 2);
	float c2 = cosf(pitch / 2);
	float s2 = sinf(pitch / 2);
	float c3 = cosf(roll / 2);
	float s3 = sinf(roll / 2);
	float c1c2 = c1*c2;
	float s1s2 = s1*s2;
	w = c1c2*c3 - s1s2*s3;
	x = c1c2*s3 + s1s2*c3;
	y = s1*c2*c3 + c1*s2*s3;
	z = c1*s2*c3 - s1*c2*s3;
}


inline float Quaternion::operator[](int index) const
{
	return (&x)[index];
}

inline float& Quaternion::operator[](int index)
{
	return (&x)[index];
}

inline Quaternion Quaternion::operator-() const
{
	return Quaternion(-x, -y, -z, -w);
}

inline Quaternion& Quaternion::operator=(const Quaternion& o)
{
	x = o.x;
	y = o.y;
	z = o.z;
	w = o.w;
	return *this;
}


inline Quaternion operator+(const Quaternion& a, const Quaternion& b) {
	return Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

inline Quaternion& Quaternion::operator+=(const Quaternion& o)
{
	x += o.x;
	y += o.y;
	z += o.z;
	w += o.w;
	return *this;
}

inline Quaternion operator-(const Quaternion& a, const Quaternion& b)
{
	return Quaternion(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

inline Quaternion& Quaternion::operator-=(const Quaternion& o)
{
	x -= o.x;
	y -= o.y;
	z -= o.z;
	w -= o.w;
	return *this;
}

inline Quaternion operator*(const Quaternion& a, const Quaternion& b)
{
	Quaternion q;
	q.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
	q.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
	q.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
	q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
	return q;
}

inline Quaternion& Quaternion::operator*=(const Quaternion& o)
{
	*this = *this * o;
	return *this;
}

inline Quaternion Normalize(const Quaternion& q)
{
	return q * (1.0f / q.Length());
}

inline Quaternion & Quaternion::operator/=(const float f)
{
	x /= f;
	y /= f;
	z /= f;
	w /= w;
	return *this;
}

inline Quaternion Quaternion::operator/(const float f) const
{
	return Quaternion(x / f, y / f, z / f, w / f);
}

inline Quaternion& Quaternion::operator*=(const float f)
{
	x *= f;
	y *= f;
	z *= f;
	w *= w;
	return *this;
}

inline Vector3 operator*(const Quaternion& q, const Vector3& v)
{
	const Vector3 t = 2.0f * Cross(Vector3(q.x, q.y, q.z), v);
	return (v + q.w * t + Cross(Vector3(q.x, q.y, q.z), t));
}

inline Vector3 operator*(const Vector3& v, const Quaternion& q)
{
	const Vector3 t = 2.0f * Cross(Vector3(q.x, q.y, q.z), v);
	return (v + q.w * t + Cross(Vector3(q.x, q.y, q.z), t));
}

inline Quaternion Quaternion::operator*(const float f) const
{
	return Quaternion(x * f, y * f, z * f, w * f);
}

inline float Dot(const Quaternion& a, const Quaternion& b)
{
	return (Vector3(a.x, a.y, a.z).Dot(Vector3(b.x, b.y, b.z))) + a.w * b.w;
}

inline Quaternion Conjugate(const Quaternion& qu)
{
	Quaternion q = Normalize(qu);
	Quaternion c(-q.x, -q.y, -q.z, q.w);
	return c;
}

inline Quaternion Inverse(const Quaternion& q)
{
	return Conjugate(q) / (Dot(q, q));
}

inline Quaternion operator*(float f, const Quaternion& q)
{
	return Quaternion(q.x * f, q.y * f, q.z * f, q.w * f);
}

inline Quaternion Lerp(const Quaternion& a, const Quaternion& b, float pct) {
	Quaternion r = a + (pct * (b - a));
	return r;
}

inline Quaternion Slerp(const Quaternion& a, const Quaternion& b, float pct) {
	Quaternion temp;
	float dot = Dot(a, b);
	if (dot < 0) {
		dot = -dot;
		temp = -b;
	}
	else {
		temp = b;
	}

	// if the angle is small, may as well just lerp it.
	if (dot < 0.95f) {
		float angle = acosf(dot);
		return (a * sinf(angle *(1 - pct)) + temp * sinf(angle * pct)) / sinf(angle);
	}
	else {
		return Lerp(a, temp, pct);
	}
}


inline Quaternion Interpolate(const Quaternion& pStart, const Quaternion& pEnd, float pct){
	// calc cosine theta
	float cosom = pStart.x * pEnd.x + pStart.y * pEnd.y + pStart.z * pEnd.z + pStart.w * pEnd.w;

	// adjust signs (if necessary)
	Quaternion end = pEnd;
	if (cosom < static_cast<float>(0.0))
	{
		cosom = -cosom;
		end.x = -end.x;   // Reverse all signs
		end.y = -end.y;
		end.z = -end.z;
		end.w = -end.w;
	}

	// Calculate coefficients
	float sclp, sclq;
	if ((static_cast<float>(1.0) - cosom) > static_cast<float>(0.0001)) // 0.0001 -> some epsillon
	{
		// Standard case (slerp)
		float omega;
		float sinom;
		omega = std::acos(cosom); // extract theta from dot product's cos theta
		sinom = sin(omega);
		sclp = sin((static_cast<float>(1.0) - pct) * omega) / sinom;
		sclq = sin(pct * omega) / sinom;
	}
	else
	{
		// Very close, do linear interp (because it's faster)
		sclp = static_cast<float>(1.0) - pct;
		sclq = pct;
	}
	Quaternion pOut;
	pOut.x = sclp * pStart.x + sclq * end.x;
	pOut.y = sclp * pStart.y + sclq * end.y;
	pOut.z = sclp * pStart.z + sclq * end.z;
	pOut.w = sclp * pStart.w + sclq * end.w;

	return pOut;
}


inline float Quaternion::Pitch() const
{
	return asinf(2 * x*y + 2 * z*w);
}

inline float Quaternion::Roll() const
{
	//return atan2f(2.0f * x * y + z * w,
	//              x * x + w * w - y * y - z * z);
	return atan2f(2 * x*w - 2 * y*z, 1 - 2 * (x*x) - 2 * (z*z));


}

inline float Quaternion::Yaw() const
{
	//return asinf(-2.0f * (x * z * - w * y));

	return atan2(2 * y*w - 2 * x*z, 1 - 2 * (y*y) - 2 * (z*z));
}

inline Angles Quaternion::ToEulerAngles() const
{
	return Angles(Roll(), Pitch(), Yaw());
}

inline float Quaternion::Length() const
{
	float len = x * x + y * y + z * z + w * w;
	return sqrtf(len);
}

inline Quaternion Quaternion::FromAxisAndAngle(const Vector3 & axis, float angle)
{
	Quaternion q;

	const Vector3 a = Normalize(axis);
	const float s = sinf(0.5f * angle);
	Vector3 tmp = a * s;
	q.x = tmp.x;
	q.y = tmp.y;
	q.z = tmp.z;
	q.w = cosf(0.5f * angle);

	return q;
}





inline Quaternion Rotation(const Vector3& orig, const Vector3& dest)
{
	float cosTheta = Dot(orig, dest);
	Vector3 rotationAxis;
	float epsilon = 0.000001f;

	if (cosTheta >= 1.f - epsilon)
		return Quaternion();

	if (cosTheta < -1.f + epsilon) {
		// special case when vectors in opposite directions :
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		// This implementation favors a rotation around the Up axis (Y),
		// since it's often what you want to do.
		rotationAxis = Cross(Vector3(0.f, 0.f, 1.f), orig);
		if (rotationAxis.LengthSquared() < epsilon) // bad luck, they were parallel, try again!
			rotationAxis = Cross(Vector3(1.f, 0.f, 0.f), orig);

		rotationAxis = Normalize(rotationAxis);
		return Quaternion::FromAxisAndAngle(rotationAxis, M_PI);
		//return angleAxis(pi<T>(), rotationAxis);
	}

	// Implementation from Stan Melax's Game Programming Gems 1 article
	rotationAxis = Cross(orig, dest);

	float s = sqrtf((1.f + cosTheta) * 2.f);
	float invs = 1.f / s;

	Quaternion qq(Vector3(rotationAxis * invs), s * 0.5);
	return qq;
	//		return tquat<T, P>(
	//			s * static_cast<T>(0.5f),
	//			rotationAxis.x * invs,
	//			rotationAxis.y * invs,
	//			rotationAxis.z * invs);
}


}

#endif
