#ifndef VEC3F_H
#define VEC3F_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "pcprep/core.h"
#include <math.h>

  typedef struct vec3f_t
  {
    float x;
    float y;
    float z;
  } vec3f_t;
  static inline vec3f_t vec3f_set(float x, float y, float z)
  {
    vec3f_t v = {x, y, z};
    return v;
  }
  static inline vec3f_t vec3f_int(vec3f_t v)
  {
    return (vec3f_t){
        (float)(int)v.x, (float)(int)v.y, (float)(int)v.z};
  }
  static inline vec3f_t vec3f_mul_scalar(vec3f_t v, float scalar)
  {
    return (vec3f_t){v.x * scalar, v.y * scalar, v.z * scalar};
  }
  static inline vec3f_t vec3f_inverse(vec3f_t v)
  {
    return (vec3f_t){1.0f / v.x, 1.0f / v.y, 1.0f / v.z};
  }
  static inline float vec3f_magnitude(vec3f_t v)
  {
    return (float)sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  }
  static inline vec3f_t vec3f_normalize(vec3f_t v)
  {
    float mag = vec3f_magnitude(v);
    if (mag > 0.0f)
    {
      return vec3f_mul_scalar(v, 1.0f / mag);
    }
    return vec3f_set(0.0f, 0.0f, 0.0f);
  }
  static inline vec3f_t vec3f_add(vec3f_t a, vec3f_t b)
  {
    return (vec3f_t){a.x + b.x, a.y + b.y, a.z + b.z};
  }
  static inline vec3f_t vec3f_sub(vec3f_t a, vec3f_t b)
  {
    return (vec3f_t){a.x - b.x, a.y - b.y, a.z - b.z};
  }
  static inline vec3f_t vec3f_mul(vec3f_t a, vec3f_t b)
  {
    return (vec3f_t){a.x * b.x, a.y * b.y, a.z * b.z};
  }
  static inline float vec3f_dot(vec3f_t a, vec3f_t b)
  {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }
  static inline vec3f_t vec3f_cross(vec3f_t a, vec3f_t b)
  {
    return (vec3f_t){a.y * b.z - a.z * b.y,
                     a.z * b.x - a.x * b.z,
                     a.x * b.y - a.y * b.x};
  }
  static inline float vec3f_angle_between(vec3f_t a, vec3f_t b)
  {
    float dot  = vec3f_dot(a, b);
    float magA = vec3f_magnitude(a);
    float magB = vec3f_magnitude(b);
    return (float)acos(dot / (magA * magB));
  }
  static inline int vec3f_eq(vec3f_t a, vec3f_t b)
  {
    return float_equal(a.x, b.x) && float_equal(a.y, b.y) &&
           float_equal(a.z, b.z);
  }

  static inline int vec3f_g(vec3f_t a, vec3f_t b)
  {
    if (a.x < b.x)
      return 0;
    if (a.x > b.x)
      return 1;
    if (a.y < b.y)
      return 0;
    if (a.y > b.y)
      return 1;
    return a.z > b.z;
  }
  static inline int vec3f_l(vec3f_t a, vec3f_t b)
  {
    if (a.x < b.x)
      return 1;
    if (a.x > b.x)
      return 0;
    if (a.y < b.y)
      return 1;
    if (a.y > b.y)
      return 0;
    return a.z < b.z;
  }
  static inline int vec3f_geq(vec3f_t a, vec3f_t b)
  {
    return vec3f_g(a, b) || vec3f_eq(a, b);
  }
  static inline int vec3f_leq(vec3f_t a, vec3f_t b)
  {
    return vec3f_l(a, b) || vec3f_eq(a, b);
  }
  static inline vec3f_t vec3f_reflect(vec3f_t v, vec3f_t n)
  {
    float dot = vec3f_dot(v, n);
    return vec3f_sub(v, vec3f_mul_scalar(n, 2 * dot));
  }

  static inline vec3f_t vec3f_quantize(vec3f_t v, float q)
  {
    return (vec3f_t){
        quantize(v.x, q), quantize(v.y, q), quantize(v.z, q)};
  }
  static inline vec3f_t vec3f_mvp_mul(vec3f_t v, float *mvp)
  {
    float temp_x =
        mvp[0] * v.x + mvp[4] * v.y + mvp[8] * v.z + mvp[12];
    float temp_y =
        mvp[1] * v.x + mvp[5] * v.y + mvp[9] * v.z + mvp[13];
    float temp_z =
        mvp[2] * v.x + mvp[6] * v.y + mvp[10] * v.z + mvp[14];
    float temp_w =
        mvp[3] * v.x + mvp[7] * v.y + mvp[11] * v.z + mvp[15];
    temp_x /= temp_w;
    temp_y /= temp_w;
    temp_z /= temp_w;
    return (vec3f_t){temp_x, temp_y, temp_z};
  }
  static inline vec3f_t
  vec3f_rotate(vec3f_t v, float angle, vec3f_t axis)
  {
    float c           = cosf(angle);
    float s           = sinf(angle);
    float one_minus_c = 1.0f - c;

    float len =
        sqrtf(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    axis.x /= len;
    axis.y /= len;
    axis.z /= len;

    float ux = axis.x, uy = axis.y, uz = axis.z;

    float R[3][3] = {
        {     ux * ux * one_minus_c + c,
         ux * uy * one_minus_c - uz * s,
         ux * uz * one_minus_c + uy * s},
        {uy * ux * one_minus_c + uz * s,
         uy * uy * one_minus_c + c,
         uy * uz * one_minus_c - ux * s},
        {uz * ux * one_minus_c - uy * s,
         uz * uy * one_minus_c + ux * s,
         uz * uz * one_minus_c + c     }
    };

    vec3f_t result;
    result.x = R[0][0] * v.x + R[0][1] * v.y + R[0][2] * v.z;
    result.y = R[1][0] * v.x + R[1][1] * v.y + R[1][2] * v.z;
    result.z = R[2][0] * v.x + R[2][1] * v.y + R[2][2] * v.z;

    return result;
  }

#ifdef __cplusplus
}
#endif

#endif
