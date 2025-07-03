#ifndef PCP_H
#define PCP_H

#include <pcprep/aabb.h>
#include <pcprep/canvas.h>
#include <pcprep/point_cloud.h>
#include <pcprep/utils.h>
#include <stdint.h>
#include <string.h>

#define MAX_PROCESS                256
#define MAX_STATUS                 256
#define MAX_MVP_COUNT              0xffff

#define PCP_PROC_SAMPLE            0x00
#define PCP_PROC_VOXEL             0x01
#define PCP_PROC_REMOVE_DUPLICATES 0x02

#define PCP_STAT_AABB              0x00
#define PCP_STAT_PIXEL_PER_TILE    0x01
#define PCP_STAT_SAVE_VIEWPORT     0x02
#endif
#define PCP_STAT_SCREEN_AREA_ESTIMATION 0x03

#define PCP_PLAN_NONE_NONE              0x00
#define PCP_PLAN_NONE_TILE              0x01
#define PCP_PLAN_NONE_MERGE             0x02
#define PCP_PLAN_TILE_NONE              0x10
#define PCP_PLAN_TILE_MERGE             0x12
#define PCP_PLAN_MERGE_NONE             0x20
#define PCP_PLAN_MERGE_TILE             0x21

#define SIZE_PATH                       0xffff
#define SET_OPT_PROCESS                 0x0001
#define SET_OPT_STATUS                  0x0002

typedef struct func_t
{
  unsigned char func_id;
  char        **func_arg;
  size_t        func_arg_size;
} func_t;

typedef struct func_info
{
  const char   *name;
  unsigned char func; // Maps to func_id
  int           min_args;
  int           max_args;
} func_info_t;

const func_info_t processes_g[] = {
    {           "sample",            PCP_PROC_SAMPLE, 2, 2},
    {            "voxel",             PCP_PROC_VOXEL, 1, 1},
    {"remove-duplicates", PCP_PROC_REMOVE_DUPLICATES, 0, 0},
    {               NULL,                          0, 0, 0}
};

const func_info_t statuses_g[] = {
    {                  "aabb",                   PCP_STAT_AABB, 3, 3},
#ifdef PCP_STAT_SAVE_VIEWPORT
    {         "save-viewport",          PCP_STAT_SAVE_VIEWPORT, 3, 3},
#endif
    {        "pixel-per-tile",         PCP_STAT_PIXEL_PER_TILE, 3, 3},
    {"screen-area-estimation", PCP_STAT_SCREEN_AREA_ESTIMATION, 2, 2},
    {                    NULL,                               0, 0, 0}
};

struct arguments
{
  uint32_t      flags;
  char         *input;
  char         *output;
  int           binary;
  int           tiled_input;
  unsigned char plan;
  size_t        procs_size;
  size_t        stats_size;
  func_t        procs[MAX_PROCESS];
  func_t        stats[MAX_STATUS];
  struct
  {
    uint8_t nx, ny, nz;
  } tile;
};

int arguments_free(struct arguments *arg)
{
  for (size_t i = 0; i < arg->procs_size; i++)
  {
    for (size_t j = 0; j < arg->procs[i].func_arg_size; j++)
    {
      free(arg->procs[i].func_arg[j]);
    }
    free(arg->procs[i].func_arg);
  }
  for (size_t i = 0; i < arg->stats_size; i++)
  {
    for (size_t j = 0; j < arg->stats[i].func_arg_size; j++)
    {
      free(arg->stats[i].func_arg[j]);
    }
    free(arg->stats[i].func_arg);
  }
  return 1;
}

const func_info_t *find_func(const char        *name,
                             const func_info_t *funcs)
{
  for (const func_info_t *p = funcs; p->name != NULL; p++)
    if (strcmp(p->name, name) == 0)
      return p;
  return NULL;
}

typedef unsigned int (*func_f)(pcp_point_cloud_t *pc,
                               void              *arg,
                               int                pc_id);
func_f       pcp_process_legs_g[MAX_PROCESS]   = {NULL};
void        *pcp_process_params_g[MAX_PROCESS] = {NULL};
unsigned int pcp_process_legs_count_g          = 0;
func_f       pcp_status_legs_g[MAX_STATUS]     = {NULL};
void        *pcp_status_params_g[MAX_STATUS]   = {NULL};
unsigned int pcp_status_legs_count_g           = 0;

