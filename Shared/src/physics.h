#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <math.h>

namespace PMG {
  bool comp(float a, float b);

	typedef struct vec2_t {
		float x;
		float y;

		vec2_t operator+(const vec2_t& other) const {
			return { x + other.x, y + other.y };
		}

		vec2_t operator-(const vec2_t& other) const {
			return { x - other.x, y - other.y };
		}

    bool operator==(const vec2_t& other) const {
      return comp(x, other.x)
        && comp(y, other.y);
    }
	} vec2_t;


	typedef struct vec3_t {
		float x;
		float y;
		float z;

		vec3_t operator+(const vec3_t& other) const {
			return { x + other.x, y + other.y, z + other.z };
		}

		vec3_t operator-(const vec3_t& other) const {
			return { x - other.x, y - other.y, z - other.z };
		}

    float operator*(const vec3_t& other) const {
      return x * other.x + y * other.y + z * other.z;
    }

    bool operator==(const vec3_t& other) const {
      return comp(x, other.x)
        && comp(y, other.y)
        && comp(z, other.z);
    }

    float length() const {
      return sqrt(x*x + y*y + z*z);
    }

    vec3_t normalize() const {
      float l = length();
      
      return { x / l, y / l, z / l };
    }
	} vec3_t;

	typedef struct vec4_t {
		float x;
		float y;
		float z;
		float w;

		vec4_t operator+(const vec4_t& other) const {
			return { x + other.x, y + other.y, z + other.z, w + other.w };
		}

		vec4_t operator-(const vec4_t& other) const {
			return { x - other.x, y - other.y, z - other.z, w - other.w };
		}

    bool operator==(const vec4_t& other) const {
      return comp(x, other.x)
        && comp(y, other.y)
        && comp(z, other.z)
        && comp(w, other.w);
    }
	} vec4_t;

