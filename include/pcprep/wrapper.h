#ifndef WRAPPER_H
#define WRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "pcprep/pcprep_export.h"
#include <stdlib.h>
  // Declare the wrapper function
  PCPREP_EXPORT
  int ply_count_vertex(const char *filename);
  PCPREP_EXPORT
  int ply_count_face(const char *filename);
  PCPREP_EXPORT
  int ply_pcp_point_cloud_loader(const char    *filename,
                                 float         *pos,
                                 unsigned char *rgb);
  PCPREP_EXPORT
  int ply_mesh_loader(const char *filename, float *pos, int *indices);

#ifdef __cplusplus
}
#endif
#endif // WRAPPER_H
