#ifndef MESH_H
#define MESH_H

#ifdef __cplusplus
extern "C"
{

#endif

#include "pcprep/defs.h"
#include <stdint.h>
#include <stdlib.h>

  struct pcp_mesh_t
  {
    float    *pos;
    uint32_t  num_verts;
    uint32_t *indices;
    uint32_t  num_indices;
    pcp_ret_t (*alloc)(pcp_mesh_t *self,
                       uint32_t    num_verts,
                       uint32_t    num_indices);
    pcp_ret_t (*load)(pcp_mesh_t *self, const char *filename);
    pcp_ret_t (*write)(pcp_mesh_t *self,
                       const char *filename,
                       int         binary);
    pcp_ret_t (*get_screen_ratio)(pcp_mesh_t *self,
                                  float      *mvp,
                                  float      *screen_ratio);
  };

  PCPREP_EXPORT
  pcp_ret_t pcp_mesh_init(pcp_mesh_t *self);
  PCPREP_EXPORT
  pcp_ret_t pcp_mesh_free(pcp_mesh_t *self);

  pcp_ret_t pcp_mesh_alloc(pcp_mesh_t *self,
                           uint32_t    num_verts,
                           uint32_t    num_indices);
  pcp_ret_t pcp_mesh_load(pcp_mesh_t *self, const char *filename);

  pcp_ret_t
  pcp_mesh_write(pcp_mesh_t *self, const char *filename, int binary);

  pcp_ret_t pcp_mesh_get_screen_ratio(pcp_mesh_t *self,
                                      float      *mvp,
                                      float      *screen_ratio);

#ifdef __cplusplus
}
#endif
#endif
