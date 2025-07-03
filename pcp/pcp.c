#include "pcp.h"
#include <argp.h>
#include <assert.h>
#include <pcprep/point_cloud.h>
#include <pcprep/utils.h>
#include <pcprep/wrapper.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int pcp_prepare(struct arguments *arg)
{
  pcp_point_cloud_t *in_pcs           = NULL;
  pcp_point_cloud_t *proc_pcs         = NULL;
  pcp_point_cloud_t *out_pcs          = NULL;
  char              *input_path       = NULL;
  char              *output_path      = NULL;
  char              *output_tile_path = NULL;
  char              *input_tile_path  = NULL;
  int                max_path_size    = 0;
  int                binary           = 0;
  int                proc_count       = 0;
  int                in_count         = 0;
  int                out_count        = 0;
  long long          read_time        = 0;
  long long          pre_proc_time    = 0;
  long long          proc_time        = 0;
  long long          post_proc_time   = 0;
  long long          write_time       = 0;
  long long          curr_time        = 0;

  max_path_size                       = SIZE_PATH;
  input_path                          = arg->input;
  output_path                         = arg->output;
  binary                              = arg->binary;
  input_tile_path  = (char *)malloc(max_path_size * sizeof(char));
  output_tile_path = (char *)malloc(max_path_size * sizeof(char));
  in_count         = arg->tiled_input;

  curr_time        = get_current_time_ms();

  in_pcs           = (pcp_point_cloud_t *)calloc(in_count,
                                       sizeof(pcp_point_cloud_t));
  for (int t = 0; t < in_count; t++)
  {
    pcp_point_cloud_init(&in_pcs[t]);
    snprintf(input_tile_path, max_path_size, input_path, t);
    in_pcs[t].load(&in_pcs[t], input_tile_path);
  }
  read_time = get_current_time_ms() - curr_time;
  curr_time = get_current_time_ms();
  // this only run if in_count = 1
  if (in_count == 1 && arg->plan & PCP_PLAN_TILE_NONE)
  {
    int nx     = 0;
    int ny     = 0;
    int nz     = 0;
    nx         = arg->tile.nx;
    ny         = arg->tile.ny;
    nz         = arg->tile.nz;
    proc_count = nx * ny * nz;
    in_pcs[0].tile(&in_pcs[0], nx, ny, nz, &proc_pcs);
    for (int i = 0; i < in_count; i++)
      pcp_point_cloud_free(&in_pcs[i]);
    free(in_pcs);
  }
  else if (in_count > 1 && arg->plan & PCP_PLAN_MERGE_NONE)
  {
    proc_pcs = (pcp_point_cloud_t *)malloc(sizeof(pcp_point_cloud_t));
    pcp_point_cloud_init(proc_pcs);
    proc_count = point_cloud_merge(in_pcs, in_count, &proc_pcs[0]);
    for (int i = 0; i < in_count; i++)
      pcp_point_cloud_free(&in_pcs[i]);
    free(in_pcs);
  }
  else
  {
    proc_pcs   = in_pcs;
    proc_count = in_count;
  }

  pre_proc_time = get_current_time_ms() - curr_time;

  if (arg->flags & SET_OPT_PROCESS)
  {
    // Handle the SET_OPT_PROCESS flag
    for (int i = 0; i < arg->procs_size; i++)
    {
      func_t *curr = &(arg->procs[i]);
      switch (curr->func_id)
      {
      case PCP_PROC_SAMPLE:
      {
        pcp_sample_p_arg_t *param =
            (pcp_sample_p_arg_t *)malloc(sizeof(pcp_sample_p_arg_t));
        *param = (pcp_sample_p_arg_t){1.0f, 0};
        assert(curr->func_arg_size >= 1);
        assert(curr->func_arg[0] != NULL);
        float percent = atof(curr->func_arg[0]);
        if (percent > 0.0f && percent < 1.0f)
          param->ratio = percent;
        param->strategy = atoi(curr->func_arg[1]);
        pcp_process_legs_append(pcp_sample_p, param);
        break;
      }
      case PCP_PROC_VOXEL:
      {
        float *param = (float *)malloc(sizeof(float));
        *param       = atof(curr->func_arg[0]);
        pcp_process_legs_append(pcp_voxel_p, param);
        break;
      }
      case PCP_PROC_REMOVE_DUPLICATES:
      {
        pcp_process_legs_append(pcp_remove_dupplicates_p, NULL);
        break;
      }
      default:
      {
        break;
      }
      }
    }
  }
  if (arg->flags & SET_OPT_STATUS)
  {
    // Handle the SET_OPT_STATUS flag
    for (int i = 0; i < arg->stats_size; i++)
    {
      func_t *curr = &(arg->stats[i]);
      switch (curr->func_id)
      {
      case PCP_STAT_AABB:
      {
        pcp_aabb_s_arg_t *param =
            (pcp_aabb_s_arg_t *)malloc(sizeof(pcp_aabb_s_arg_t));
        *param        = (pcp_aabb_s_arg_t){.binary = 1, .output = 0};
        param->output = atoi(curr->func_arg[0]);
        param->binary = atoi(curr->func_arg[1]);
        strcpy(param->output_path, curr->func_arg[2]);
        pcp_status_legs_append(pcp_aabb_s, param);
        break;
      }
      case PCP_STAT_PIXEL_PER_TILE:
      {
        pcp_pixel_per_tile_s_arg_t *param =
            (pcp_pixel_per_tile_s_arg_t *)malloc(
                sizeof(pcp_pixel_per_tile_s_arg_t));
        *param = (pcp_pixel_per_tile_s_arg_t){.height    = 0,
                                              .width     = 0,
                                              .mvp_count = 0,
                                              .nx        = 1,
                                              .ny        = 1,
                                              .nz        = 1};
        param->mvp_count =
            json_parse_cam_matrix(curr->func_arg[0],
                                  &param->mvps[0][0][0],
                                  MAX_MVP_COUNT,
                                  &param->width,
                                  &param->height);

        sscanf(curr->func_arg[1],
               "%d,%d,%d",
               &param->nx,
               &param->ny,
               &param->nz);
        strcpy(param->outpath, curr->func_arg[2]);
        pcp_status_legs_append(pcp_pixel_per_tile_s, param);
        break;
      }
      case PCP_STAT_SCREEN_AREA_ESTIMATION:
      {
        pcp_screen_area_estimation_s_arg_t *param =
            (pcp_screen_area_estimation_s_arg_t *)malloc(
                sizeof(pcp_screen_area_estimation_s_arg_t));
        *param = (pcp_screen_area_estimation_s_arg_t){
            .height    = 0,
            .width     = 0,
            .mvp_count = 0,
        };
        float mvps[MAX_MVP_COUNT][4][4];
        param->mvp_count =
            json_parse_cam_matrix(curr->func_arg[0],
                                  &param->mvps[0][0][0],
                                  MAX_MVP_COUNT,
                                  &param->width,
                                  &param->height);

        strcpy(param->outpath, curr->func_arg[1]);
        pcp_status_legs_append(pcp_screen_area_estimation_s, param);
        break;
      }
#ifdef PCP_STAT_SAVE_VIEWPORT
      case PCP_STAT_SAVE_VIEWPORT:
      {
        pcp_save_viewport_s_arg_t *param =
            (pcp_save_viewport_s_arg_t *)malloc(
                sizeof(pcp_save_viewport_s_arg_t));
        *param = (pcp_save_viewport_s_arg_t){
            .height     = 0,
            .width      = 0,
            .mvp_count  = 0,
            .background = (pcp_vec3uc_t){255, 255, 255}
        };
        param->mvp_count =
            json_parse_cam_matrix(curr->func_arg[0],
                                  &param->mvps[0][0][0],
                                  MAX_MVP_COUNT,
                                  &param->width,
                                  &param->height);

        sscanf(curr->func_arg[1],
               "%hhu,%hhu,%hhu",
               &param->background.x,
               &param->background.y,
               &param->background.z);
        strcpy(param->outpath, curr->func_arg[2]);

        pcp_status_legs_append(pcp_save_viewport_s, param);
        break;
      }
#endif
      default:
      {
        break;
      }
      }
    }
  }
  curr_time = get_current_time_ms();
  /******************************************/
  // Run processes
  for (int t = 0; t < proc_count; t++)
    pcp_process_legs_run(&proc_pcs[t], t);
  // Run statuses
  for (int t = 0; t < proc_count; t++)
    pcp_status_legs_run(&proc_pcs[t], t);
  /******************************************/
  proc_time = get_current_time_ms() - curr_time;

  pcp_free_param();

  curr_time = get_current_time_ms();

  if (arg->plan & PCP_PLAN_NONE_MERGE)
  {
    out_pcs = (pcp_point_cloud_t *)malloc(sizeof(pcp_point_cloud_t));
    pcp_point_cloud_init(out_pcs);
    out_count = point_cloud_merge(proc_pcs, proc_count, &out_pcs[0]);
    for (int i = 0; i < proc_count; i++)
      pcp_point_cloud_free(&proc_pcs[i]);
    free(proc_pcs);
  }
  else if (arg->plan & PCP_PLAN_NONE_TILE)
  {
    int nx    = 0;
    int ny    = 0;
    int nz    = 0;
    nx        = arg->tile.nx;
    ny        = arg->tile.ny;
    nz        = arg->tile.nz;
    out_count = nx * ny * nz;
    proc_pcs[0].tile(&proc_pcs[0], nx, ny, nz, &out_pcs);
    for (int i = 0; i < proc_count; i++)
      pcp_point_cloud_free(&proc_pcs[i]);
    free(proc_pcs);
  }
  else
  {
    out_pcs   = proc_pcs;
    out_count = proc_count;
  }

  post_proc_time = get_current_time_ms() - curr_time;

  curr_time      = get_current_time_ms();

  if (proc_count == 0)
    return 0;
  for (int t = 0; t < out_count; t++)
  {
    if (out_pcs[t].size == 0)
    {
      printf("Tile %d have no points, skip writing...\n", t);
    }
    snprintf(output_tile_path, max_path_size, output_path, t);
    out_pcs[t].write(&out_pcs[t], output_tile_path, binary);
  }

  write_time = get_current_time_ms() - curr_time;

  printf("read time:\t%lld ms\npre-process time:\t%lld "
         "ms\nprocess/status time:\t%lld ms\npost-process "
         "time:\t%lld ms\nwrite time:\t%lld ms\n",
         read_time,
         pre_proc_time,
         proc_time,
         post_proc_time,
         write_time);

  for (int i = 0; i < out_count; i++)
    pcp_point_cloud_free(&out_pcs[i]);
  free(out_pcs);

  free(input_tile_path);
  free(output_tile_path);

  return proc_count;
}

