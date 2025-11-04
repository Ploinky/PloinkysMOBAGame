#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <math.h>
#include "vector4.h"
#include "quaternion.h"

typedef struct mat {
    // m[row][column]
    float m[4][4] = { 0 };

    bool operator==(const mat& other) const {
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                if (!CompareFloat(m[x][y], other.m[x][y])) {
                    return false;
                }
            }
        }

        return true;
    }

    Vector4 operator*(const Vector4& vec) const {
        return {
            m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z + m[0][3] * vec.w,
            m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z + m[1][3] * vec.w,
            m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z + m[2][3] * vec.w,
            m[3][0] * vec.x + m[3][1] * vec.y + m[3][2] * vec.z + m[3][3] * vec.w
        };
    }

    mat operator*(const mat& other) const {
        return {
            {
            {
                m[0][0] * other.m[0][0]
                + m[0][1] * other.m[1][0]
                + m[0][2] * other.m[2][0]
                + m[0][3] * other.m[3][0],
                m[0][0] * other.m[0][1]
                + m[0][1] * other.m[1][1]
                + m[0][2] * other.m[2][1]
                + m[0][3] * other.m[3][1],
                m[0][0] * other.m[0][2]
                + m[0][1] * other.m[1][2]
                + m[0][2] * other.m[2][2]
                + m[0][3] * other.m[3][2],
                m[0][0] * other.m[0][3]
                + m[0][1] * other.m[1][3]
                + m[0][2] * other.m[2][3]
                + m[0][3] * other.m[3][3],
            },
            {
                m[1][0] * other.m[0][0]
                + m[1][1] * other.m[1][0]
                + m[1][2] * other.m[2][0]
                + m[1][3] * other.m[3][0],
                m[1][0] * other.m[0][1]
                + m[1][1] * other.m[1][1]
                + m[1][2] * other.m[2][1]
                + m[1][3] * other.m[3][1],
                m[1][0] * other.m[0][2]
                + m[1][1] * other.m[1][2]
                + m[1][2] * other.m[2][2]
                + m[1][3] * other.m[3][2],
                m[1][0] * other.m[0][3]
                + m[1][1] * other.m[1][3]
                + m[1][2] * other.m[2][3]
                + m[1][3] * other.m[3][3],
            },
            {
                m[2][0] * other.m[0][0]
                + m[2][1] * other.m[1][0]
                + m[2][2] * other.m[2][0]
                + m[2][3] * other.m[3][0],
                m[2][0] * other.m[0][1]
                + m[2][1] * other.m[1][1]
                + m[2][2] * other.m[2][1]
                + m[2][3] * other.m[3][1],
                m[2][0] * other.m[0][2]
                + m[2][1] * other.m[1][2]
                + m[2][2] * other.m[2][2]
                + m[2][3] * other.m[3][2],
                m[2][0] * other.m[0][3]
                + m[2][1] * other.m[1][3]
                + m[2][2] * other.m[2][3]
                + m[2][3] * other.m[3][3],
            },
            {
                m[3][0] * other.m[0][0]
                + m[3][1] * other.m[1][0]
                + m[3][2] * other.m[2][0]
                + m[3][3] * other.m[3][0],
                m[3][0] * other.m[0][1]
                + m[3][1] * other.m[1][1]
                + m[3][2] * other.m[2][1]
                + m[3][3] * other.m[3][1],
                m[3][0] * other.m[0][2]
                + m[3][1] * other.m[1][2]
                + m[3][2] * other.m[2][2]
                + m[3][3] * other.m[3][2],
                m[3][0] * other.m[0][3]
                + m[3][1] * other.m[1][3]
                + m[3][2] * other.m[2][3]
                + m[3][3] * other.m[3][3],
            },
            }
        };
    }

    static mat Identity() {
        return {
            {
                {1, 0, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 1}
            },
        };
    }

    mat Transpose() {
        return {
            {
            { m[0][0], m[1][0], m[2][0], m[3][0] },
            { m[0][1], m[1][1], m[2][1], m[3][1] },
            { m[0][2], m[1][2], m[2][2], m[3][2] },
            { m[0][3], m[1][3], m[2][3], m[3][3] },
            }
        };
    }

    static mat Perspective(float aspect, float fov, float nearZ, float farZ) {
        return {
            {
                {1 / (aspect * tanf(fov / 2)), 0, 0, 0},
                {0, 1 / (tanf(fov / 2)), 0, 0},
                {0, 0, (nearZ + farZ) / (nearZ - farZ), (2 * farZ * nearZ) / (nearZ - farZ)},
                {0, 0, -1, 0}
            },
        };
    }

    static mat Translation(float x, float y, float z) {
        return {
            {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {x, y, z, 1}
            }
        };
    }

    static mat Rotation(Quaternion q) {
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
        /*
        DirectX::XMVECTOR vec{q.x, q.y, q.z, q.w};
        vec = DirectX::XMVectorSetX(vec, q.x);
        vec = DirectX::XMVectorSetY(vec, q.y);
        vec = DirectX::XMVectorSetZ(vec, q.z);
        vec = DirectX::XMVectorSetW(vec, q.w);
        DirectX::XMMATRIX matrix = DirectX::XMMatrixRotationQuaternion(vec);

        return {
            {
                {
                    DirectX::XMVectorGetX(matrix.r[0]),
                    DirectX::XMVectorGetY(matrix.r[0]),
                    DirectX::XMVectorGetZ(matrix.r[0]),
                    DirectX::XMVectorGetW(matrix.r[0]),
                }, {
                    DirectX::XMVectorGetX(matrix.r[1]),
                    DirectX::XMVectorGetY(matrix.r[1]),
                    DirectX::XMVectorGetZ(matrix.r[1]),
                    DirectX::XMVectorGetW(matrix.r[1]),
                }, {
                    DirectX::XMVectorGetX(matrix.r[2]),
                    DirectX::XMVectorGetY(matrix.r[2]),
                    DirectX::XMVectorGetZ(matrix.r[2]),
                    DirectX::XMVectorGetW(matrix.r[2]),
                }, {
                    DirectX::XMVectorGetX(matrix.r[3]),
                    DirectX::XMVectorGetY(matrix.r[3]),
                    DirectX::XMVectorGetZ(matrix.r[3]),
                    DirectX::XMVectorGetW(matrix.r[3]),
                }
            }
        };
        */

        /*  
        return {
            {
                {
                    1.0f - 2.0f * (float) q.y * (float)q.y - 2.0f * (float)q.z * (float)q.z,
                    2.0f * (float)q.x * (float)q.y + 2.0f * (float)q.w * (float)q.z,
                    2.0f * (float)q.x * (float)q.z - 2.0f * (float)q.w * (float)q.y,
                    0.0f
                },
                {
                    2.0f * (float)q.x * (float)q.y - 2.0f * (float)q.w * (float)q.z,
                    1.0f - 2.0f * (float)q.x * (float)q.x - 2.0f * (float)q.z * (float)q.z,
                    2.0f * (float)q.y * (float)q.z + 2.0f * (float)q.w * (float)q.x,
                    0.0f
                },
                {
                    2.0f * (float)q.x * (float)q.z + 2.0f * (float)q.w * (float)q.y,
                    2.0f * (float)q.y * (float)q.z - 2.0f * (float)q.w * (float)q.x,
                    1.0f - 2.0f * (float)q.x * (float)q.x - 2.0f * (float)q.y * (float)q.y,
                    0.0f
                },
                {
                    0.0f,
                    0.0f,
                    0.0f,
                    1.0f
                }
            }
        };
        */
    }

    static mat Rotation(float yaw, float pitch, float roll) {
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

    float Determinant() {
        return m[0][0] * (m[1][1] * m[2][2] * m[3][3]
            + m[1][2] * m[2][3] * m[3][1]
            + m[1][3] * m[2][1] * m[3][2]
            - m[1][3] * m[2][2] * m[3][1]
            - m[1][2] * m[2][1] * m[3][3]
            - m[1][1] * m[2][3] * m[3][2])
            - m[1][0] * (m[0][1] * m[2][2] * m[3][3]
                + m[0][2] * m[2][3] * m[3][1]
                + m[0][3] * m[2][1] * m[3][2]
                - m[0][3] * m[2][2] * m[3][1]
                - m[0][2] * m[2][1] * m[3][3]
                - m[0][1] * m[2][3] * m[3][2])
            + m[2][0] * (m[0][1] * m[1][2] * m[3][3]
                + m[0][2] * m[1][3] * m[3][1]
                + m[0][3] * m[1][1] * m[3][2]
                - m[0][3] * m[1][2] * m[3][1]
                - m[0][2] * m[1][1] * m[3][3]
                - m[0][1] * m[1][3] * m[3][2])
            - m[3][0] * (m[0][1] * m[1][2] * m[2][3]
                + m[0][2] * m[1][3] * m[2][1]
                + m[0][3] * m[1][1] * m[2][2]
                - m[0][3] * m[1][2] * m[2][1]
                - m[0][2] * m[1][1] * m[2][3]
                - m[0][1] * m[1][3] * m[2][2]);
    }

    mat inverse() {
        float a2323 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
        float a1323 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
        float a1223 = m[2][1] * m[3][2] - m[2][2] * m[3][1];
        float a0323 = m[2][0] * m[3][3] - m[2][3] * m[3][0];
        float a0223 = m[2][0] * m[3][2] - m[2][2] * m[3][0];
        float a0123 = m[2][0] * m[3][1] - m[2][1] * m[3][0];
        float a2313 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
        float a1313 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
        float a1213 = m[1][1] * m[3][2] - m[1][2] * m[3][1];
        float a2312 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
        float a1312 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
        float a1212 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
        float a0313 = m[1][0] * m[3][3] - m[1][3] * m[3][0];
        float a0213 = m[1][0] * m[3][2] - m[1][2] * m[3][0];
        float a0312 = m[1][0] * m[2][3] - m[1][3] * m[2][0];
        float a0212 = m[1][0] * m[2][2] - m[1][2] * m[2][0];
        float a0113 = m[1][0] * m[3][1] - m[1][1] * m[3][0];
        float a0112 = m[1][0] * m[2][1] - m[1][1] * m[2][0];

        float det = Determinant();
        det = 1 / det;

        return {
            {
            {
                det * (m[1][1] * a2323 - m[1][2] * a1323 + m[1][3] * a1223),
                det * -(m[0][1] * a2323 - m[0][2] * a1323 + m[0][3] * a1223),
                det * (m[0][1] * a2313 - m[0][2] * a1313 + m[0][3] * a1213),
                det * -(m[0][1] * a2312 - m[0][2] * a1312 + m[0][3] * a1212),
            },
            {
                det * -(m[1][0] * a2323 - m[1][2] * a0323 + m[1][3] * a0223),
                det * (m[0][0] * a2323 - m[0][2] * a0323 + m[0][3] * a0223),
                det * -(m[0][0] * a2313 - m[0][2] * a0313 + m[0][3] * a0213),
                det * (m[0][0] * a2312 - m[0][2] * a0312 + m[0][3] * a0212),
            },
            {
                det * (m[1][0] * a1323 - m[1][1] * a0323 + m[1][3] * a0123),
                det * -(m[0][0] * a1323 - m[0][1] * a0323 + m[0][3] * a0123),
                det * (m[0][0] * a1313 - m[0][1] * a0313 + m[0][3] * a0113),
                det * -(m[0][0] * a1312 - m[0][1] * a0312 + m[0][3] * a0112),
            },
            {
                det * -(m[1][0] * a1223 - m[1][1] * a0223 + m[1][2] * a0123),
                det * (m[0][0] * a1223 - m[0][1] * a0223 + m[0][2] * a0123),
                det * -(m[0][0] * a1213 - m[0][1] * a0213 + m[0][2] * a0113),
                det * (m[0][0] * a1212 - m[0][1] * a0212 + m[0][2] * a0112),
            }
            }
        };
    }
} mat_t;
inline mat PMathMatRotationX(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat m = mat::Identity();
    m.m[1][1] = c;  m.m[1][2] = s;
    m.m[2][1] = -s; m.m[2][2] = c;
    return m;
}

