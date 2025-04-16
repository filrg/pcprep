#ifndef VEC3U_H
#define VEC3U_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "pcprep/core.h"
#include <math.h>
#include <stdint.h>
  typedef struct vec3u_t
  {
    uint32_t x;
    uint32_t y;
    uint32_t z;
  } vec3u_t;

  static inline vec3u_t vec3u_set(uint32_t x, uint32_t y, uint32_t z)
  {
    vec3u_t v = {x, y, z};
    return v;
  }

  static inline vec3u_t vec3u_mul_scalar(vec3u_t v, float scalar)
  {
    return (vec3u_t){v.x * scalar, v.y * scalar, v.z * scalar};
  }

  static inline float vec3u_magnitude(vec3u_t v)
  {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  }

  static inline vec3u_t vec3u_normalize(vec3u_t v)
  {
    float mag = vec3u_magnitude(v);
    if (mag > 0.0f)
    {
      return vec3u_mul_scalar(v, 1.0f / mag);
    }
    return vec3u_set(0.0f, 0.0f, 0.0f);
  }

  static inline vec3u_t vec3u_add(vec3u_t a, vec3u_t b)
  {
    return (vec3u_t){a.x + b.x, a.y + b.y, a.z + b.z};
  }

  static inline vec3u_t vec3u_sub(vec3u_t a, vec3u_t b)
  {
    return (vec3u_t){a.x - b.x, a.y - b.y, a.z - b.z};
  }

  static inline vec3u_t vec3u_mul(vec3u_t a, vec3u_t b)
  {
    return (vec3u_t){a.x * b.x, a.y * b.y, a.z * b.z};
  }

  static inline float vec3u_dot(vec3u_t a, vec3u_t b)
  {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }

  static inline vec3u_t vec3u_cross(vec3u_t a, vec3u_t b)
  {
    return (vec3u_t){a.y * b.z - a.z * b.y,
                     a.z * b.x - a.x * b.z,
                     a.x * b.y - a.y * b.x};
  }

  static inline float vec3u_angle_between(vec3u_t a, vec3u_t b)
  {
    float dot  = vec3u_dot(a, b);
    float magA = vec3u_magnitude(a);
    float magB = vec3u_magnitude(b);
    return acos(dot / (magA * magB));
  }

  static inline int vec3u_eq(vec3u_t a, vec3u_t b)
  {
    return a.x == b.x && a.y == b.y && a.z == b.z;
  }

  static inline int vec3u_g(vec3u_t a, vec3u_t b)
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

  static inline int vec3u_l(vec3u_t a, vec3u_t b)
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

  static inline int vec3u_geq(vec3u_t a, vec3u_t b)
  {
    return vec3u_g(a, b) || vec3u_eq(a, b);
  }

  static inline int vec3u_leq(vec3u_t a, vec3u_t b)
  {
    return vec3u_l(a, b) || vec3u_eq(a, b);
  }

  static inline vec3u_t vec3u_reflect(vec3u_t v, vec3u_t n)
  {
    float dot = vec3u_dot(v, n);
    return vec3u_sub(v, vec3u_mul_scalar(n, 2 * dot));
  }

  static inline vec3u_t vec3u_quantize(vec3u_t v, float q)
  {
    return (vec3u_t){
        quantize(v.x, q), quantize(v.y, q), quantize(v.z, q)};
  }

#ifdef __cplusplus
}
#endif

#endif