const char *argp_program_version     = "pcp 1.0";
const char *argp_program_bug_address = "quang.nglong@gmail.com";

static char doc[] = "A program that prepare point cloud.";
static struct argp_option options[] = {
    {"input",
     'i', "FILE",
     0, "Input point cloud source file(s) (current support Polygon File "
     "Format)."},
    {"output",
     'o', "FILE",
     0, "Output point cloud source file(s) (current support Polygon "
     "File Format)."},
    {"binary",
     'b', "0|1",
     0, "Output binary or not (0 for not, default is 1)."},
    {"pre-process",
     0x80, "ACTION",
     0, "Set the pre-process action of the program (ACTION can be "
     "either TILE, MERGE, or NONE, default is TILE). If the input "
     "are file path to point cloud tiles, ACTION can only be MERGE "
     "or NONE."},
    {"post-process",
     0x81, "ACTION",
     0, "Set the post-process action of the program (ACTION can be "
     "either TILE, MERGE, or NONE, default is NONE). Post-process "
     "ACTION must be different from pre-process ACTION, except for "
     "action NONE."},
    {"tiled-input",
     0x82, "NUM",
     0, "Input NUM point cloud tiles (1 for normal input, default is "
     "1)."},
    {"tile",
     't', "nx,ny,nz",
     0, "Set the number of division per axis for tiling (default is "
     "1,1,1)."},
    {"process",
     'p', "PROCESS",
     0, "Process which the point cloud undergo, use '--process help' "
     "for more info."},
    {"status",
     's', "STATUS",
     0, "Status of the point cloud given factors, use '--status help' "
     "for more info."},
    {0}
};