unsigned int pcp_process_legs_append(func_f func, void *param)
{
  pcp_process_legs_g[pcp_process_legs_count_g]   = func;
  pcp_process_params_g[pcp_process_legs_count_g] = param;
  pcp_process_legs_count_g++;
  return 1;
}
unsigned int pcp_status_legs_append(func_f func, void *param)
{
  pcp_status_legs_g[pcp_status_legs_count_g]   = func;
  pcp_status_params_g[pcp_status_legs_count_g] = param;
  pcp_status_legs_count_g++;
  return 1;
}

unsigned int pcp_process_legs_run(pcp_point_cloud_t *pc, int pc_id)
{
  for (int i = 0; i < pcp_process_legs_count_g; i++)
  {
    if (pcp_process_legs_g[i] != NULL)
    {
      pcp_process_legs_g[i](pc, pcp_process_params_g[i], pc_id);
    }
  }
}
unsigned int pcp_status_legs_run(pcp_point_cloud_t *pc, int pc_id)
{
  for (int i = 0; i < pcp_status_legs_count_g; i++)
  {
    if (pcp_status_legs_g[i] != NULL)
    {
      pcp_status_legs_g[i](pc, pcp_status_params_g[i], pc_id);
    }
  }
}

unsigned int pcp_free_param(void)
{
  for (int i = 0; i < pcp_process_legs_count_g; i++)
    free(pcp_process_params_g[i]);
  for (int i = 0; i < pcp_status_legs_count_g; i++)
    free(pcp_status_params_g[i]);
}

typedef struct pcp_sample_p_arg_t
{
  float         ratio;
  unsigned char strategy;
} pcp_sample_p_arg_t;

unsigned int pcp_sample_p(pcp_point_cloud_t *pc, void *arg, int pc_id)
{
  pcp_sample_p_arg_t *param = (pcp_sample_p_arg_t *)arg;

  pcp_point_cloud_t   out   = {0};
  pcp_point_cloud_init(&out);
  pc->sample(pc, param->ratio, param->strategy, &out);
  pcp_point_cloud_free(pc);
  *pc = out;
  return 1;
}

unsigned int pcp_voxel_p(pcp_point_cloud_t *pc, void *arg, int pc_id)
{
  float             step_size = *(float *)arg;

  pcp_point_cloud_t out       = {0};
  pcp_point_cloud_init(&out);
  pc->voxelize(pc, step_size, &out);
  pcp_point_cloud_free(pc);
  *pc = out;
  return 1;
}

unsigned int
pcp_remove_dupplicates_p(pcp_point_cloud_t *pc, void *arg, int pc_id)
{
  pcp_point_cloud_t out = {0};
  pcp_point_cloud_init(&out);
  pc->remove_dupplicates(pc, &out);
  pcp_point_cloud_free(pc);
  *pc = out;
  return 1;
}

typedef struct pcp_aabb_s_arg_t
{
  int  output;
  int  binary;
  char output_path[SIZE_PATH];
} pcp_aabb_s_arg_t;

unsigned int pcp_aabb_s(pcp_point_cloud_t *pc, void *arg, int pc_id)
{
  pcp_aabb_s_arg_t *param = (pcp_aabb_s_arg_t *)arg;

  pcp_vec3f_t       min   = {0};
  pcp_vec3f_t       max   = {0};
  pc->get_min(pc, &min);
  pc->get_max(pc, &max);
  if (param->output == 0 || param->output == 2)
  {
    printf("Min: %f %f %f\n", min.x, min.y, min.z);
    printf("Max: %f %f %f\n", max.x, max.y, max.z);
    if (param->output == 0)
    {
      return 1;
    }
  }
  pcp_aabb_t aabb                 = {0};
  pcp_mesh_t mesh                 = {0};
  char       tile_path[SIZE_PATH] = {0};

  pcp_mesh_init(&mesh);
  pcp_aabb_init(&aabb);

  aabb.min = min;
  aabb.max = max;

  aabb.to_mesh(&aabb, &mesh);

  snprintf(tile_path, SIZE_PATH, param->output_path, pc_id);
  mesh.write(&mesh, tile_path, param->binary);
  pcp_mesh_free(&mesh);
  pcp_aabb_free(&aabb);
  return 1;
}

