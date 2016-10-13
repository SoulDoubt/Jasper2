#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <assert.h>
#include "vector.h"


#define MATRIX_INVERSE_EPSILON 1e-14
#define MATRIX_EPSILON 1e-6

namespace Jasper {

class Transform;

class Matrix3 {


public:

	Vector3 mat[3];

	Matrix3();
	Matrix3(const Vector3& x, const Vector3& y, const Vector3& z);
	Matrix3(const Matrix3& o) = default;

	Matrix3& operator+=(const Matrix3& o);
	Matrix3& operator-=(const Matrix3& o);

	Matrix3 operator*(const Matrix3& o) const;
	Matrix3 operator*(const float f) const;

	Vector3 operator*(const Vector3& vec) const;
	Matrix3& operator*=(const float f);

	std::string ToString() const;

	float Determinant() const;

	const Vector3& operator[](int index) const;
	Vector3& operator[](int index);

	Matrix3 Inverted() const;

	Matrix3 Transposed() const;

	Matrix3& Transpose();

	const float* AsFloatPtr() const;


};

inline std::string Matrix3::ToString() const {
	auto r0 = mat[0].ToString();
	auto r1 = mat[1].ToString();
	auto r2 = mat[2].ToString();
	return r0 + "\n" + r1 + "\n" + r2 + "\n";
}

inline Matrix3::Matrix3(const Vector3& x, const Vector3& y, const Vector3& z) {
	mat[0].x = x.x; mat[0].y = x.y;	mat[0].z = x.z;
	mat[1].x = y.x; mat[1].y = y.y;	mat[1].z = y.z;
	mat[2].x = z.x; mat[2].y = z.y;	mat[2].z = z.z;
}

inline Matrix3::Matrix3() {
	// construct the identity matrix
	mat[0] = Vector3(1.0f, 0.0f, 0.0f);
	mat[1] = Vector3(0.0f, 1.0f, 0.0f);
	mat[2] = Vector3(0.0f, 0.0f, 1.0f);
}

inline const Vector3& Matrix3::operator[](int index) const {
	assert(index >= 0 && index <= 2);
	return mat[index];
}

inline Vector3& Matrix3::operator[](int index) {
	assert(index >= 0 && index <= 2);
	return mat[index];
}

// vector multiplication
inline Vector3 Matrix3::operator*(const Vector3& vec) const {
	return Vector3(mat[0].x * vec.x + mat[1].x * vec.y + mat[2].x * vec.z,
		mat[0].y * vec.x + mat[1].y * vec.y + mat[2].y * vec.z,
		mat[0].z * vec.x + mat[1].z * vec.y + mat[2].z * vec.z);

}

// scalar multiplcation
inline Matrix3& Matrix3::operator*=(const float f) {
	mat[0] *= f;
	mat[1] *= f;
	mat[2] *= f;
	return *this;
}

inline Matrix3& Matrix3::operator+=(const Matrix3& o) {
	mat[0].x += o.mat[0].x;
	mat[0].y += o.mat[0].y;
	mat[0].z += o.mat[0].z;

	mat[1].x += o.mat[1].x;
	mat[1].y += o.mat[1].y;
	mat[1].z += o.mat[1].z;

	mat[2].x += o.mat[2].x;
	mat[2].y += o.mat[2].y;
	mat[2].z += o.mat[2].z;

	return *this;
}

inline Matrix3& Matrix3::operator-=(const Matrix3& o) {
	mat[0].x -= o.mat[0].x;
	mat[0].y -= o.mat[0].y;
	mat[0].z -= o.mat[0].z;

	mat[1].x -= o.mat[1].x;
	mat[1].y -= o.mat[1].y;
	mat[1].z -= o.mat[1].z;

	mat[2].x -= o.mat[2].x;
	mat[2].y -= o.mat[2].y;
	mat[2].z -= o.mat[2].z;

	return *this;
}

inline Matrix3 Matrix3::operator*(const Matrix3& o) const {

	Matrix3 ret;

	const Vector3& ta = mat[0];
	const Vector3& tb = mat[1];
	const Vector3& tc = mat[2];

	Vector3 oa = Vector3(o.mat[0].x, o.mat[1].x, o.mat[2].x);
	float aa = ta.Dot(oa);
	ret.mat[0].x = aa;

	Vector3 ob = Vector3(o.mat[0].y, o.mat[1].y, o.mat[2].y);
	float tab = ta.Dot(ob);
	ret.mat[0].y = tab;

	Vector3 oc = Vector3(o.mat[0].z, o.mat[1].z, o.mat[2].z);

	float tac = ta.Dot(oc);
	ret.mat[0].z = tac;

	float tba = tb.Dot(oa);
	ret.mat[1].x = tba;

	float tbb = tb.Dot(ob);
	ret.mat[1].y = tbb;

	float tbc = tb.Dot(oc);
	ret.mat[1].z = tbc;

	float tca = tc.Dot(oa);
	ret.mat[2].x = tca;

	float tcb = tc.Dot(ob);
	ret.mat[2].y = tcb;

	float tcc = tc.Dot(oc);
	ret.mat[2].z = tcc;

	return ret;
}

inline Matrix3 Matrix3::operator*(const float f) const {
	Vector3 r0 = mat[0];
	r0 *= f;
	Vector3 r1 = mat[1];
	r1 *= f;
	Vector3 r2 = mat[2];
	r2 *= f;
	return Matrix3(r0, r1, r2);
}

inline Matrix3 operator*(const float f, const Matrix3& m) {
	return m * f;
}

inline float Matrix3::Determinant() const {
	float a = (mat[1].y * mat[2].z) - (mat[1].z * mat[2].y);
	float b = (mat[1].x * mat[2].z) - (mat[1].z * mat[2].x);
	float c = (mat[1].x * mat[2].y) - (mat[1].y * mat[2].x);
	return (mat[0].x * a) - (mat[0].y * b) + (mat[0].z * c);
}

inline Matrix3 Matrix3::Transposed() const {
	Matrix3 t;
	t.mat[0] = { Vector3(mat[0].x, mat[1].x, mat[2].x) };
	t.mat[1] = { Vector3(mat[0].y, mat[1].y, mat[2].y) };
	t.mat[2] = { Vector3(mat[0].z, mat[1].z, mat[2].z) };
	return t;
}

inline Matrix3& Matrix3::Transpose() {
	Vector3 m0 = { mat[0].x, mat[1].x, mat[2].x };
	Vector3 m1 = { mat[0].y, mat[1].y, mat[2].y };
	Vector3 m2 = { mat[0].z, mat[1].z, mat[2].z };
	mat[0] = m0;
	mat[1] = m1;
	mat[2] = m2;
	return *this;
}

inline const float* Matrix3::AsFloatPtr() const
{
	return &(mat[0].x);
}

inline Matrix3 Matrix3::Inverted() const {
	float det = Determinant();
	if (fabsf(det) < MATRIX_INVERSE_EPSILON) {
		//determinant is 0 - can not invert.
		// return the identity
		return Matrix3();
	}

	float invDet = 1.0f / det;

	return Matrix3();


}



/*------------ MATRIX 4 ---------------------------*/

class Matrix4 {
public:
	Vector4 mat[4];

