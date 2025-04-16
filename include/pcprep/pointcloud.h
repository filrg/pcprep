#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "pcprep/pcprep_export.h"
#include "pcprep/vec3f.h"
#include <stdint.h>
#include <stdlib.h>

  typedef struct pointcloud_t
  {
    float   *pos;
    uint8_t *rgb;
    size_t   size; // number of points, *pos have size*3 elements
  } pointcloud_t;
  // this need reference
  PCPREP_EXPORT
  int pointcloud_init(pointcloud_t *pc, size_t size);
  // this need reference
  PCPREP_EXPORT
  int pointcloud_free(pointcloud_t *pc);
  // this need reference
  PCPREP_EXPORT
  int pointcloud_load(pointcloud_t *pc, const char *filename);
  // this doesn't need reference
  PCPREP_EXPORT
  int pointcloud_write(pointcloud_t pc,
                       const char  *filename,
                       int          binary);
  // this doesn't need reference
  PCPREP_EXPORT
  int pointcloud_min(pointcloud_t pc, vec3f_t *min);
  // this doesn't need reference
  PCPREP_EXPORT
  int pointcloud_max(pointcloud_t pc, vec3f_t *max);
  PCPREP_EXPORT
  int get_tile_id(vec3f_t n, vec3f_t min, vec3f_t max, vec3f_t v);
  // this doesn't need reference
  // `tiles` should be passed as a reference of a pointcloud_t*
  PCPREP_EXPORT
  int pointcloud_tile(pointcloud_t   pc,
                      int            n_x,
                      int            n_y,
                      int            n_z,
                      pointcloud_t **tiles);
  // `pcs` should be passed as an array of pointcloud_t
  // `output` should be passed as a reference to a pointcloud_t
  PCPREP_EXPORT
  int pointcloud_merge(pointcloud_t *pcs,
                       size_t        pc_count,
                       pointcloud_t *out);
  // `output` should be passed as a reference to a pointcloud_t
  PCPREP_EXPORT
  int         pointcloud_sample(pointcloud_t  pc,
                                float         ratio,
                                unsigned char strategy,
                                pointcloud_t *out);

  static void pointcloud_element_merge(pointcloud_t pc,
                                       int          left,
                                       int          mid,
                                       int          right);
  static void
  pointcloud_element_merge_sort(pointcloud_t pc, int left, int right);
  // `output` should be passed as a reference to a pointcloud_t
  PCPREP_EXPORT
  int pointcloud_remove_dupplicates(pointcloud_t  pc,
                                    pointcloud_t *out);
  // `output` should be passed as a reference to a pointcloud_t
  PCPREP_EXPORT
  int pointcloud_voxel(pointcloud_t  pc,
                       float         voxel_size,
                       pointcloud_t *out);
  PCPREP_EXPORT
  int pointcloud_count_pixel_per_tile(pointcloud_t pc,
                                      int          nx,
                                      int          ny,
                                      int          nz,
                                      int          width,
                                      int          height,
                                      float       *mvp,
                                      int         *pixel_count);
#ifdef __cplusplus
}
#endif
#endif