#ifdef PCP_STAT_SAVE_VIEWPORT
typedef struct pcp_save_viewport_s_arg_t
{
  char         outpath[SIZE_PATH];
  float        mvps[MAX_MVP_COUNT][4][4]; // 4x4 matrixes
  int          mvp_count;
  size_t       width;
  size_t       height;
  pcp_vec3uc_t background;
} pcp_save_viewport_s_arg_t;
unsigned int
pcp_save_viewport_s(pcp_point_cloud_t *pc, void *arg, int pc_id)
{
  pcp_save_viewport_s_arg_t *param = (pcp_save_viewport_s_arg_t *)arg;

  pcp_canvas_t               cv    = {0};

  pcp_canvas_init(&cv,
                  param->width,
                  param->height,
#ifdef HAVE_GPU
                  NULL,
                  NULL,
#endif
                  param->background);

  for (int v = 0; v < param->mvp_count; v++)
  {
    cv.clear(&cv);

    cv.draw_points(
        &cv, &param->mvps[v][0][0], pc->pos, pc->rgb, pc->size);
    unsigned char **row_pointers = NULL;
    row_pointers = malloc(param->height * sizeof(unsigned char *));

    for (int i = 0; i < param->height; i++)
      row_pointers[i] = malloc(param->width * 3);

    flip_image(row_pointers, cv.pixels, param->width, param->height);

    char tile_path[SIZE_PATH];
    snprintf(tile_path, SIZE_PATH, param->outpath, v, pc_id);
    save_viewport(
        row_pointers, param->width, param->height, tile_path);
    for (int i = 0; i < param->height; i++)
      free(row_pointers[i]);
    free(row_pointers);
  }
  pcp_canvas_free(&cv);
  return 1;
}
#endif

typedef struct pcp_pixel_per_tile_s_arg_t
{
  char   outpath[SIZE_PATH];
  float  mvps[MAX_MVP_COUNT][4][4]; // 4x4 matrix
  int    mvp_count;
  size_t width;
  size_t height;
  int    nx;
  int    ny;
  int    nz;
} pcp_pixel_per_tile_s_arg_t;

unsigned int
pcp_pixel_per_tile_s(pcp_point_cloud_t *pc, void *arg, int pc_id)
{
  pcp_pixel_per_tile_s_arg_t *param =
      (pcp_pixel_per_tile_s_arg_t *)arg;
  int   num_tile = param->nx * param->ny * param->nz;
  int **pixel_count =
      (int **)malloc(sizeof(int *) * param->mvp_count);
  for (int v = 0; v < param->mvp_count; v++)
    pixel_count[v] = (int *)calloc(sizeof(int), num_tile);

  for (int v = 0; v < param->mvp_count; v++)
  {
    pc->get_pixel_per_tile(pc,
                           param->nx,
                           param->ny,
                           param->nz,
                           param->width,
                           param->height,
                           &param->mvps[v][0][0],
                           &pixel_count[v][0]);
  }
  json_write_tiles_pixel(param->outpath,
                         num_tile,
                         param->mvp_count,
                         pixel_count,
                         param->width * param->height);
  for (int v = 0; v < param->mvp_count; v++)
    free(pixel_count[v]);
  free(pixel_count);
  return 1;
}

typedef struct pcp_screen_area_estimation_s_arg_t
{
  float  mvps[MAX_MVP_COUNT][4][4];
  int    mvp_count;
  size_t width;
  size_t height;
  char   outpath[SIZE_PATH];
} pcp_screen_area_estimation_s_arg_t;

unsigned int pcp_screen_area_estimation_s(pcp_point_cloud_t *pc,
                                          void              *arg,
                                          int                pc_id)
{
  float                              *screen_ratio = NULL;
  pcp_vec3f_t                         min          = {0};
  pcp_vec3f_t                         max          = {0};
  pcp_aabb_t                          aabb         = {0};
  pcp_mesh_t                          aabb_m       = {0};

  pcp_screen_area_estimation_s_arg_t *param =
      (pcp_screen_area_estimation_s_arg_t *)arg;

  pc->get_min(pc, &min);
  pc->get_max(pc, &max);

  pcp_aabb_init(&aabb);
  pcp_mesh_init(&aabb_m);

  aabb.min = min;
  aabb.max = max;

  aabb.to_mesh(&aabb, &aabb_m);

  screen_ratio = (float *)malloc(sizeof(float) * param->mvp_count);
  for (int v = 0; v < param->mvp_count; v++)
  {
    aabb_m.get_screen_ratio(
        &aabb_m, &param->mvps[v][0][0], &screen_ratio[v]);
  }
  char pc_path[SIZE_PATH];
  snprintf(pc_path, SIZE_PATH, param->outpath, pc_id);
  json_write_screen_area_estimation(pc_path,
                                    param->mvp_count,
                                    param->width,
                                    param->height,
                                    screen_ratio);
  free(screen_ratio);
  pcp_mesh_free(&aabb_m);
  pcp_aabb_free(&aabb);
}