inline mat PMathMatRotationY(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat m = mat::Identity();
    m.m[0][0] = c;  m.m[0][2] = -s;
    m.m[2][0] = s;  m.m[2][2] = c;
    return m;
}

inline mat PMathMatRotationZ(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat m = mat::Identity();
    m.m[0][0] = c;  m.m[0][1] = s;
    m.m[1][0] = -s; m.m[1][1] = c;
    return m;
}

// Roll (Z) → Pitch (X) → Yaw (Y)
inline mat PMathMatRollPitchYaw(float roll, float pitch, float yaw) {
    mat Rz = PMathMatRotationZ(roll);
    mat Rx = PMathMatRotationX(pitch);
    mat Ry = PMathMatRotationY(yaw);

    // Row-vector multiplication order: v * (Rz * Rx * Ry)
    return Rz * Rx * Ry;
}
inline mat PMathMatRotationQuaternion(const Quaternion& q) {
    mat m = mat::Identity();

    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    m.m[0][0] = 1.0f - 2.0f * (yy + zz);
    m.m[0][1] = 2.0f * (xy + wz);
    m.m[0][2] = 2.0f * (xz - wy);
    m.m[0][3] = 0.0f;

    m.m[1][0] = 2.0f * (xy - wz);
    m.m[1][1] = 1.0f - 2.0f * (xx + zz);
    m.m[1][2] = 2.0f * (yz + wx);
    m.m[1][3] = 0.0f;

    m.m[2][0] = 2.0f * (xz + wy);
    m.m[2][1] = 2.0f * (yz - wx);
    m.m[2][2] = 1.0f - 2.0f * (xx + yy);
    m.m[2][3] = 0.0f;

    m.m[3][0] = 0.0f;
    m.m[3][1] = 0.0f;
    m.m[3][2] = 0.0f;
    m.m[3][3] = 1.0f;

    return m;
}

