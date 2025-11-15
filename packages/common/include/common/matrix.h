#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <math.h>
#include "vector4.h"
#include "quaternion.h"

// row-major!
typedef struct {
    // m[row][column]
    float m[4][4];
} mat_t;

inline bool operator==(const mat_t& a, const mat_t& b) {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (!CompareFloat(a.m[x][y], b.m[x][y])) {
                return false;
            }
        }
    }

    return true;
}

inline Vector4 operator*(const mat_t& mat, const Vector4& vec) {
    return {
        mat.m[0][0] * vec.x + mat.m[0][1] * vec.y + mat.m[0][2] * vec.z + mat.m[0][3] * vec.w,
        mat.m[1][0] * vec.x + mat.m[1][1] * vec.y + mat.m[1][2] * vec.z + mat.m[1][3] * vec.w,
        mat.m[2][0] * vec.x + mat.m[2][1] * vec.y + mat.m[2][2] * vec.z + mat.m[2][3] * vec.w,
        mat.m[3][0] * vec.x + mat.m[3][1] * vec.y + mat.m[3][2] * vec.z + mat.m[3][3] * vec.w
    };
}

inline mat_t operator*(const mat_t& a, const mat_t& b) {
    return {
        {
        {
            a.m[0][0] * b.m[0][0]
            + a.m[0][1] * b.m[1][0]
            + a.m[0][2] * b.m[2][0]
            + a.m[0][3] * b.m[3][0],
            a.m[0][0] * b.m[0][1]
            + a.m[0][1] * b.m[1][1]
            + a.m[0][2] * b.m[2][1]
            + a.m[0][3] * b.m[3][1],
            a.m[0][0] * b.m[0][2]
            + a.m[0][1] * b.m[1][2]
            + a.m[0][2] * b.m[2][2]
            + a.m[0][3] * b.m[3][2],
            a.m[0][0] * b.m[0][3]
            + a.m[0][1] * b.m[1][3]
            + a.m[0][2] * b.m[2][3]
            + a.m[0][3] * b.m[3][3],
        },
        {
            a.m[1][0] * b.m[0][0]
            + a.m[1][1] * b.m[1][0]
            + a.m[1][2] * b.m[2][0]
            + a.m[1][3] * b.m[3][0],
            a.m[1][0] * b.m[0][1]
            + a.m[1][1] * b.m[1][1]
            + a.m[1][2] * b.m[2][1]
            + a.m[1][3] * b.m[3][1],
            a.m[1][0] * b.m[0][2]
            + a.m[1][1] * b.m[1][2]
            + a.m[1][2] * b.m[2][2]
            + a.m[1][3] * b.m[3][2],
            a.m[1][0] * b.m[0][3]
            + a.m[1][1] * b.m[1][3]
            + a.m[1][2] * b.m[2][3]
            + a.m[1][3] * b.m[3][3],
        },
        {
            a.m[2][0] * b.m[0][0]
            + a.m[2][1] * b.m[1][0]
            + a.m[2][2] * b.m[2][0]
            + a.m[2][3] * b.m[3][0],
            a.m[2][0] * b.m[0][1]
            + a.m[2][1] * b.m[1][1]
            + a.m[2][2] * b.m[2][1]
            + a.m[2][3] * b.m[3][1],
            a.m[2][0] * b.m[0][2]
            + a.m[2][1] * b.m[1][2]
            + a.m[2][2] * b.m[2][2]
            + a.m[2][3] * b.m[3][2],
            a.m[2][0] * b.m[0][3]
            + a.m[2][1] * b.m[1][3]
            + a.m[2][2] * b.m[2][3]
            + a.m[2][3] * b.m[3][3],
        },
        {
            a.m[3][0] * b.m[0][0]
            + a.m[3][1] * b.m[1][0]
            + a.m[3][2] * b.m[2][0]
            + a.m[3][3] * b.m[3][0],
            a.m[3][0] * b.m[0][1]
            + a.m[3][1] * b.m[1][1]
            + a.m[3][2] * b.m[2][1]
            + a.m[3][3] * b.m[3][1],
            a.m[3][0] * b.m[0][2]
            + a.m[3][1] * b.m[1][2]
            + a.m[3][2] * b.m[2][2]
            + a.m[3][3] * b.m[3][2],
            a.m[3][0] * b.m[0][3]
            + a.m[3][1] * b.m[1][3]
            + a.m[3][2] * b.m[2][3]
            + a.m[3][3] * b.m[3][3],
        },
        }
    };
}

