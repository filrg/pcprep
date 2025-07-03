#ifndef UTILS_H
#define UTILS_H

#define PCP_SAMPLE_RULE_UNIFORM 0x00
#define PCP_FLOAT_ERROR         1e-6f

#if defined(WITH_GLFW) && defined(WITH_GL) && defined(WITH_GLEW)
#define HAVE_GPU
#endif

#include <pcprep/defs.h>

#include <png.h>
#include <stdlib.h>

typedef struct
{
  float x, y;
} vec2f_t;

// Function to get the current time in milliseconds
PCPREP_EXPORT
long long get_current_time_ms(void);

PCPREP_EXPORT
int float_error(float a, float b, float e);

PCPREP_EXPORT
int float_equal(float a, float b);

PCPREP_EXPORT
int sample_union(int *input,
                 int  input_size,
                 int *output,
                 int  output_size);

PCPREP_EXPORT
float quantize(float x, float q);
PCPREP_EXPORT
char *read_file(const char *filename);

// `mvps` should be parse as an array of float mvp[4][4]
PCPREP_EXPORT
int json_parse_cam_matrix(char   *filepath,
                          float  *mvps,
                          size_t  view_count,
                          size_t *width,
                          size_t *height);

PCPREP_EXPORT
void json_write_to_file(const char *filename, void *json);

PCPREP_EXPORT
int json_write_tiles_pixel(char  *outpath,
                           int    num_tile,
                           int    num_view,
                           int  **pixel_count_per_tile,
                           size_t total_pixel);

PCPREP_EXPORT
int json_write_screen_area_estimation(char  *outpath,
                                      int    num_view,
                                      size_t width,
                                      size_t height,
                                      float *screen_ratio);

PCPREP_EXPORT
float clipped_triangle_area(vec2f_t p1, vec2f_t p2, vec2f_t p3);

PCPREP_EXPORT
int flip_image(unsigned char **row_pointers,
               unsigned char  *pixels,
               size_t          width,
               size_t          height);

PCPREP_EXPORT
int save_viewport(unsigned char **row_pointers,
                  int             width,
                  int             height,
                  const char     *filename);

// TODO: put this in utils.h
PCPREP_EXPORT
int get_tile_id(pcp_vec3f_t n,
                pcp_vec3f_t min,
                pcp_vec3f_t max,
                pcp_vec3f_t v);
// `pcs` should be passed as an array of pcp_point_cloud_t
// `output` should be passed as a reference to a pcp_point_cloud_t
// TODO: put this in utils.h
PCPREP_EXPORT
int point_cloud_merge(pcp_point_cloud_t *pcs,
                      size_t             pc_count,
                      pcp_point_cloud_t *out);

#endif
