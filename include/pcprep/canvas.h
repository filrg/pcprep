#ifndef CANVAS_H
#define CANVAS_H

#include "pcprep/core.h"
#include "pcprep/pcprep_export.h"
#include "pcprep/vec3uc.h"

typedef struct canvas_t canvas_t;

/**
 * @brief holds the data of a virtual screen to be drawn.
 *
 *
 */
struct canvas_t
{
  size_t         width;
  size_t         height;
  vec3uc_t       bg_col;
  unsigned char *pixels;
  float        **min_z_value;

#ifdef HAVE_GPU
  char        *vert_shader;
  char        *frag_shader;
  unsigned int program_id;
  unsigned int frame_buffer;
  unsigned int rendered_texture;
  unsigned int depth_buffer;
  unsigned int vertex_array_id;
  unsigned int matrix_id;
#endif

  void (*draw_points)(
      canvas_t *, float *, float *, unsigned char *, size_t);
  void (*clear)(canvas_t *);
};

PCPREP_EXPORT
int canvas_init(canvas_t *cv,
                size_t    width,
                size_t    height,
#ifdef HAVE_GPU
                char *vert_shader,
                char *frag_shader,
#endif
                vec3uc_t bg_col);

PCPREP_EXPORT
int canvas_free(canvas_t *cv);

#ifdef HAVE_GPU
PCPREP_EXPORT
void canvas_draw_points_gpu(canvas_t      *cv,
                            float         *mvp,
                            float         *pos,
                            unsigned char *rgb,
                            size_t         count);
#endif
PCPREP_EXPORT
void canvas_draw_points_cpu(canvas_t      *cv,
                            float         *mvp,
                            float         *pos,
                            unsigned char *rgb,
                            size_t         count);

PCPREP_EXPORT
void canvas_clear(canvas_t *cv);

#endif