static char process_doc[] =
    "Defines a specific process to be applied to the point cloud.";
static struct argp_option process_options[] = {
    {"help", 0, NULL, OPTION_DOC, "Give this help list"},
    {"sample", 0, NULL, OPTION_DOC, "<ratio=FLOAT> <binary=0|1>"},
    {"voxel", 0, NULL, OPTION_DOC, "<voxel-size=FLOAT>"},
    {"remove-duplicates", 0, NULL, OPTION_DOC, "No arguments"},
    {0}
};

static char status_doc[] = "Defines a specific status operation to "
                           "be applied to the point cloud.";
static struct argp_option status_options[] = {
    {"help", 0, NULL, OPTION_DOC, "Give this help list"},
    {"aabb",
     0, NULL,
     OPTION_DOC, "<output=0|1|2> <binary=0|1> <output-path=FILE>"},
#ifdef PCP_STAT_SAVE_VIEWPORT
    {"save-viewport",
     0, NULL,
     OPTION_DOC, "<camera=JSON> <background-color=R,G,B> <output-png(s)=FILE>"},
#endif
    {"pixel-per-tile",
     0, NULL,
     OPTION_DOC, "<camera=JSON> <nx,ny,nz> <output-visibility=JSON>"},
    {"screen-area-estimation",
     0, NULL,
     OPTION_DOC, "<camera=JSON> <output-estimation=JSON>"},
    {0}
};

static struct argp process_argp = {
    process_options, NULL, "[<ARG>...]", process_doc};
static struct argp status_argp = {
    status_options, NULL, "[<ARG>...]", status_doc};
