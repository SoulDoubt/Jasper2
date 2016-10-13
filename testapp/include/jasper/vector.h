#ifndef _VECTOR_3_H_
#define _VECTOR_3_H_

#include "Common.h"
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#include <cmath>

#endif
#include <string>

#include <bullet/btBulletDynamicsCommon.h>

#ifndef DEG_TO_RAD
#define DEG_TO_RAD(theta) (((float)(M_PI / 180.0f)) * theta)
#endif

namespace Jasper {

struct Vector2 {
	float x, y;

	Vector2(float x, float y);
	Vector2() = default;
};

inline Vector2::Vector2(float x, float y) : x(x), y(y) {
}

inline Vector2 operator-(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x - b.x, a.y - b.y);
}

inline Vector2 operator+(const Vector2& a, const Vector2& b) {
	return Vector2(a.x + b.x, a.y + b.y);
}

inline Vector2 operator*(const Vector2& a, const Vector2& b) {
	return Vector2(a.x * b.x, a.y * b.y);
}


inline Vector2 operator*(const Vector2& a, const float f) {
	return Vector2(a.x * f, a.y * f);
}

inline Vector2 operator/(const Vector2& a, const float f) {
	return Vector2(a.x / f, a.y / f);
}

inline float Length(const Vector2& vec) {
	return sqrtf(vec.x * vec.x + vec.y * vec.y);

}

inline Vector2 Normalize(const Vector2& vec) {
	float len = Length(vec);
	return Vector2(vec.x / len, vec.y / len);
}


struct Vector3
{

	float x, y, z;

	Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vector3(const btVector3& v) : x(v.x()), y(v.y()), z(v.z()) {}

	float Length() const;
	float LengthSquared() const;
	void Normalize();
	Vector3 Normalized() const;

	btVector3 AsBtVector3() { return btVector3(x, y, z); }

	Vector3& operator=(const Vector3& o);

	bool Compare(const Vector3& o) const;
	bool Compare(const Vector3& o, float epsilon) const;

	Vector3 Cross(const Vector3& o) const;
	float Dot(const Vector3& o) const;

	std::string ToString(int precision = 4) const;

	float& operator[](int index) {
		return (&x)[index];
	}

	float operator[](int index) const {
		return (&x)[index];
	}

	const float* AsFloatPtr() const {
		return &x;
	}

};