	Matrix4() {
		SetToIdentity();
	}

	static Matrix4 FromTransform(const Transform& tr);
	static Matrix4 FromBtTransform(const btTransform& t);

	float Determinant() const;
	Matrix4 Inverted() const;
	Matrix3 NormalMatrix() const;
	Matrix3 RotationMatrix() const;

	Matrix4(const Vector4& x, const Vector4& y, const Vector4& z, const Vector4& w) {
		mat[0] = x;
		mat[1] = y;
		mat[2] = z;
		mat[3] = w;
	}

	void SetToIdentity() {
		mat[0] = { 1.0f, 0.0f, 0.0f, 0.0f };
		mat[1] = { 0.0f, 1.0f, 0.0f, 0.0f };
		mat[2] = { 0.0f, 0.0f, 1.0f, 0.0f };
		mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	}

	const Vector4& operator[](int index) const {
		assert(index >= 0 && index <= 3);
		return mat[index];
	}

	Vector4& operator[](int index) {
		assert(index >= 0 && index <= 3);
		return mat[index];
	}



	std::string ToString() const {
		auto r0 = mat[0].ToString();
		auto r1 = mat[1].ToString();
		auto r2 = mat[2].ToString();
		auto r3 = mat[3].ToString();
		return r0 + "\n" + r1 + "\n" + r2 + "\n" + r3 + "\n";
	}