inline mat_t PMathMatIdentity() {
    return {
        {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        },
    };
}

inline mat_t PMathMatTranspose(const mat_t& m) {
    return {
        {
            { m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0] },
            { m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1] },
            { m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2] },
            { m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3] },
        }
    };
}

inline mat_t PMathMatTranslation(float x, float y, float z) {
    return {
        {
            {1, 0, 0, x},
            {0, 1, 0, y},
            {0, 0, 1, z},
            {0, 0, 0, 1}
        }
    };
}

inline mat_t PMathMatRotation(const mat_t& mat, const Quaternion& q) {
    /*
    */

    float m[4][4] = { {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} };

    float x = q.x, y = q.y, z = q.z, w = q.w;
    float x2 = x + x, y2 = y + y, z2 = z + z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2, zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;

    return {
        {
            {
                m[0][0] = 1.0f - (yy + zz),
                m[0][1] = xy - wz,
                m[0][2] = xz + wy,
                m[0][3] = 0.0f
            }, {
                m[1][0] = xy + wz,
                m[1][1] = 1.0f - (xx + zz),
                m[1][2] = yz - wx,
                m[1][3] = 0
            }, {
                m[2][0] = xz - wy,
                m[2][1] = yz + wx,
                m[2][2] = 1.0f - (xx + yy),
                m[2][3] = 0
            }, {
                m[3][0] = 0,
                m[3][1] = 0,
                m[3][2] = 0,
                m[3][3] = 1.0f
            }
        }
    };
}

inline mat_t PMathMatRotation(float yaw, float pitch, float roll) {
    const float halfC = (float)(M_PI / 180.0f);

    yaw *= halfC;
    pitch *= halfC;
    roll *= halfC;

    return {
        {
        {
            cosf(yaw) * cosf(pitch),
            cosf(yaw) * sinf(pitch) * sinf(roll) - sinf(yaw) * cosf(roll),
            cosf(yaw) * sinf(pitch) * cosf(roll) + sinf(yaw) * sinf(roll),
            0
        },
        {
            sinf(yaw) * cosf(pitch),
            sinf(yaw) * sinf(pitch) * sinf(roll) + cosf(yaw) * cosf(roll),
            sinf(yaw) * sinf(pitch) * cosf(roll) - cosf(yaw) * sinf(roll),
            0
        },
        {
            -sinf(pitch),
            cosf(pitch) * sinf(roll),
            cosf(pitch) * cosf(roll),
            0
        },
        {0, 0, 0, 1}
        }
    };
}

inline float PMathMatDeterminant(const mat_t& m) {
    return m.m[0][0] * (m.m[1][1] * m.m[2][2] * m.m[3][3]
        + m.m[1][2] * m.m[2][3] * m.m[3][1]
        + m.m[1][3] * m.m[2][1] * m.m[3][2]
        - m.m[1][3] * m.m[2][2] * m.m[3][1]
        - m.m[1][2] * m.m[2][1] * m.m[3][3]
        - m.m[1][1] * m.m[2][3] * m.m[3][2])
        - m.m[1][0] * (m.m[0][1] * m.m[2][2] * m.m[3][3]
            + m.m[0][2] * m.m[2][3] * m.m[3][1]
            + m.m[0][3] * m.m[2][1] * m.m[3][2]
            - m.m[0][3] * m.m[2][2] * m.m[3][1]
            - m.m[0][2] * m.m[2][1] * m.m[3][3]
            - m.m[0][1] * m.m[2][3] * m.m[3][2])
        + m.m[2][0] * (m.m[0][1] * m.m[1][2] * m.m[3][3]
            + m.m[0][2] * m.m[1][3] * m.m[3][1]
            + m.m[0][3] * m.m[1][1] * m.m[3][2]
            - m.m[0][3] * m.m[1][2] * m.m[3][1]
            - m.m[0][2] * m.m[1][1] * m.m[3][3]
            - m.m[0][1] * m.m[1][3] * m.m[3][2])
        - m.m[3][0] * (m.m[0][1] * m.m[1][2] * m.m[2][3]
            + m.m[0][2] * m.m[1][3] * m.m[2][1]
            + m.m[0][3] * m.m[1][1] * m.m[2][2]
            - m.m[0][3] * m.m[1][2] * m.m[2][1]
            - m.m[0][2] * m.m[1][1] * m.m[2][3]
            - m.m[0][1] * m.m[1][3] * m.m[2][2]);
}

