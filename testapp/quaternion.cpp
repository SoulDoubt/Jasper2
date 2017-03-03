#include "quaternion.h"

namespace Jasper
{

Matrix3 Quaternion::ToMatrix3() const
{
    Matrix3 m;
    float	wx, wy, wz;
    float	xx, yy, yz;
    float	xy, xz, zz;
    float	x2, y2, z2;

    x2 = x + x;
    y2 = y + y;
    z2 = z + z;

    xx = x * x2;
    xy = x * y2;
    xz = x * z2;

    yy = y * y2;
    yz = y * z2;
    zz = z * z2;

    wx = w * x2;
    wy = w * y2;
    wz = w * z2;

    m.mat[0][0] = 1.0f - (yy + zz);
    m.mat[0][1] = xy - wz;
    m.mat[0][2] = xz + wy;

    m.mat[1][0] = xy + wz;
    m.mat[1][1] = 1.0f - (xx + zz);
    m.mat[1][2] = yz - wx;

    m.mat[2][0] = xz - wy;
    m.mat[2][1] = yz + wx;
    m.mat[2][2] = 1.0f - (xx + yy);

    return m;

}

Matrix4 Quaternion::ToMatrix4() const
{
    Matrix4 m;
    float	wx, wy, wz;
    float	xx, yy, yz;
    float	xy, xz, zz;
    float	x2, y2, z2;

    x2 = x + x;
    y2 = y + y;
    z2 = z + z;

    xx = x * x2;
    xy = x * y2;
    xz = x * z2;

    yy = y * y2;
    yz = y * z2;
    zz = z * z2;

    wx = w * x2;
    wy = w * y2;
    wz = w * z2;

    m.mat[0][0] = 1.0f - (yy + zz);
    m.mat[0][1] = xy - wz;
    m.mat[0][2] = xz + wy;
    m.mat[0][3] = 0.0f;

    m.mat[1][0] = xy + wz;
    m.mat[1][1] = 1.0f - (xx + zz);
    m.mat[1][2] = yz - wx;
    m.mat[1][3] = 0.0f;

    m.mat[2][0] = xz - wy;
    m.mat[2][1] = yz + wx;
    m.mat[2][2] = 1.0f - (xx + yy);
    m.mat[2][3] = 0.0f;

    m.mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

    return m;
}

Quaternion::Quaternion(const Matrix3& m)
{
	/*
	w = Math.sqrt(1.0 + m1.m00 + m1.m11 + m1.m22) / 2.0;
	double w4 = (4.0 * w);
	x = (m1.m21 - m1.m12) / w4 ;
	y = (m1.m02 - m1.m20) / w4 ;
	z = (m1.m10 - m1.m01) / w4 ;
	*/
	/*w = sqrtf(1.0f + m.mat[0].x + m.mat[1].y + m.mat[2].z) / 2.0f;
	float w4 = 4.0f * w;
	x = (m.mat[2].x - m.mat[1].z) / w4;
	y = (m.mat[0].z - m.mat[2].x) / w4;
	z = (m.mat[1].x - m.mat[0].y) / w4;*/

	float t = m.mat[0].x + m.mat[1].y + m.mat[2].z;

	// large enough
	if (t > 0.f)
	{
		float s = std::sqrt(1 + t) * static_cast<float>(2.0);
		x = (m.mat[2].y - m.mat[1].z) / s;
		y = (m.mat[0].z - m.mat[2].x) / s;
		z = (m.mat[1].x - m.mat[0].y) / s;
		w = static_cast<float>(0.25) * s;
	} // else we have to check several cases
	else if (m.mat[0].x > m.mat[1].y && m.mat[0].x > m.mat[2].z)
	{
		// Column 0:
		float s = std::sqrt(static_cast<float>(1.0) + m.mat[0].x - m.mat[1].y - m.mat[2].z) * static_cast<float>(2.0);
		x = static_cast<float>(0.25) * s;
		y = (m.mat[1].x + m.mat[0].y) / s;
		z = (m.mat[0].z + m.mat[2].x) / s;
		w = (m.mat[2].y - m.mat[1].z) / s;
	}
	else if (m.mat[1].y > m.mat[2].z)
	{
		// Column 1:
		float s = std::sqrt(static_cast<float>(1.0) + m.mat[1].y - m.mat[0].x - m.mat[2].z) * static_cast<float>(2.0);
		x = (m.mat[1].x + m.mat[0].y) / s;
		y = static_cast<float>(0.25) * s;
		z = (m.mat[2].y + m.mat[1].z) / s;
		w = (m.mat[0].z - m.mat[2].x) / s;
	}
	else
	{
		// Column 2:
		float s = std::sqrt(static_cast<float>(1.0) + m.mat[2].z - m.mat[0].x - m.mat[1].y) * static_cast<float>(2.0);
		x = (m.mat[0].z + m.mat[2].x) / s;
		y = (m.mat[2].y + m.mat[1].z) / s;
		z = static_cast<float>(0.25) * s;
		w = (m.mat[1].x - m.mat[0].y) / s;
	}
	Normalize(*this);
}

}