	void CreatePerspectiveProjection(const float fov, const float aspectRatio, const float near, const float far);
	void CreateOrthographicProjection(const float left, const float right, const float bottom, const float top, const float near, const float far);

	Matrix4& operator*=(const Matrix4& b) {
		Vector4 tx = mat[0];
		Vector4 ty = mat[1];
		Vector4 tz = mat[2];
		Vector4 tw = mat[3];

		Vector4 b0 = Vector4(b.mat[0].x, b.mat[1].x, b.mat[2].x, b.mat[3].x);
		Vector4 b1 = Vector4(b.mat[0].y, b.mat[1].y, b.mat[2].y, b.mat[3].y);
		Vector4 b2 = Vector4(b.mat[0].z, b.mat[1].z, b.mat[2].z, b.mat[3].z);
		Vector4 b3 = Vector4(b.mat[0].w, b.mat[1].w, b.mat[2].w, b.mat[3].w);

		mat[0].x = tx.Dot(b0);
		mat[0].y = tx.Dot(b1);
		mat[0].z = tx.Dot(b2);
		mat[0].w = tx.Dot(b3);

		mat[1].x = ty.Dot(b0);
		mat[1].y = ty.Dot(b1);
		mat[1].z = ty.Dot(b2);
		mat[1].w = ty.Dot(b3);

		mat[2].x = tz.Dot(b0);
		mat[2].y = tz.Dot(b1);
		mat[2].z = tz.Dot(b2);
		mat[2].w = tz.Dot(b3);

		mat[3].x = tw.Dot(b0);
		mat[3].y = tw.Dot(b1);
		mat[3].z = tw.Dot(b2);
		mat[3].w = tw.Dot(b3);

		return *this;

	}

	Matrix4 Transposed() const {
		Matrix4 t;
		t[0] = Vector4(mat[0].x, mat[1].x, mat[2].x, mat[3].x);
		t[1] = Vector4(mat[0].y, mat[1].y, mat[2].y, mat[3].y);
		t[2] = Vector4(mat[0].z, mat[1].z, mat[2].z, mat[3].z);
		t[3] = Vector4(mat[0].w, mat[1].w, mat[2].w, mat[3].w);
		return t;
	}

	Matrix4& Transpose() {
		Vector4 m0 = { mat[0].x, mat[1].x, mat[2].x, mat[3].x };
		Vector4 m1 = { mat[0].y, mat[1].y, mat[2].y, mat[3].y };
		Vector4 m2 = { mat[0].z, mat[1].z, mat[2].z, mat[3].z };
		Vector4 m3 = { mat[0].w, mat[1].w, mat[2].w, mat[3].w };
		mat[0] = m0;
		mat[1] = m1;
		mat[2] = m2;
		mat[3] = m3;
		return *this;
	}

	const float* AsFloatPtr() const;

