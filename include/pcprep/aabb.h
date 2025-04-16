#ifndef AABB_H
#define AABB_H

#ifdef __cplusplus
extern "C"
{

#endif
#include "pcprep/mesh.h"
#include "pcprep/pcprep_export.h"
#include "pcprep/vec3f.h"

  /**
   * @brief represent an Axis-Aligned Bounding Box (AABB).
   *
   * Defined by its min and max corners.
   * @see aabb.h
   */
  typedef struct aabb_t
  {
    vec3f_t min; ///< Minimum (x, y, z)
    vec3f_t max; ///< Maximum (x, y, z)
  } aabb_t;

  /**
   * @brief Constructs a triangle mesh from an axis-aligned bounding
   * box.
   *
   * Creates a new mesh based on the input AABB.
   *
   * @param aabb  The input axis-aligned bounding box.
   * @param mesh  Output pointer to the resulting mesh.
   * @return 0 on success, non-zero on failure.
   */
  PCPREP_EXPORT
  int aabb_to_mesh(aabb_t aabb, mesh_t *mesh);

#ifdef __cplusplus
}
#endif
#endif