inline mat PMathMatScaling(float fScaleX, float fScaleY, float fScaleZ) {
    mat m = mat::Identity();

    m.m[0][0] = fScaleX;
    m.m[1][1] = fScaleY;
    m.m[2][2] = fScaleZ;

    return m;
}

inline mat PMathMatPerspectiveLH(float aspect, float fov, float nearZ, float farZ) {
    float yScale = 1.0f / tanf(fov * 0.5f);
    float xScale = yScale / aspect;

    return {
        {
            {xScale, 0, 0, 0},
            {0, yScale, 0, 0},
            {0, 0, farZ / (farZ - nearZ), 1},
            {0, 0, (-nearZ * farZ) / (farZ - nearZ), 0}
        },
    };
}

inline mat PMathMatPerspectiveRH(float fov, float aspect, float nearZ, float farZ) {
    float yScale = 1.0f / tanf(fov / 2.0f);
    float xScale = yScale / aspect;

    mat m {};

    // scale
    m.m[0][0] = xScale;
    m.m[1][1] = yScale;

    // z mapping
    m.m[2][2] = farZ / (nearZ - farZ);         // note the negative denominator
    m.m[2][3] = -1.0f;                         // opposite sign vs LH
    m.m[3][2] = (nearZ * farZ) / (nearZ - farZ);  // also flips compared to LH
    m.m[3][3] = 0.0f;

    return m;
}

inline mat PMathMatAffineTransform(const Vector3& scaling, const Vector3& rotationOrigin, const Quaternion& rotationQuat, const Vector3& translation) {
    // 1. Scale matrix
    mat S = PMathMatScaling(scaling.x, scaling.y, scaling.z);

    // 2. Rotation matrix
    mat R = PMathMatRotationQuaternion(rotationQuat);

    // 3. Translation to/from rotation origin
    mat T_negOrigin = mat::Translation(-rotationOrigin.x, -rotationOrigin.y, -rotationOrigin.z);
    mat T_origin     = mat::Translation(rotationOrigin.x, rotationOrigin.y, rotationOrigin.z);

    // 4. Final translation
    mat T = mat::Translation(translation.x, translation.y, translation.z);

    // Combine: T * T_origin * R * S * T_negOrigin
    return T * T_origin * R * S * T_negOrigin;
}