	typedef struct mat_t {
		// m[row][column]
		float m[4][4] = { 0 };

    bool operator==(const mat_t& other) const {
      for(int x = 0; x < 4; x++) {
        for(int y = 0; y < 4; y++) {
          if(!comp(m[x][y], other.m[x][y])) {
            return false;
          }
        }
      }

      return true;
    }

    vec4_t operator*(const vec4_t& vec) const {
      return {
        m[0][0]*vec.x+m[0][1]*vec.y+m[0][2]*vec.z+m[0][3]*vec.w,
        m[1][0]*vec.x+m[1][1]*vec.y+m[1][2]*vec.z+m[1][3]*vec.w,
        m[2][0]*vec.x+m[2][1]*vec.y+m[2][2]*vec.z+m[2][3]*vec.w,
        m[3][0]*vec.x+m[3][1]*vec.y+m[3][2]*vec.z+m[3][3]*vec.w
      };
    }

    mat_t operator*(const mat_t& other) const {
      return {
        {
          {
            m[0][0]*other.m[0][0]
              +m[0][1]*other.m[1][0]
              +m[0][2]*other.m[2][0]
              +m[0][3]*other.m[3][0],
            m[0][0]*other.m[0][1]
              +m[0][1]*other.m[1][1]
              +m[0][2]*other.m[2][1]
              +m[0][3]*other.m[3][1],
            m[0][0]*other.m[0][2]
              +m[0][1]*other.m[1][2]
              +m[0][2]*other.m[2][2]
              +m[0][3]*other.m[3][2],
            m[0][0]*other.m[0][3]
              +m[0][1]*other.m[1][3]
              +m[0][2]*other.m[2][3]
              +m[0][3]*other.m[3][3],
          },
          {
            m[1][0]*other.m[0][0]
              +m[1][1]*other.m[1][0]
              +m[1][2]*other.m[2][0]
              +m[1][3]*other.m[3][0],
            m[1][0]*other.m[0][1]
              +m[1][1]*other.m[1][1]
              +m[1][2]*other.m[2][1]
              +m[1][3]*other.m[3][1],
            m[1][0]*other.m[0][2]
              +m[1][1]*other.m[1][2]
              +m[1][2]*other.m[2][2]
              +m[1][3]*other.m[3][2],
            m[1][0]*other.m[0][3]
              +m[1][1]*other.m[1][3]
              +m[1][2]*other.m[2][3]
              +m[1][3]*other.m[3][3],
          },
          {
            m[2][0]*other.m[0][0]
              +m[2][1]*other.m[1][0]
              +m[2][2]*other.m[2][0]
              +m[2][3]*other.m[3][0],
            m[2][0]*other.m[0][1]
              +m[2][1]*other.m[1][1]
              +m[2][2]*other.m[2][1]
              +m[2][3]*other.m[3][1],
            m[2][0]*other.m[0][2]
              +m[2][1]*other.m[1][2]
              +m[2][2]*other.m[2][2]
              +m[2][3]*other.m[3][2],
            m[2][0]*other.m[0][3]
              +m[2][1]*other.m[1][3]
              +m[2][2]*other.m[2][3]
              +m[2][3]*other.m[3][3],
          },
          {
            m[3][0]*other.m[0][0]
              +m[3][1]*other.m[1][0]
              +m[3][2]*other.m[2][0]
              +m[3][3]*other.m[3][0],
            m[3][0]*other.m[0][1]
              +m[3][1]*other.m[1][1]
              +m[3][2]*other.m[2][1]
              +m[3][3]*other.m[3][1],
            m[3][0]*other.m[0][2]
              +m[3][1]*other.m[1][2]
              +m[3][2]*other.m[2][2]
              +m[3][3]*other.m[3][2],
            m[3][0]*other.m[0][3]
              +m[3][1]*other.m[1][3]
              +m[3][2]*other.m[2][3]
              +m[3][3]*other.m[3][3],
          },
        }
      };
    }

		static mat_t Identity() {
			return {
				{
					{1, 0, 0, 0},
					{0, 1, 0, 0},
					{0, 0, 1, 0},
					{0, 0, 0, 1}
				},
			};
		}

    mat_t Transpose() {
      return {
        {
          { m[0][0], m[1][0], m[2][0], m[3][0] },
          { m[0][1], m[1][1], m[2][1], m[3][1] },
          { m[0][2], m[1][2], m[2][2], m[3][2] },
          { m[0][3], m[1][3], m[2][3], m[3][3] },
        }
      };
    }

		static mat_t Perspective(float aspect, float fov, float nearZ, float farZ) {
			return {
				{
					{1 / (aspect * std::tanf(fov / 2)), 0, 0, 0},
					{0, 1 / (std::tanf(fov / 2)), 0, 0},
					{0, 0, (-nearZ - farZ) / (nearZ - farZ), (2 * farZ * nearZ) / (nearZ - farZ)},
					{0, 0, 1, 0}
				},
			};
		}

    static mat_t Translation(float x, float y, float z) {
      return {
        {
          {1, 0, 0, x},
          {0, 1, 0, y},
          {0, 0, 1, z},
          {0, 0, 0, 1}
        }
      };
    }

    static mat_t Rotation(float yaw, float pitch, float roll) {
        const double halfC = M_PI / 180;
        
        yaw *= halfC;
        pitch *= halfC;
        roll *= halfC;
        
      return {
        {
          {
            cosf(yaw)*cosf(pitch),
            cosf(yaw)*sinf(pitch)*sinf(roll)-sinf(yaw)*cosf(roll),
            cosf(yaw)*sinf(pitch)*cosf(roll)+sinf(yaw)*sinf(roll),
            0
          },
          {
            sinf(yaw)*cosf(pitch),
            sinf(yaw)*sinf(pitch)*sinf(roll)+cosf(yaw)*cosf(roll),
            sinf(yaw)*sinf(pitch)*cosf(roll)-cosf(yaw)*sinf(roll),
            0
          },
          {
            -sinf(pitch),
            cosf(pitch)*sinf(roll),
            cosf(pitch)*cosf(roll),
            0
          },
          {0, 0, 0, 1}
        }
      };
    }

		float Determinant() {
			return m[0][0]*(m[1][1]*m[2][2]*m[3][3]
                        +m[1][2]*m[2][3]*m[3][1]
                        +m[1][3]*m[2][1]*m[3][2]
                        -m[1][3]*m[2][2]*m[3][1]
                        -m[1][2]*m[2][1]*m[3][3]
                        -m[1][1]*m[2][3]*m[3][2])
              -m[1][0]*(m[0][1]*m[2][2]*m[3][3]
                        +m[0][2]*m[2][3]*m[3][1]
                        +m[0][3]*m[2][1]*m[3][2]
                        -m[0][3]*m[2][2]*m[3][1]
                        -m[0][2]*m[2][1]*m[3][3]
                        -m[0][1]*m[2][3]*m[3][2])
              +m[2][0]*(m[0][1]*m[1][2]*m[3][3]
                        +m[0][2]*m[1][3]*m[3][1]
                        +m[0][3]*m[1][1]*m[3][2]
                        -m[0][3]*m[1][2]*m[3][1]
                        -m[0][2]*m[1][1]*m[3][3]
                        -m[0][1]*m[1][3]*m[3][2])
              -m[3][0]*(m[0][1]*m[1][2]*m[2][3]
                        +m[0][2]*m[1][3]*m[2][1]
                        +m[0][3]*m[1][1]*m[2][2]
                        -m[0][3]*m[1][2]*m[2][1]
                        -m[0][2]*m[1][1]*m[2][3]
                        -m[0][1]*m[1][3]*m[2][2]);
		}

		mat_t inverse() {
      float a2323 = m[2][2]*m[3][3]-m[2][3]*m[3][2];
      float a1323 = m[2][1]*m[3][3]-m[2][3]*m[3][1];
      float a1223 = m[2][1]*m[3][2]-m[2][2]*m[3][1];
      float a0323 = m[2][0]*m[3][3]-m[2][3]*m[3][0];
      float a0223 = m[2][0]*m[3][2]-m[2][2]*m[3][0];
      float a0123 = m[2][0]*m[3][1]-m[2][1]*m[3][0];
      float a2313 = m[1][2]*m[3][3]-m[1][3]*m[3][2];
      float a1313 = m[1][1]*m[3][3]-m[1][3]*m[3][1];
      float a1213 = m[1][1]*m[3][2]-m[1][2]*m[3][1];
      float a2312 = m[1][2]*m[2][3]-m[1][3]*m[2][2];
      float a1312 = m[1][1]*m[2][3]-m[1][3]*m[2][1];
      float a1212 = m[1][1]*m[2][2]-m[1][2]*m[2][1];
      float a0313 = m[1][0]*m[3][3]-m[1][3]*m[3][0];
      float a0213 = m[1][0]*m[3][2]-m[1][2]*m[3][0];
      float a0312 = m[1][0]*m[2][3]-m[1][3]*m[2][0];
      float a0212 = m[1][0]*m[2][2]-m[1][2]*m[2][0];
      float a0113 = m[1][0]*m[3][1]-m[1][1]*m[3][0];
      float a0112 = m[1][0]*m[2][1]-m[1][1]*m[2][0];

      float det = Determinant();
      det = 1 / det;

      return {
        {
          {
            det *   ( m[1][1] * a2323 - m[1][2] * a1323 + m[1][3] * a1223 ),
            det * - ( m[0][1] * a2323 - m[0][2] * a1323 + m[0][3] * a1223 ),
            det *   ( m[0][1] * a2313 - m[0][2] * a1313 + m[0][3] * a1213 ),
            det * - ( m[0][1] * a2312 - m[0][2] * a1312 + m[0][3] * a1212 ),
          },
          {
            det * - ( m[1][0] * a2323 - m[1][2] * a0323 + m[1][3] * a0223 ),
            det *   ( m[0][0] * a2323 - m[0][2] * a0323 + m[0][3] * a0223 ),
            det * - ( m[0][0] * a2313 - m[0][2] * a0313 + m[0][3] * a0213 ),
            det *   ( m[0][0] * a2312 - m[0][2] * a0312 + m[0][3] * a0212 ),
          },
          {
            det *   ( m[1][0] * a1323 - m[1][1] * a0323 + m[1][3] * a0123 ),
            det * - ( m[0][0] * a1323 - m[0][1] * a0323 + m[0][3] * a0123 ),
            det *   ( m[0][0] * a1313 - m[0][1] * a0313 + m[0][3] * a0113 ),
            det * - ( m[0][0] * a1312 - m[0][1] * a0312 + m[0][3] * a0112 ),
          },
          {
            det * - ( m[1][0] * a1223 - m[1][1] * a0223 + m[1][2] * a0123 ),
            det *   ( m[0][0] * a1223 - m[0][1] * a0223 + m[0][2] * a0123 ),
            det * - ( m[0][0] * a1213 - m[0][1] * a0213 + m[0][2] * a0113 ),
            det *   ( m[0][0] * a1212 - m[0][1] * a0212 + m[0][2] * a0112 ),
          }
        }
      };
    }
  };
}