	Matrix4 Translate(const Vector3& v) const;
	Matrix4& Translate(const Vector3& v);
	Matrix4 Rotate(float angle, const Vector3& axis) const;
	Matrix4& Rotate(float angle, const Vector3& axis);

};

inline Vector4 operator*(const Matrix4& m, const Vector4& v) {
	float x, y, z, w;
	x = m.mat[0].Dot(v);
	y = m.mat[1].Dot(v);
	z = m.mat[2].Dot(v);
	w = m.mat[3].Dot(v);
	return Vector4(x, y, z, w);
}

inline Matrix4 operator*(const Matrix4& a, const Matrix4& b) {

	Matrix4 m;

	Vector4 tx = a.mat[0];
	Vector4 ty = a.mat[1];
	Vector4 tz = a.mat[2];
	Vector4 tw = a.mat[3];

	Vector4 b0 = Vector4(b.mat[0].x, b.mat[1].x, b.mat[2].x, b.mat[3].x);
	Vector4 b1 = Vector4(b.mat[0].y, b.mat[1].y, b.mat[2].y, b.mat[3].y);
	Vector4 b2 = Vector4(b.mat[0].z, b.mat[1].z, b.mat[2].z, b.mat[3].z);
	Vector4 b3 = Vector4(b.mat[0].w, b.mat[1].w, b.mat[2].w, b.mat[3].w);

	m.mat[0].x = tx.Dot(b0);
	m.mat[0].y = tx.Dot(b1);
	m.mat[0].z = tx.Dot(b2);
	m.mat[0].w = tx.Dot(b3);

	m.mat[1].x = ty.Dot(b0);
	m.mat[1].y = ty.Dot(b1);
	m.mat[1].z = ty.Dot(b2);
	m.mat[1].w = ty.Dot(b3);

	m.mat[2].x = tz.Dot(b0);
	m.mat[2].y = tz.Dot(b1);
	m.mat[2].z = tz.Dot(b2);
	m.mat[2].w = tz.Dot(b3);

	m.mat[3].x = tw.Dot(b0);
	m.mat[3].y = tw.Dot(b1);
	m.mat[3].z = tw.Dot(b2);
	m.mat[3].w = tw.Dot(b3);

	return m;
}


static inline float Det3(const Matrix4& m, int col0, int col1, int col2, int row0, int row1, int row2) {
	return m.mat[col0][row0] *
		(m.mat[col1][row1] * m.mat[col2][row2] -
			m.mat[col1][row2] * m.mat[col2][row1]) -
		m.mat[col1][row0] *
		(m.mat[col0][row1] * m.mat[col2][row2] -
			m.mat[col0][row2] * m.mat[col2][row1]) +
		m.mat[col2][row0] *
		(m.mat[col0][row1] * m.mat[col1][row2] -
			m.mat[col0][row2] * m.mat[col1][row1]);
}

static inline float Det4(const Matrix4& m) {
	float det;
	det = m[0][0] * Det3(m, 1, 2, 3, 1, 2, 3);
	det -= m[1][0] * Det3(m, 0, 2, 3, 1, 2, 3);
	det += m[2][0] * Det3(m, 0, 1, 3, 1, 2, 3);
	det -= m[3][0] * Det3(m, 0, 1, 2, 1, 2, 3);
	return det;
}

inline float Matrix4::Determinant() const {
	return Det4(*this);
}

inline Matrix4 Matrix4::Inverted() const {
	float det = Determinant();
	if (fabsf(det) < MATRIX_INVERSE_EPSILON) {
		// not invertible, return identity
		return Matrix4();
	}

	Matrix4 inv;
	det = 1.0f / det;

	inv.mat[0][0] = Det3(*this, 1, 2, 3, 1, 2, 3) * det;
	inv.mat[0][1] = -Det3(*this, 0, 2, 3, 1, 2, 3) * det;
	inv.mat[0][2] = Det3(*this, 0, 1, 3, 1, 2, 3) * det;
	inv.mat[0][3] = -Det3(*this, 0, 1, 2, 1, 2, 3) * det;
	inv.mat[1][0] = -Det3(*this, 1, 2, 3, 0, 2, 3) * det;
	inv.mat[1][1] = Det3(*this, 0, 2, 3, 0, 2, 3) * det;
	inv.mat[1][2] = -Det3(*this, 0, 1, 3, 0, 2, 3) * det;
	inv.mat[1][3] = Det3(*this, 0, 1, 2, 0, 2, 3) * det;
	inv.mat[2][0] = Det3(*this, 1, 2, 3, 0, 1, 3) * det;
	inv.mat[2][1] = -Det3(*this, 0, 2, 3, 0, 1, 3) * det;
	inv.mat[2][2] = Det3(*this, 0, 1, 3, 0, 1, 3) * det;
	inv.mat[2][3] = -Det3(*this, 0, 1, 2, 0, 1, 3) * det;
	inv.mat[3][0] = -Det3(*this, 1, 2, 3, 0, 1, 2) * det;
	inv.mat[3][1] = Det3(*this, 0, 2, 3, 0, 1, 2) * det;
	inv.mat[3][2] = -Det3(*this, 0, 1, 3, 0, 1, 2) * det;
	inv.mat[3][3] = Det3(*this, 0, 1, 2, 0, 1, 2) * det;

	return inv;

}

inline Matrix3 Matrix4::RotationMatrix() const {
	Matrix3 m;
	m.mat[0] = { mat[0].x, mat[0].y, mat[0].z };
	m.mat[1] = { mat[1].x, mat[1].y, mat[1].z };
	m.mat[2] = { mat[2].x, mat[2].y, mat[2].z };
	return m;
}

inline Matrix3 Matrix4::NormalMatrix() const {
	// we only care about the rotation (top Left 3x3)
	Matrix3 rotation = RotationMatrix();

	// it is not strictly necessary to multiply the 
	// result by the inverse of the determinant, but we 
	// would need to ensure we normalize any vectors transformed by this matrix...
	float det = rotation.Determinant();
	float invdet = 1.0f / det;

	Matrix3 normal;
	normal.mat[0] = rotation.mat[1].Cross(rotation.mat[2]);
	normal.mat[1] = rotation.mat[2].Cross(rotation.mat[0]);
	normal.mat[2] = rotation.mat[0].Cross(rotation.mat[1]);

	normal.mat[0] *= invdet;
	normal.mat[1] *= invdet;
	normal.mat[2] *= invdet;

	return normal;
}

inline const float* Matrix4::AsFloatPtr() const
{
	return &(mat[0].x);
}

inline Matrix4 Matrix4::Translate(const Vector3 & v) const
{
	Vector4 translation = mat[3];
	translation += Vector4(v, 1.0);
	return Matrix4(mat[0], mat[1], mat[2], translation);
}

inline Matrix4 & Matrix4::Translate(const Vector3 & v)
{
	mat[0].w += v.x;
	mat[1].w += v.y;
	mat[2].w += v.z;
	return *this;
}

inline Matrix4 Matrix4::Rotate(float angle, const Vector3 & ax) const
{
	float cosine = cosf(angle);
	float sine = sinf(angle);
	const Vector3 axis = ax.Normalized();
	const Vector3 t = (1.0f - cosine) * axis;

	Matrix4 rot;
	rot.mat[0].x = cosine + t.x * axis.x;
	//rot.mat[0].y = t.x * axis.y
	return rot;


}

inline Matrix4& Matrix4::Rotate(float angle, const Vector3 & ax)
{
	// TODO: insert return statement here
	float cosine = cosf(angle);
	float sine = sinf(angle); 
	const Vector3 axis = ax.Normalized();
	const Vector3 t = (1.0f - cosine) * axis;

	mat[0].x = cosine + t.x * axis.x;
	//rot.mat[0].y = t.x * axis.y
	return *this;

}

inline Matrix4 TranslationMatrix(const Vector3& vec) {
	Matrix4 mat;
	mat.SetToIdentity();
	mat[0].w = vec.x;
	mat[1].w = vec.y;
	mat[2].w = vec.z;
	mat[3].w = 1.0f;
	
}



}


#endif