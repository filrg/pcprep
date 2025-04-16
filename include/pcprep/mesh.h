#ifndef MESH_H
#define MESH_H

#ifdef __cplusplus
extern "C"
{

#endif

#include "pcprep/pcprep_export.h"
#include <stdint.h>
#include <stdlib.h>

  typedef struct mesh_t
  {
    float    *pos;
    uint32_t  num_verts;
    uint32_t *indices;
    uint32_t  num_indices;
  } mesh_t;

  PCPREP_EXPORT
  int mesh_init(mesh_t  *mesh,
                uint32_t num_verts,
                uint32_t num_indices);
  PCPREP_EXPORT
  int mesh_free(mesh_t *mesh);
  PCPREP_EXPORT
  int mesh_load(mesh_t *mesh, const char *filename);
  PCPREP_EXPORT
  int mesh_write(mesh_t mesh, const char *filename, int binary);
  PCPREP_EXPORT
  int mesh_screen_ratio(mesh_t mesh, float *mvp, float *screen_ratio);

#ifdef __cplusplus
}
#endif
#endif
