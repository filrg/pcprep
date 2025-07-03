#ifndef VEC3U_H
#define VEC3U_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "pcprep/utils.h"
#include <math.h>
#include <stdint.h>
  typedef struct pcp_vec3u_t
  {
    uint32_t x;
    uint32_t y;
    uint32_t z;
  } pcp_vec3u_t;

  static inline pcp_vec3u_t
  pcp_vec3u_set(uint32_t x, uint32_t y, uint32_t z)
  {
    pcp_vec3u_t v = {x, y, z};
    return v;
  }

  static inline pcp_vec3u_t pcp_vec3u_mul_scalar(pcp_vec3u_t v,
                                                 float       scalar)
  {
    return (pcp_vec3u_t){v.x * scalar, v.y * scalar, v.z * scalar};
  }

  static inline float pcp_vec3u_magnitude(pcp_vec3u_t v)
  {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  }

  static inline pcp_vec3u_t pcp_vec3u_normalize(pcp_vec3u_t v)
  {
    float mag = pcp_vec3u_magnitude(v);
    if (mag > 0.0f)
    {
      return pcp_vec3u_mul_scalar(v, 1.0f / mag);
    }
    return pcp_vec3u_set(0.0f, 0.0f, 0.0f);
  }

  static inline pcp_vec3u_t pcp_vec3u_add(pcp_vec3u_t a,
                                          pcp_vec3u_t b)
  {
    return (pcp_vec3u_t){a.x + b.x, a.y + b.y, a.z + b.z};
  }

  static inline pcp_vec3u_t pcp_vec3u_sub(pcp_vec3u_t a,
                                          pcp_vec3u_t b)
  {
    return (pcp_vec3u_t){a.x - b.x, a.y - b.y, a.z - b.z};
  }

  static inline pcp_vec3u_t pcp_vec3u_mul(pcp_vec3u_t a,
                                          pcp_vec3u_t b)
  {
    return (pcp_vec3u_t){a.x * b.x, a.y * b.y, a.z * b.z};
  }

  static inline float pcp_vec3u_dot(pcp_vec3u_t a, pcp_vec3u_t b)
  {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }

  static inline pcp_vec3u_t pcp_vec3u_cross(pcp_vec3u_t a,
                                            pcp_vec3u_t b)
  {
    return (pcp_vec3u_t){a.y * b.z - a.z * b.y,
                         a.z * b.x - a.x * b.z,
                         a.x * b.y - a.y * b.x};
  }

  static inline float pcp_vec3u_angle_between(pcp_vec3u_t a,
                                              pcp_vec3u_t b)
  {
    float dot  = pcp_vec3u_dot(a, b);
    float magA = pcp_vec3u_magnitude(a);
    float magB = pcp_vec3u_magnitude(b);
    return acos(dot / (magA * magB));
  }

  static inline int pcp_vec3u_eq(pcp_vec3u_t a, pcp_vec3u_t b)
  {
    return a.x == b.x && a.y == b.y && a.z == b.z;
  }

  static inline int pcp_vec3u_g(pcp_vec3u_t a, pcp_vec3u_t b)
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

  static inline int pcp_vec3u_l(pcp_vec3u_t a, pcp_vec3u_t b)
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

  static inline int pcp_vec3u_geq(pcp_vec3u_t a, pcp_vec3u_t b)
  {
    return pcp_vec3u_g(a, b) || pcp_vec3u_eq(a, b);
  }

  static inline int pcp_vec3u_leq(pcp_vec3u_t a, pcp_vec3u_t b)
  {
    return pcp_vec3u_l(a, b) || pcp_vec3u_eq(a, b);
  }

  static inline pcp_vec3u_t pcp_vec3u_reflect(pcp_vec3u_t v,
                                              pcp_vec3u_t n)
  {
    float dot = pcp_vec3u_dot(v, n);
    return pcp_vec3u_sub(v, pcp_vec3u_mul_scalar(n, 2 * dot));
  }

  static inline pcp_vec3u_t pcp_vec3u_quantize(pcp_vec3u_t v, float q)
  {
    return (pcp_vec3u_t){
        quantize(v.x, q), quantize(v.y, q), quantize(v.z, q)};
  }

#ifdef __cplusplus
}
#endif

#endif