char *safe_dup(const char *src)
{
  if (!src)
    return NULL;
  size_t len = strlen(src);
  char  *dup = malloc(len + 1);
  if (!dup)
    return NULL;
  memcpy(dup, src, len + 1); // includes null terminator
  return dup;
}
static int parse_func_opt(char              *arg,
                          struct argp       *argp,
                          struct argp_state *state,
                          func_t            *funcs,
                          size_t             size,
                          int                max_size,
                          const func_info_t *func_info_list,
                          const char        *name)
{
  if (size >= max_size)
  {
    argp_error(
        state, "Too many options specified (max: %d)", max_size);
    return ARGP_ERR_UNKNOWN;
  }
  if (strcmp(arg, "help") == 0)
  {
    argp_help(argp, stdout, ARGP_HELP_STD_HELP, (char *)name);
    exit(0);
  }

  const func_info_t *info = find_func(arg, func_info_list);
  if (!info)
  {
    argp_error(state, "Unknown option: %s", arg);
    return ARGP_ERR_UNKNOWN;
  }

  func_t *curr   = &funcs[size];
  curr->func_id  = info->func;

  // Collect arguments
  curr->func_arg = (char **)malloc(info->max_args * sizeof(char *));
  curr->func_arg_size = 0;

  // Collect the required number of arguments
  for (int i = 0; i < info->min_args; i++)
  {
    arg = state->argv[state->next++];
    if (!arg)
    {
      argp_error(state,
                 "Option %s requires %d arguments",
                 info->name,
                 info->min_args);
      return ARGP_ERR_UNKNOWN;
    }
    curr->func_arg[i] = safe_dup(arg);
    curr->func_arg_size++;
  }
  return 1;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  struct arguments *args = (struct arguments *)state->input;

  switch (key)
  {
  case 'i':
    // add safe input
    args->input = arg;
    break;

  case 'o':
    // add safe input
    args->output = arg;
    break;

  case 'b':
    // add safe input
    args->binary = atoi(arg);
    break;
  case 0x80:
  {
    if (strcmp(arg, "TILE") == 0)
    {
      args->plan |= PCP_PLAN_TILE_NONE;
    }
    else if (strcmp(arg, "MERGE") == 0)
    {
      args->plan |= PCP_PLAN_MERGE_NONE;
    }
    else if (strcmp(arg, "NONE") == 0)
    {
      args->plan |= PCP_PLAN_NONE_NONE;
    }
    else
    {
      argp_error(state,
                 "Invalid plan format. Use: TILE, MERGE, or NONE");
      return ARGP_ERR_UNKNOWN;
    }
    break;
  }
  case 0x81:
  {
    if (strcmp(arg, "TILE") == 0)
    {
      args->plan |= PCP_PLAN_NONE_TILE;
    }
    else if (strcmp(arg, "MERGE") == 0)
    {
      args->plan |= PCP_PLAN_NONE_MERGE;
    }
    else if (strcmp(arg, "NONE") == 0)
    {
      args->plan |= PCP_PLAN_NONE_NONE;
    }
    else
    {
      argp_error(state,
                 "Invalid plan format. Use: TILE, MERGE, or NONE");
      return ARGP_ERR_UNKNOWN;
    }
    break;
  }
  case 0x82:
    // add safe input
    args->tiled_input = atoi(arg);
    break;
  case 't':
  {
    if (sscanf(arg,
               "%hhu,%hhu,%hhu",
               &args->tile.nx,
               &args->tile.ny,
               &args->tile.nz) != 3)
    {
      argp_error(state, "Invalid tile format. Use: nx,ny,nz");
      return ARGP_ERR_UNKNOWN;
    }
    break;
  }
  case 'p':
  {
    parse_func_opt(arg,
                   &process_argp,
                   state,
                   args->procs,
                   args->procs_size,
                   MAX_PROCESS,
                   processes_g,
                   "--process");
    args->flags |= SET_OPT_PROCESS;
    args->procs_size++;
    break;
  }

  case 's':
  {
    parse_func_opt(arg,
                   &status_argp,
                   state,
                   args->stats,
                   args->stats_size,
                   MAX_STATUS,
                   statuses_g,
                   "--status");
    args->flags |= SET_OPT_STATUS;
    args->stats_size++;
    break;
  }
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, 0, doc};

int                main(int argc, char *argv[])
{
  // default param for args
  struct arguments args = (struct arguments){
      .flags       = 0,
      .input       = NULL,
      .output      = NULL,
      .binary      = 1,
      .tiled_input = 1,
      .plan        = PCP_PLAN_NONE_NONE,
      .procs_size  = 0,
      .stats_size  = 0,
      .tile        = {1, 1, 1}
  };

  argp_parse(&argp, argc, argv, 0, 0, &args);

  printf("input:\t%s\n", args.input);
  printf("output:\t%s\n", args.output);
  printf("binary:\t%d\n", args.binary);

  pcp_prepare(&args);

  arguments_free(&args);
  return 0;
}
