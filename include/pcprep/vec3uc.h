#ifndef VEC3UC_H
#define VEC3UC_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "pcprep/core.h"
#include <math.h>
#include <stdint.h>
  typedef struct vec3uc_t
  {
    uint8_t x;
    uint8_t y;
    uint8_t z;
  } vec3uc_t;

  static inline vec3uc_t vec3uc_set(uint8_t x, uint8_t y, uint8_t z)
  {
    vec3uc_t v = {x, y, z};
    return v;
  }

  static inline vec3uc_t vec3uc_mul_scalar(vec3uc_t v, float scalar)
  {
    return (vec3uc_t){v.x * scalar, v.y * scalar, v.z * scalar};
  }

  static inline float vec3uc_magnitude(vec3uc_t v)
  {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  }

  static inline vec3uc_t vec3uc_normalize(vec3uc_t v)
  {
    float mag = vec3uc_magnitude(v);
    if (mag > 0.0f)
    {
      return vec3uc_mul_scalar(v, 1.0f / mag);
    }
    return vec3uc_set(0.0f, 0.0f, 0.0f);
  }

  static inline vec3uc_t vec3uc_add(vec3uc_t a, vec3uc_t b)
  {
    return (vec3uc_t){a.x + b.x, a.y + b.y, a.z + b.z};
  }

  static inline vec3uc_t vec3uc_sub(vec3uc_t a, vec3uc_t b)
  {
    return (vec3uc_t){a.x - b.x, a.y - b.y, a.z - b.z};
  }

  static inline vec3uc_t vec3uc_mul(vec3uc_t a, vec3uc_t b)
  {
    return (vec3uc_t){a.x * b.x, a.y * b.y, a.z * b.z};
  }

  static inline float vec3uc_dot(vec3uc_t a, vec3uc_t b)
  {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }

  static inline vec3uc_t vec3uc_cross(vec3uc_t a, vec3uc_t b)
  {
    return (vec3uc_t){a.y * b.z - a.z * b.y,
                      a.z * b.x - a.x * b.z,
                      a.x * b.y - a.y * b.x};
  }

  static inline float vec3uc_angle_between(vec3uc_t a, vec3uc_t b)
  {
    float dot  = vec3uc_dot(a, b);
    float magA = vec3uc_magnitude(a);
    float magB = vec3uc_magnitude(b);
    return acos(dot / (magA * magB));
  }

  static inline int vec3uc_eq(vec3uc_t a, vec3uc_t b)
  {
    return a.x == b.x && a.y == b.y && a.z == b.z;
  }

  static inline int vec3uc_g(vec3uc_t a, vec3uc_t b)
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

  static inline int vec3uc_l(vec3uc_t a, vec3uc_t b)
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

  static inline int vec3uc_geq(vec3uc_t a, vec3uc_t b)
  {
    return vec3uc_g(a, b) || vec3uc_eq(a, b);
  }

  static inline int vec3uc_leq(vec3uc_t a, vec3uc_t b)
  {
    return vec3uc_l(a, b) || vec3uc_eq(a, b);
  }

  static inline vec3uc_t vec3uc_reflect(vec3uc_t v, vec3uc_t n)
  {
    float dot = vec3uc_dot(v, n);
    return vec3uc_sub(v, vec3uc_mul_scalar(n, 2 * dot));
  }

  static inline vec3uc_t vec3uc_quantize(vec3uc_t v, float q)
  {
    return (vec3uc_t){
        quantize(v.x, q), quantize(v.y, q), quantize(v.z, q)};
  }

#ifdef __cplusplus
}
#endif

#endif
