#ifndef POINT_CLOUD_H
#define POINT_CLOUD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <pcprep/defs.h>
#include <pcprep/vec3f.h>
#include <stdint.h>
#include <stdlib.h>

  struct pcp_point_cloud_t
  {
    float   *pos;
    uint8_t *rgb;
    size_t   size; // number of points, *pos have size*3 elements
    pcp_ret_t (*alloc)(pcp_point_cloud_t *self, size_t size);
    pcp_ret_t (*load)(pcp_point_cloud_t *self, const char *filename);
    pcp_ret_t (*write)(pcp_point_cloud_t *self,
                       const char        *filename,
                       int                binary);
    pcp_ret_t (*get_min)(pcp_point_cloud_t *self, pcp_vec3f_t *min);
    pcp_ret_t (*get_max)(pcp_point_cloud_t *self, pcp_vec3f_t *max);
    pcp_ret_t (*tile)(pcp_point_cloud_t  *self,
                      int                 n_x,
                      int                 n_y,
                      int                 n_z,
                      pcp_point_cloud_t **tiles);
    pcp_ret_t (*sample)(pcp_point_cloud_t *self,
                        float              ratio,
                        unsigned char      strategy,
                        pcp_point_cloud_t *out);
    pcp_ret_t (*remove_dupplicates)(pcp_point_cloud_t *self,
                                    pcp_point_cloud_t *out);
    pcp_ret_t (*voxelize)(pcp_point_cloud_t *self,
                          float              voxel_size,
                          pcp_point_cloud_t *out);
    pcp_ret_t (*get_pixel_per_tile)(pcp_point_cloud_t *self,
                                    int                nx,
                                    int                ny,
                                    int                nz,
                                    int                width,
                                    int                height,
                                    float             *mvp,
                                    int               *pixel_count);
  };

  PCPREP_EXPORT
  pcp_ret_t pcp_point_cloud_init(pcp_point_cloud_t *self);
  PCPREP_EXPORT
  pcp_ret_t pcp_point_cloud_free(pcp_point_cloud_t *pc);

  pcp_ret_t pcp_point_cloud_alloc(pcp_point_cloud_t *pc, size_t size);

  pcp_ret_t pcp_point_cloud_load(pcp_point_cloud_t *pc,
                                 const char        *filename);
  pcp_ret_t pcp_point_cloud_write(pcp_point_cloud_t *pc,
                                  const char        *filename,
                                  int                binary);
  pcp_ret_t pcp_point_cloud_get_min(pcp_point_cloud_t *pc,
                                    pcp_vec3f_t       *min);
  pcp_ret_t pcp_point_cloud_get_max(pcp_point_cloud_t *pc,
                                    pcp_vec3f_t       *max);

  pcp_ret_t pcp_point_cloud_tile(pcp_point_cloud_t  *pc,
                                 int                 n_x,
                                 int                 n_y,
                                 int                 n_z,
                                 pcp_point_cloud_t **tiles);

  pcp_ret_t pcp_point_cloud_sample(pcp_point_cloud_t *pc,
                                   float              ratio,
                                   unsigned char      strategy,
                                   pcp_point_cloud_t *out);

  pcp_ret_t
            pcp_point_cloud_remove_dupplicates(pcp_point_cloud_t *pc,
                                               pcp_point_cloud_t *out);
  pcp_ret_t pcp_point_cloud_voxelize(pcp_point_cloud_t *pc,
                                     float              voxel_size,
                                     pcp_point_cloud_t *out);
  pcp_ret_t pcp_point_cloud_get_pixel_per_tile(pcp_point_cloud_t *pc,
                                               int                nx,
                                               int                ny,
                                               int                nz,
                                               int    width,
                                               int    height,
                                               float *mvp,
                                               int   *pixel_count);
#ifdef __cplusplus
}
#endif
#endif
