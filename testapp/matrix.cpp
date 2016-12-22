#include "matrix.h"
#include "quaternion.h"
#include "Transform.h"


namespace Jasper
{

void Matrix4::CreatePerspectiveProjection(const float fov, const float aspect, const float nearPlane, const float farPlane)
{
    // Bail out if the projection volume is zero-sized.
    if (nearPlane == farPlane || aspect == 0.0f)
        return;
    // Construct the projection.
    Matrix4 m;
    const float radians = (fov / 2.0f) * (float)M_PI / 180.0f;
    const float sine = sinf(radians);
    if (sine == 0.0f)
        return;
    const float cotan = cosf(radians) / sine;
    const float clip = farPlane - nearPlane;
    m.mat[0][0] = cotan / aspect;
    m.mat[0][1] = 0.0f;
    m.mat[0][2] = 0.0f;
    m.mat[0][3] = 0.0f;

    m.mat[1][0] = 0.0f;
    m.mat[1][1] = cotan;
    m.mat[1][2] = 0.0f;
    m.mat[1][3] = 0.0f;

    m.mat[2][0] = 0.0f;
    m.mat[2][1] = 0.0f;
    m.mat[2][2] = -(nearPlane + farPlane) / clip;
    m.mat[2][3] = -(2.0f * nearPlane * farPlane) / clip;

    m.mat[3][0] = 0.0f;
    m.mat[3][1] = 0.0f;
    m.mat[3][2] = -1.0f;
    m.mat[3][3] = 0.0f;

    // Apply the projection.
    *this *= m;

}

Matrix4 CreateOrtho(const float left, const float right, const float bottom, const float top, const float near, const float far)
{
    Matrix4 m = Matrix4();
    m.mat[0][0] = 2.0f / (right - left);
    m.mat[0][1] = 0.0f;
    m.mat[0][2] = 0.0f;
    m.mat[0][3] = -(right + left) / (right - left);

    m.mat[1][0] = 0.0f;
    m.mat[1][1] = 2.0f / (top - bottom);
    m.mat[1][2] = 0.0f;
    m.mat[1][3] = -(top + bottom) / (top - bottom);;

    m.mat[2][0] = 0.0f;
    m.mat[2][1] = 0.0f;
    m.mat[2][2] = -2.0f / (far - near);
    m.mat[2][3] = -(far + near) / (far - near);

    m.mat[3][0] = 0.0f;//
    m.mat[3][1] = 0.0f;//
    m.mat[3][2] = 0.0f;// ;
    m.mat[3][3] = 1.0f;

    return m;
}

void Matrix4::CreateOrthographicProjection(const float left, const float right, const float bottom, const float top, const float near, const float far)
{

    Matrix4 m = Matrix4();
    m.mat[0][0] = 2.0f / (right - left);
    m.mat[0][1] = 0.0f;
    m.mat[0][2] = 0.0f;
    m.mat[0][3] = -(right + left) / (right - left);

    m.mat[1][0] = 0.0f;
    m.mat[1][1] = 2.0f / (top - bottom);
    m.mat[1][2] = 0.0f;
    m.mat[1][3] = -(top + bottom) / (top - bottom);;

    m.mat[2][0] = 0.0f;
    m.mat[2][1] = 0.0f;
    m.mat[2][2] = -2.0f / (far - near);
    m.mat[2][3] = -(far + near) / (far - near);

    m.mat[3][0] = 0.0f;//
    m.mat[3][1] = 0.0f;//
    m.mat[3][2] = 0.0f;// ;
    m.mat[3][3] = 1.0f;
    //m.Transpose();
    *this =  *this * m;
}



Matrix4 Matrix4::FromTransform(const Transform& tr)
{

    const Quaternion q = Normalize(tr.Orientation);
    const auto p = tr.Position;
    const auto scale = tr.Scale;

    const float xx = q.x * q.x;
    const float yy = q.y * q.y;
    const float yz = q.y * q.z;
    const float zz = q.z * q.z;
    const float xy = q.x * q.y;
    const float zw = q.z * q.w;
    const float xz = q.x * q.z;
    const float yw = q.y * q.w;
    const float xw = q.x * q.w;

    Matrix4 mat;

    mat.mat[0].x = 1.0f - (2.0f * yy) - (2.0f * zz);
    mat.mat[0].y = (2.0f * xy) - (2.0f * zw);
    mat.mat[0].z = (2.0f * xz) + (2.0f * yw);
    mat.mat[0].w = p.x;

    mat.mat[1].x = (2.0f * xy) + (2.0f * zw);
    mat.mat[1].y = 1.0f - (2.0f * xx) - (2.0f * zz);
    mat.mat[1].z = (2.0f * yz) - (2.0f * xw);
    mat.mat[1].w = p.y;

    mat.mat[2].x = (2.0f * xz) - (2.0f * yw);
    mat.mat[2].y = (2.0f * yz) + (2.0f * xw);
    mat.mat[2].z = 1.0f - (2.0f * xx) - (2.0f * yy);
    mat.mat[2].w = p.z;

    mat.mat[3].x = 0.f;
    mat.mat[3].y = 0.f;
    mat.mat[3].z = 0.f;
    mat.mat[3].w = 1.0f;

    if (scale != Vector3(1.f, 1.f, 1.f)) {
        Matrix4 scaleMat;
        scaleMat.mat[0].x = scale.x;
        scaleMat.mat[1].y = scale.y;
        scaleMat.mat[2].z = scale.z;
        return mat * scaleMat;
    }

    return mat;
}

} // namespace jasper
