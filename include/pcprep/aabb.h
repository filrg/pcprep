#ifndef AABB_H
#define AABB_H

#ifdef __cplusplus
extern "C"
{

#endif
#include "pcprep/defs.h"
#include "pcprep/mesh.h"
#include "pcprep/vec3f.h"

  /**
   * @brief represent an Axis-Aligned Bounding Box (AABB).
   *
   * Defined by its min and max corners.
   * @see aabb.h
   */
  struct pcp_aabb_t
  {
    pcp_vec3f_t min; ///< Minimum (x, y, z)
    pcp_vec3f_t max; ///< Maximum (x, y, z)
    pcp_ret_t (*to_mesh)(pcp_aabb_t *self, pcp_mesh_t *mesh);
  };

  PCPREP_EXPORT
  pcp_ret_t pcp_aabb_init(pcp_aabb_t *self);

  PCPREP_EXPORT
  pcp_ret_t pcp_aabb_free(pcp_aabb_t *self);

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
  pcp_ret_t pcp_aabb_to_mesh(pcp_aabb_t *self, pcp_mesh_t *mesh);

#ifdef __cplusplus
}
#endif
#endif
