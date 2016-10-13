#include "quaternion.h"

namespace Jasper {

	Matrix3 Quaternion::ToMatrix3() const {
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

	Matrix4 Quaternion::ToMatrix4() const {
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
}