inline mat_t PMathMatInverse(const mat_t& m) {
    float a2323 = m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2];
    float a1323 = m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1];
    float a1223 = m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1];
    float a0323 = m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0];
    float a0223 = m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0];
    float a0123 = m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0];
    float a2313 = m.m[1][2] * m.m[3][3] - m.m[1][3] * m.m[3][2];
    float a1313 = m.m[1][1] * m.m[3][3] - m.m[1][3] * m.m[3][1];
    float a1213 = m.m[1][1] * m.m[3][2] - m.m[1][2] * m.m[3][1];
    float a2312 = m.m[1][2] * m.m[2][3] - m.m[1][3] * m.m[2][2];
    float a1312 = m.m[1][1] * m.m[2][3] - m.m[1][3] * m.m[2][1];
    float a1212 = m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1];
    float a0313 = m.m[1][0] * m.m[3][3] - m.m[1][3] * m.m[3][0];
    float a0213 = m.m[1][0] * m.m[3][2] - m.m[1][2] * m.m[3][0];
    float a0312 = m.m[1][0] * m.m[2][3] - m.m[1][3] * m.m[2][0];
    float a0212 = m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0];
    float a0113 = m.m[1][0] * m.m[3][1] - m.m[1][1] * m.m[3][0];
    float a0112 = m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0];

    float det = PMathMatDeterminant(m);
    det = 1 / det;

    return {
        {
        {
            det * (m.m[1][1] * a2323 - m.m[1][2] * a1323 + m.m[1][3] * a1223),
            det * -(m.m[0][1] * a2323 - m.m[0][2] * a1323 + m.m[0][3] * a1223),
            det * (m.m[0][1] * a2313 - m.m[0][2] * a1313 + m.m[0][3] * a1213),
            det * -(m.m[0][1] * a2312 - m.m[0][2] * a1312 + m.m[0][3] * a1212),
        },
        {
            det * -(m.m[1][0] * a2323 - m.m[1][2] * a0323 + m.m[1][3] * a0223),
            det * (m.m[0][0] * a2323 - m.m[0][2] * a0323 + m.m[0][3] * a0223),
            det * -(m.m[0][0] * a2313 - m.m[0][2] * a0313 + m.m[0][3] * a0213),
            det * (m.m[0][0] * a2312 - m.m[0][2] * a0312 + m.m[0][3] * a0212),
        },
        {
            det * (m.m[1][0] * a1323 - m.m[1][1] * a0323 + m.m[1][3] * a0123),
            det * -(m.m[0][0] * a1323 - m.m[0][1] * a0323 + m.m[0][3] * a0123),
            det * (m.m[0][0] * a1313 - m.m[0][1] * a0313 + m.m[0][3] * a0113),
            det * -(m.m[0][0] * a1312 - m.m[0][1] * a0312 + m.m[0][3] * a0112),
        },
        {
            det * -(m.m[1][0] * a1223 - m.m[1][1] * a0223 + m.m[1][2] * a0123),
            det * (m.m[0][0] * a1223 - m.m[0][1] * a0223 + m.m[0][2] * a0123),
            det * -(m.m[0][0] * a1213 - m.m[0][1] * a0213 + m.m[0][2] * a0113),
            det * (m.m[0][0] * a1212 - m.m[0][1] * a0212 + m.m[0][2] * a0112),
        }
        }
    };
}

inline mat_t PMathMatRotationX(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat_t m = PMathMatIdentity();
    m.m[1][1] = c;  m.m[1][2] = -s;
    m.m[2][1] = s; m.m[2][2] = c;
    return m;
}

