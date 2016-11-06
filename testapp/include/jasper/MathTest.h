#ifndef _JASPER_MATH_TEST_H_
#define _JASPER_MATH_TEST_H_

#include <bullet\btBulletDynamicsCommon.h>
#include "vector.h"
#include "matrix.h"
#include "quaternion.h"
#include "Transform.h"

namespace Jasper
{


bool compareQuats(const btQuaternion& btq, const Quaternion& q)
{
    if (fabs(q.x - btq.x()) < 0.00001) {
        if (fabs(q.y - btq.y()) < 0.00001) {
            if (fabs(q.z - btq.z()) < 0.00001) {
                if (fabs(q.w - btq.w()) < 0.00001) {
                    return true;
                }
            }
        }
    }
    return false;
}


class MathTest
{
public:
    bool testQuaternionMultiplication() {
        btQuaternion btq0 = btQuaternion(btVector3(0, 1, 0), 90 * SIMD_RADS_PER_DEG);
        Quaternion q0 = Quaternion::FromAxisAndAngle( { 0, 1, 0 }, 90 * SIMD_RADS_PER_DEG);

        btQuaternion btq1 = btQuaternion(btVector3(0.45f, 8.54f, 0.34f), 36 * SIMD_RADS_PER_DEG);
        Quaternion q1 = Quaternion::FromAxisAndAngle( { 0.45f, 8.54f, 0.34f }, 36 * SIMD_RADS_PER_DEG);
        btQuaternion btqFinal = btq0 * btq1;
        Quaternion qFinal = q0 * q1;

        if (compareQuats(btqFinal, qFinal)) {
            return true;
        }
        return false;

    }

    bool testTransforms() {
        btQuaternion btq0 = btQuaternion(btVector3(0, 1, 0), 90 * SIMD_RADS_PER_DEG);
        Quaternion q0 = Quaternion::FromAxisAndAngle( { 0, 1, 0 }, 90 * SIMD_RADS_PER_DEG);
        btQuaternion btq1 = btQuaternion(btVector3(0.45f, 8.54f, 0.34f), 36 * SIMD_RADS_PER_DEG);
        Quaternion q1 = Quaternion::FromAxisAndAngle( { 0.45f, 8.54f, 0.34f }, 36 * SIMD_RADS_PER_DEG);
        btQuaternion btqFinal = btq0 * btq1;
        Quaternion qFinal = q0 * q1;

        if (compareQuats(btqFinal, qFinal)) {
            Vector3 position = { 10.0f, 34.2f, 76.8876f };
            btVector3 btPosition = { 10.0f, 34.2f, 76.8876f };

            btTransform btTrans;
            btTrans.setRotation(btqFinal);
            btTrans.setOrigin(btPosition);

            Transform trans;
            trans.Position = position;
            trans.Orientation = qFinal;

            Matrix4 fromJ = Matrix4::FromTransform(trans);
            Matrix4 trJ = fromJ.Transposed();
            float btMatrix[16];
            btTrans.getOpenGLMatrix(btMatrix);

            printf("Hello");
        }

        return false;

    }

};




}
#endif // _JASPER_MATH_TEST_H_
