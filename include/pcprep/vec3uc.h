#ifndef VEC3UC_H
#define VEC3UC_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "pcprep/utils.h"
#include <math.h>
#include <stdint.h>
  typedef struct pcp_vec3uc_t
  {
    uint8_t x;
    uint8_t y;
    uint8_t z;
  } pcp_vec3uc_t;

  static inline pcp_vec3uc_t
  pcp_vec3uc_set(uint8_t x, uint8_t y, uint8_t z)
  {
    pcp_vec3uc_t v = {x, y, z};
    return v;
  }

  static inline pcp_vec3uc_t pcp_vec3uc_mul_scalar(pcp_vec3uc_t v,
                                                   float scalar)
  {
    return (pcp_vec3uc_t){v.x * scalar, v.y * scalar, v.z * scalar};
  }

  static inline float pcp_vec3uc_magnitude(pcp_vec3uc_t v)
  {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  }

  static inline pcp_vec3uc_t pcp_vec3uc_normalize(pcp_vec3uc_t v)
  {
    float mag = pcp_vec3uc_magnitude(v);
    if (mag > 0.0f)
    {
      return pcp_vec3uc_mul_scalar(v, 1.0f / mag);
    }
    return pcp_vec3uc_set(0.0f, 0.0f, 0.0f);
  }

  static inline pcp_vec3uc_t pcp_vec3uc_add(pcp_vec3uc_t a,
                                            pcp_vec3uc_t b)
  {
    return (pcp_vec3uc_t){a.x + b.x, a.y + b.y, a.z + b.z};
  }

  static inline pcp_vec3uc_t pcp_vec3uc_sub(pcp_vec3uc_t a,
                                            pcp_vec3uc_t b)
  {
    return (pcp_vec3uc_t){a.x - b.x, a.y - b.y, a.z - b.z};
  }

  static inline pcp_vec3uc_t pcp_vec3uc_mul(pcp_vec3uc_t a,
                                            pcp_vec3uc_t b)
  {
    return (pcp_vec3uc_t){a.x * b.x, a.y * b.y, a.z * b.z};
  }

  static inline float pcp_vec3uc_dot(pcp_vec3uc_t a, pcp_vec3uc_t b)
  {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }

  static inline pcp_vec3uc_t pcp_vec3uc_cross(pcp_vec3uc_t a,
                                              pcp_vec3uc_t b)
  {
    return (pcp_vec3uc_t){a.y * b.z - a.z * b.y,
                          a.z * b.x - a.x * b.z,
                          a.x * b.y - a.y * b.x};
  }

  static inline float pcp_vec3uc_angle_between(pcp_vec3uc_t a,
                                               pcp_vec3uc_t b)
  {
    float dot  = pcp_vec3uc_dot(a, b);
    float magA = pcp_vec3uc_magnitude(a);
    float magB = pcp_vec3uc_magnitude(b);
    return acos(dot / (magA * magB));
  }

  static inline int pcp_vec3uc_eq(pcp_vec3uc_t a, pcp_vec3uc_t b)
  {
    return a.x == b.x && a.y == b.y && a.z == b.z;
  }

  static inline int pcp_vec3uc_g(pcp_vec3uc_t a, pcp_vec3uc_t b)
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

  static inline int pcp_vec3uc_l(pcp_vec3uc_t a, pcp_vec3uc_t b)
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

  static inline int pcp_vec3uc_geq(pcp_vec3uc_t a, pcp_vec3uc_t b)
  {
    return pcp_vec3uc_g(a, b) || pcp_vec3uc_eq(a, b);
  }

  static inline int pcp_vec3uc_leq(pcp_vec3uc_t a, pcp_vec3uc_t b)
  {
    return pcp_vec3uc_l(a, b) || pcp_vec3uc_eq(a, b);
  }

  static inline pcp_vec3uc_t pcp_vec3uc_reflect(pcp_vec3uc_t v,
                                                pcp_vec3uc_t n)
  {
    float dot = pcp_vec3uc_dot(v, n);
    return pcp_vec3uc_sub(v, pcp_vec3uc_mul_scalar(n, 2 * dot));
  }

  static inline pcp_vec3uc_t pcp_vec3uc_quantize(pcp_vec3uc_t v,
                                                 float        q)
  {
    return (pcp_vec3uc_t){
        quantize(v.x, q), quantize(v.y, q), quantize(v.z, q)};
  }

#ifdef __cplusplus
}
#endif

#endif