inline mat_t PMathMatRotationY(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat_t m = PMathMatIdentity();
    m.m[0][0] = c;  m.m[0][2] = s;
    m.m[2][0] = -s;  m.m[2][2] = c;
    return m;
}

inline mat_t PMathMatRotationZ(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat_t m = PMathMatIdentity();
    m.m[0][0] = c;  m.m[0][1] = s;
    m.m[1][0] = -s; m.m[1][1] = c;
    return m;
}

// Roll (Z) → Pitch (X) → Yaw (Y)
inline mat_t PMathMatRollPitchYaw(float roll, float pitch, float yaw) {
    mat_t Rz = PMathMatRotationZ(roll);
    mat_t Rx = PMathMatRotationX(pitch);
    mat_t Ry = PMathMatRotationY(yaw);

    return Ry * Rx * Rz;
}

inline mat_t PMathMatRotationQuaternion(const Quaternion& q) {
    mat_t m = PMathMatIdentity();

    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    // RIGHT-HANDED VERSION
    m.m[0][0] = 1.0f - 2.0f * (yy + zz);
    m.m[0][1] = 2.0f * (xy - wz);
    m.m[0][2] = 2.0f * (xz + wy);

    m.m[1][0] = 2.0f * (xy + wz);
    m.m[1][1] = 1.0f - 2.0f * (xx + zz);
    m.m[1][2] = 2.0f * (yz - wx);

    m.m[2][0] = 2.0f * (xz - wy);
    m.m[2][1] = 2.0f * (yz + wx);
    m.m[2][2] = 1.0f - 2.0f * (xx + yy);

    m.m[3][0] = 0.0f;
    m.m[3][1] = 0.0f;
    m.m[3][2] = 0.0f;
    m.m[3][3] = 1.0f;

    return m;
}


inline mat_t PMathMatScaling(float fScaleX, float fScaleY, float fScaleZ) {
    mat_t m = PMathMatIdentity();

    m.m[0][0] = fScaleX;
    m.m[1][1] = fScaleY;
    m.m[2][2] = fScaleZ;

    return m;
}

/**
 * Creates a perspective projection matrix for a right handed coordinate system.
 * Assumes NDCz = [0, 1]!
 * @param fovV Vertical field of view of the camera in degrees
 * @param aspect Aspect ratio of the viewing plane
 * @param nearZ Distance to the near viewing plane from the camera
 * @param farZ Distance to the far viewing plane from the camrea
 */
inline mat_t PMathMatPerspectiveRH(float fovV, float aspect, float nearZ, float farZ) {
    // works correctly as of 15/11/2025 for
    // RH coordinate system
    // z- is forward in camera space
    // NDCz is in [0, 1]
    // row-major matrices
    // column-vector
    float halfFovV = fovV / 2.0f;
    float yScale = 1.0f / tanf(halfFovV);
    float xScale = yScale / aspect;
    mat_t m {};

    // scale
    m.m[0][0] = xScale;
    m.m[1][1] = yScale;

    // z mapping
    m.m[2][2] = farZ / (nearZ - farZ);
    m.m[3][2] = (farZ * nearZ) / (nearZ - farZ);
    m.m[2][3] = -1.0f;

    return m;
}

inline mat_t PMathMatAffineTransform(const Vector3& scaling, const Vector3& rotationOrigin, const Quaternion& rotationQuat, const Vector3& translation) {
    // 1. Scale matrix
    mat_t S = PMathMatScaling(scaling.x, scaling.y, scaling.z);

    // 2. Rotation matrix
    mat_t R = PMathMatRotationQuaternion(rotationQuat);

    // 3. Translation to/from rotation origin
    mat_t T_negOrigin = PMathMatTranslation(-rotationOrigin.x, -rotationOrigin.y, -rotationOrigin.z);
    mat_t T_origin     = PMathMatTranslation(rotationOrigin.x, rotationOrigin.y, rotationOrigin.z);

    // 4. Final translation
    mat_t T = PMathMatTranslation(translation.x, translation.y, translation.z);

    return T * T_negOrigin * S * R * T_origin;
}