inline Vector3& operator+=(Vector3& a, const Vector3& b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

inline Vector3& operator-=(Vector3& a, const Vector3& b) {
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

inline Vector3& operator*=(Vector3& a, float f) {
	a.x *= f;
	a.y *= f;
	a.z *= f;
	return a;
}

inline Vector3 operator*(const Vector3& a, const float f) {
	return Vector3(a.x * f, a.y * f, a.z * f);
}

inline Vector3 operator*(const float f, const Vector3& v) {
	return v * f;
}

inline Vector3 operator/(const Vector3& a, const float f) {
	return Vector3(a.x / f, a.y / f, a.z / f);
}

inline void Vector3::Normalize() {
	float len = this->Length();
	x /= len;
	y /= len;
	z /= len;
}

inline Vector3& operator/=(Vector3& a, const Vector3& b) {
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	return a;
}

inline Vector3& operator/=(Vector3& a, const float f) {
	float invf = 1.0f / f;
	a.x *= invf;
	a.y *= invf;
	a.z *= invf;
	return a;
}

inline Vector3& Vector3::operator=(const Vector3& o) {
	x = o.x;
	y = o.y;
	z = o.z;
	return *this;
}

inline Vector3 operator-(const Vector3& a) {
	return Vector3(-a.x, -a.y, -a.z);
}

inline bool operator==(const Vector3& a, const Vector3& b) {
	return a.Compare(b);
}

inline bool operator!=(const Vector3& a, const Vector3& b) {
	return !a.Compare(b);
}

inline Vector3 operator+(const Vector3& a, const Vector3& b) {
	return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vector3 operator-(const Vector3& a, const Vector3& b) {
	return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vector3 operator*(const Vector3& a, const Vector3& b) {
	return{ a.x * b.x, a.y * b.y, a.z * b.z };

}

inline float Vector3::Length() const {
	return sqrtf(x * x + y * y + z * z);
}

inline float Vector3::LengthSquared() const {
	return x * x + y * y + z * z;
}

inline float Length(const Vector3& a) {
	return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

inline Vector3 Vector3::Normalized() const {
	float len = Length();
	return Vector3(x / len, y / len, z / len);
}

inline Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	return Vector3(v.x / len, v.y / len, v.z / len);
}


inline bool Vector3::Compare(const Vector3& o) const {
	return ((x == o.x) && (y == o.y) && (z == o.z));
}

inline bool Vector3::Compare(const Vector3& o, float epsilon) const {
	if (std::fabs(x - o.x) > epsilon) {
		return false;
	}
	if (std::fabs(y - o.y) > epsilon) {
		return false;
	}
	if (std::fabs(z - o.z) > epsilon) {
		return false;
	}
	return true;
}

inline Vector3 Vector3::Cross(const Vector3& o) const {
	return Vector3(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);
}

inline Vector3 Cross(const Vector3& a, const Vector3& b) {
	float x = a.y * b.z - a.z * b.y;
	float y = a.z * b.x - a.x * b.z;
	float z = a.x * b.y - a.y * b.x;
	return Vector3(x, y, z);
}

inline float Vector3::Dot(const Vector3& o) const {
	return x * o.x + y * o.y + z * o.z;
}


inline float Dot(const Vector3& a, const Vector3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vector3 Lerp(const Vector3& a, const Vector3& b, const float pct) {
	return (a + pct * (b - a));
}

inline Vector3 SmoothStep(const Vector3& a, const Vector3& b, const float pct) {
	float t = pct * pct * (3.0f - 2.0f * pct);
	return Lerp(a, b, t);
}

inline std::string Vector3::ToString(int precision) const {
	std::string xval = std::to_string(x);
	std::string yval = std::to_string(y);
	std::string zval = std::to_string(z);
	return "(" + xval + "," + yval + "," + zval + ")";
}




class Vector4 {

private:
	bool Compare(const Vector4& o) const;
	bool Compare(const Vector4& o, float epsilon) const;

public:
	float x, y, z, w;

	Vector4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) {}

	Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	Vector4(const Vector3& xyz, float w);

	Vector4 operator-() const {
		return Vector4(-x, -y, -z, -w);
	}

	float Length() const {
		float lenSquared = x * x + y * y + z * z + w * w;
		return sqrtf(lenSquared);
	}

	Vector4& Normalize() {
		float len = Length();
		x /= len;
		y /= len;
		z /= len;
		w /= len;
		return *this;
	}

	Vector4 Normalized() const {
		float len = Length();
		return Vector4(x / len, y / len, z / len, w / len);
	}

	Vector4 operator*(const float f) const {
		return Vector4(x * f, y * f, z * f, w * f);
	}

	Vector4& operator*=(const float f) {
		x *= f;
		y *= f;
		z *= f;
		w *= f;
		return *this;
	}

	Vector4& operator/=(const float f) {
		x /= f;
		y /= f;
		z /= f;
		w /= f;
		return *this;
	}

	Vector4& operator+=(const float f) {
		x += f;
		y += f;
		z += f;
		w += f;
		return *this;
	}

	Vector4& operator-=(const float f) {
		x -= f;
		y -= f;
		z -= f;
		w -= f;
		return *this;
	}

	Vector4& operator+=(const Vector4& o) {
		x += o.x;
		y += o.y;
		z += o.z;
		w += o.w;
		return *this;
	}

	Vector4& operator-=(const Vector4& o) {
		x -= o.x;
		y -= o.y;
		z -= o.z;
		w -= o.w;
		return *this;
	}

	Vector4& operator*=(const Vector4& o) {
		x *= o.x;
		y *= o.y;
		z *= o.z;
		w *= o.w;
		return *this;
	}

	bool operator==(const Vector4& o) {
		return Compare(o);
	}

	bool operator !=(const Vector4& o) {
		return !Compare(o);
	}

	Vector3 AsVector3() const {
		return Vector3(x, y, z);
	}

	float Dot(const Vector4& o)const {
		return x * o.x + y * o.y + z * o.z + w * o.w;
	}

	float& operator[](int index) {
		return (&x)[index];
	}

	float operator[](int index) const {
		return (&x)[index];
	}

	std::string ToString() const;

	const float* AsFloatPtr() const {
		return &x;
	}


};

inline Vector4 operator/(const Vector4& v, float f) {
	return Vector4(v.x / f, v.y / f, v.z / f, v.w / f);
}

inline bool Vector4::Compare(const Vector4& o) const {
	return ((x == o.x) && (y == o.y) && (z == o.z) && (w == o.w));
}

inline bool Vector4::Compare(const Vector4& o, const float epsilon) const {
	if (fabs(x - o.x) > epsilon) {
		return false;
	}
	if (fabs(y - o.y) > epsilon) {
		return false;
	}
	if (fabs(z - o.z) > epsilon) {
		return false;
	}
	if (fabs(w - o.w) > epsilon) {
		return false;
	}
	return true;
}

inline Vector4::Vector4(const Vector3 & v, float dub)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = dub;
}

inline Vector4 operator*(const Vector4& a, const Vector4& b) {
	return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

inline std::string Vector4::ToString() const {
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")";
}

inline Vector4 Normalize(const Vector4& v) {
	return v / v.Length();
}

}

#endif