#include "pcprep/point_cloud.h"
#include "pcprep/utils.h"
#include "pcprep/vec3uc.h"
#include "pcprep/wrapper.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

pcp_ret_t pcp_point_cloud_init(pcp_point_cloud_t *self)
{
  *self                    = (pcp_point_cloud_t){0};
  self->alloc              = pcp_point_cloud_alloc;
  self->load               = pcp_point_cloud_load;
  self->write              = pcp_point_cloud_write;
  self->get_min            = pcp_point_cloud_get_min;
  self->get_max            = pcp_point_cloud_get_max;
  self->tile               = pcp_point_cloud_tile;
  self->sample             = pcp_point_cloud_sample;
  self->remove_dupplicates = pcp_point_cloud_remove_dupplicates;
  self->voxelize           = pcp_point_cloud_voxelize;
  self->get_pixel_per_tile = pcp_point_cloud_get_pixel_per_tile;
  return PCPREP_RET_SUCCESS;
}

pcp_ret_t pcp_point_cloud_alloc(pcp_point_cloud_t *pc, size_t size)
{
  pc->size = size;
  pc->pos  = (float *)malloc(sizeof(float) * 3 * pc->size);
  pc->rgb  = (uint8_t *)malloc(sizeof(uint8_t) * 3 * pc->size);
  if (pc->size < 0)
  {
    return PCPREP_RET_FAIL;
  }
  return PCPREP_RET_SUCCESS;
}
pcp_ret_t pcp_point_cloud_free(pcp_point_cloud_t *pc)
{
  if (pc == NULL)
  {
    return PCPREP_RET_FAIL;
  }
  if (pc->pos)
  {
    free(pc->pos);
    pc->pos = NULL;
  }
  if (pc->rgb)
  {
    free(pc->rgb);
    pc->rgb = NULL;
  }
  return PCPREP_RET_SUCCESS;
}
pcp_ret_t pcp_point_cloud_load(pcp_point_cloud_t *pc,
                               const char        *filename)
{
  pc->alloc(pc, ply_count_vertex(filename));
  if (!ply_pcp_point_cloud_loader(filename, pc->pos, pc->rgb))
  {
    return PCPREP_RET_FAIL;
  }
  return PCPREP_RET_SUCCESS;
}
pcp_ret_t pcp_point_cloud_write(pcp_point_cloud_t *pc,
                                const char        *filename,
                                int                binary)
{
  FILE *file = fopen(filename, "wb");
  if (!file)
  {
    perror("Error opening file");
    return PCPREP_RET_FAIL;
  }

  if (binary)
  {
    // Binary PLY Header
    fprintf(file, "ply\n");
    fprintf(file, "format binary_little_endian 1.0\n");
    fprintf(file, "element vertex %zu\n", pc->size);
    fprintf(file, "property float x\n");
    fprintf(file, "property float y\n");
    fprintf(file, "property float z\n");
    fprintf(file, "property uchar red\n");
    fprintf(file, "property uchar green\n");
    fprintf(file, "property uchar blue\n");
    fprintf(file, "end_header\n");

    for (size_t i = 0; i < pc->size; i++)
    {
      fwrite(&pc->pos[i * 3], sizeof(float), 3, file);   // x, y, z
      fwrite(&pc->rgb[i * 3], sizeof(uint8_t), 3, file); // r, g, b
    }
  }
  else
  {
    // ASCII PLY Header
    fprintf(file, "ply\n");
    fprintf(file, "format ascii 1.0\n");
    fprintf(file, "element vertex %zu\n", pc->size);
    fprintf(file, "property float x\n");
    fprintf(file, "property float y\n");
    fprintf(file, "property float z\n");
    fprintf(file, "property uchar red\n");
    fprintf(file, "property uchar green\n");
    fprintf(file, "property uchar blue\n");
    fprintf(file, "end_header\n");

    for (size_t i = 0; i < pc->size; i++)
    {
      fprintf(file,
              "%f %f %f %u %u %u\n",
              pc->pos[i * 3],
              pc->pos[i * 3 + 1],
              pc->pos[i * 3 + 2],
              pc->rgb[i * 3],
              pc->rgb[i * 3 + 1],
              pc->rgb[i * 3 + 2]);
    }
  }

  fclose(file);
  return PCPREP_RET_SUCCESS;
}

pcp_ret_t pcp_point_cloud_get_min(pcp_point_cloud_t *pc,
                                  pcp_vec3f_t       *min)
{
  if (!pc->pos)
  {
    return PCPREP_RET_FAIL;
  }
  pcp_vec3f_t *pos_lst = (pcp_vec3f_t *)(pc->pos);
  *min                 = pos_lst[0];
  for (int i = 0; i < pc->size; i++)
  {
    if (pos_lst[i].x < min->x)
      min->x = pos_lst[i].x;
    if (pos_lst[i].y < min->y)
      min->y = pos_lst[i].y;
    if (pos_lst[i].z < min->z)
      min->z = pos_lst[i].z;
  }
  return PCPREP_RET_SUCCESS;
}
pcp_ret_t pcp_point_cloud_get_max(pcp_point_cloud_t *pc,
                                  pcp_vec3f_t       *max)
{
  if (!pc->pos)
  {
    return PCPREP_RET_FAIL;
  }
  pcp_vec3f_t *pos_lst = (pcp_vec3f_t *)(pc->pos);
  *max                 = pos_lst[0];
  for (int i = 0; i < pc->size; i++)
  {
    if (pos_lst[i].x > max->x)
      max->x = pos_lst[i].x;
    if (pos_lst[i].y > max->y)
      max->y = pos_lst[i].y;
    if (pos_lst[i].z > max->z)
      max->z = pos_lst[i].z;
  }
  return PCPREP_RET_SUCCESS;
}

// TODO: this function is not safe
pcp_ret_t pcp_point_cloud_tile(pcp_point_cloud_t  *pc,
                               int                 n_x,
                               int                 n_y,
                               int                 n_z,
                               pcp_point_cloud_t **tiles)
{
  pcp_vec3f_t  min      = {0};
  pcp_vec3f_t  max      = {0};
  pcp_vec3f_t *pos_lst  = (pcp_vec3f_t *)pc->pos;
  pcp_vec3f_t  n        = (pcp_vec3f_t){n_x, n_y, n_z};
  int          size     = n.x * n.y * n.z;

  int         *numVerts = (int *)malloc(sizeof(int) * size);
  int         *tmp      = (int *)malloc(sizeof(int) * size);
  *tiles =
      (pcp_point_cloud_t *)malloc(sizeof(pcp_point_cloud_t) * size);
  for (int i = 0; i < size; i++)
  {
    pcp_point_cloud_init(&(*tiles)[i]);
  }

  if (!tiles)
  {
    free(numVerts);
    free(tmp);
    return PCPREP_RET_FAIL;
  }

  pc->get_min(pc, &min);
  pc->get_max(pc, &max);

  for (int t = 0; t < size; t++)
  {
    numVerts[t] = 0;
    tmp[t]      = 0;
  }
  for (int i = 0; i < pc->size; i++)
  {
    int t = get_tile_id(n, min, max, pos_lst[i]);
    numVerts[t]++;
  }

  for (int t = 0; t < size; t++)
  {
    (*tiles)[t].alloc(&(*tiles)[t], numVerts[t]);
  }

  for (int i = 0; i < pc->size; i++)
  {
    int t = get_tile_id(n, min, max, pos_lst[i]);
    (*tiles)[t].pos[3 * tmp[t]]     = pc->pos[3 * i];
    (*tiles)[t].pos[3 * tmp[t] + 1] = pc->pos[3 * i + 1];
    (*tiles)[t].pos[3 * tmp[t] + 2] = pc->pos[3 * i + 2];
    (*tiles)[t].rgb[3 * tmp[t]]     = pc->rgb[3 * i];
    (*tiles)[t].rgb[3 * tmp[t] + 1] = pc->rgb[3 * i + 1];
    (*tiles)[t].rgb[3 * tmp[t] + 2] = pc->rgb[3 * i + 2];
    tmp[t]++;
  }

  free(numVerts);
  free(tmp);

  return PCPREP_RET_SUCCESS;
}

pcp_ret_t pcp_point_cloud_sample(pcp_point_cloud_t *pc,
                                 float              ratio,
                                 unsigned char      strategy,
                                 pcp_point_cloud_t *out)
{
  size_t num_points = (size_t)(pc->size * ratio);

  out->alloc(out, num_points);

  switch (strategy)
  {
  case PCP_SAMPLE_RULE_UNIFORM:
  {
    int *index_arr = (int *)malloc(sizeof(int) * pc->size);
    int *sample    = (int *)malloc(num_points * sizeof(int));
    // Seed the random number generator
    srand((unsigned int)time(NULL));
    for (int i = 0; i < pc->size; i++)
      index_arr[i] = i;
    sample_union(index_arr, pc->size, sample, num_points);
    for (int i = 0; i < num_points; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        out->pos[i * 3 + j] = pc->pos[sample[i] * 3 + j];
        out->rgb[i * 3 + j] = pc->rgb[sample[i] * 3 + j];
      }
    }
    free(index_arr);
    free(sample);
    break;
  }
  default:
    break;
  }

  return PCPREP_RET_SUCCESS;
}

static void point_cloud_element_merge(pcp_point_cloud_t pc,
                                      int               left,
                                      int               mid,
                                      int               right)
{
  pcp_vec3f_t  *arr_pos = (pcp_vec3f_t *)pc.pos;
  pcp_vec3uc_t *arr_col = (pcp_vec3uc_t *)pc.rgb;

  int           n1      = mid - left + 1;
  int           n2      = right - mid;

  pcp_vec3f_t  *L_pos =
      (pcp_vec3f_t *)malloc(n1 * sizeof(pcp_vec3f_t));
  pcp_vec3f_t *R_pos =
      (pcp_vec3f_t *)malloc(n2 * sizeof(pcp_vec3f_t));

  pcp_vec3uc_t *L_col =
      (pcp_vec3uc_t *)malloc(n1 * sizeof(pcp_vec3uc_t));
  pcp_vec3uc_t *R_col =
      (pcp_vec3uc_t *)malloc(n2 * sizeof(pcp_vec3uc_t));

  for (int i = 0; i < n1; i++)
  {
    *(L_pos + i) = arr_pos[left + i];
    *(L_col + i) = arr_col[left + i];
  }
  for (int i = 0; i < n2; i++)
  {
    *(R_pos + i) = arr_pos[mid + 1 + i];
    *(R_col + i) = arr_col[mid + 1 + i];
  }
  int i = 0, j = 0, k = left;
  while (i < n1 && j < n2)
  {
    if (pcp_vec3f_leq(*(L_pos + i), *(R_pos + j)))
    {
      arr_pos[k]   = *(L_pos + i);
      arr_col[k++] = *(L_col + i++);
    }
    else
    {
      arr_pos[k]   = *(R_pos + j);
      arr_col[k++] = *(R_col + j++);
    }
  }
  while (i < n1)
  {
    arr_pos[k]   = *(L_pos + i);
    arr_col[k++] = *(L_col + i++);
  }
  while (j < n2)
  {
    arr_pos[k]   = *(R_pos + j);
    arr_col[k++] = *(R_col + j++);
  }

  free(L_pos);
  free(R_pos);
  free(L_col);
  free(R_col);
}
static void point_cloud_element_merge_sort(pcp_point_cloud_t pc,
                                           int               left,
                                           int               right)
{
  if (left >= right)
    return;
  int mid = left + (right - left) / 2;

  point_cloud_element_merge_sort(pc, left, mid);
  point_cloud_element_merge_sort(pc, mid + 1, right);

  point_cloud_element_merge(pc, left, mid, right);
}

pcp_ret_t pcp_point_cloud_remove_dupplicates(pcp_point_cloud_t *pc,
                                             pcp_point_cloud_t *out)
{
  // use mergesort to sort points, then remove consecutives,
  // O(Nlog(N))
  point_cloud_element_merge_sort(*pc, 0, pc->size - 1);

  // count unique points to get output size
  size_t count_unique = 1;
  for (int i = 1; i < pc->size; i++)
  {
    pcp_vec3f_t pre  = ((pcp_vec3f_t *)pc->pos)[i - 1];
    pcp_vec3f_t curr = ((pcp_vec3f_t *)pc->pos)[i];
    if (!pcp_vec3f_eq(pre, curr))
    {
      count_unique++;
    }
  }
  out->alloc(out, count_unique);

  // puts the unique points into the output, O(N)
  pcp_vec3f_t  *uni_pos = (pcp_vec3f_t *)(out->pos);
  pcp_vec3uc_t *uni_col = (pcp_vec3uc_t *)(out->rgb);
  pcp_vec3f_t  *pos     = (pcp_vec3f_t *)(pc->pos);
  pcp_vec3uc_t *col     = (pcp_vec3uc_t *)(pc->rgb);

  int           index   = 0;
  for (int i = 1; i < pc->size; i++)
  {
    if (!pcp_vec3f_eq(pos[i], pos[i - 1]))
    {
      uni_pos[index]   = pos[i];
      uni_col[index++] = col[i]; // Start the color for the new point
    }
  }

  return PCPREP_RET_SUCCESS;
}

pcp_ret_t pcp_point_cloud_voxelize(pcp_point_cloud_t *pc,
                                   float              voxel_size,
                                   pcp_point_cloud_t *out)
{
  // quantize the points to the voxel grid
  // then remove the duplicates, should it tho ?

  out->alloc(out, pc->size);

  memcpy(out->pos, pc->pos, pc->size * sizeof(float) * 3);
  memcpy(out->rgb, pc->rgb, pc->size * sizeof(uint8_t) * 3);

  pcp_vec3f_t *pos = (pcp_vec3f_t *)out->pos;
  for (int i = 0; i < out->size; i++)
    pos[i] = pcp_vec3f_quantize(pos[i], voxel_size);

  // pcp_point_cloud_remove_dupplicates(pc, out);
  return PCPREP_RET_SUCCESS;
}

pcp_ret_t pcp_point_cloud_get_pixel_per_tile(pcp_point_cloud_t *pc,
                                             int                nx,
                                             int                ny,
                                             int                nz,
                                             int                width,
                                             int    height,
                                             float *mvp,
                                             int   *pixel_count)
{
  for (int i = 0; i < nx * ny * nz; i++)
    pixel_count[i] = 0;

  pcp_vec3f_t *points    = (pcp_vec3f_t *)pc->pos;
  pcp_vec3f_t  ndc       = {0, 0, 0};
  int          screen_w  = 0;
  int          screen_h  = 0;
  int          tile_id   = 0;
  float      **minZvalue = NULL;
  int16_t    **curr_tile = NULL;
  pcp_vec3f_t  min, max;
  pc->get_min(pc, &min);
  pc->get_max(pc, &max);

  minZvalue = (float **)malloc(sizeof(float *) * height);
  for (int i = 0; i < height; i++)
  {
    minZvalue[i] = (float *)malloc(sizeof(float) * width);
    for (int j = 0; j < width; j++)
      minZvalue[i][j] = 2.0f;
    // 2 because it is smaller than the max of NDC [-1, 1]
  }

  curr_tile = (int16_t **)malloc(sizeof(int16_t *) * height);
  for (int i = 0; i < height; i++)
  {
    curr_tile[i] = (int16_t *)malloc(sizeof(int16_t) * width);
    for (int j = 0; j < width; j++)
      curr_tile[i][j] = -1;
  }

  for (int i = 0; i < pc->size; i++)
  {
    tile_id = (int)get_tile_id(
        (pcp_vec3f_t){nx, ny, nz}, min, max, points[i]);

    pcp_vec3f_t ndc = pcp_vec3f_mvp_mul(points[i], mvp);
    // check only if ndc is in side view-frustum
    if (ndc.x >= -1 && ndc.x <= 1 && ndc.y >= -1 && ndc.y <= 1 &&
        ndc.z >= 0 && ndc.z <= 1)
    {
      // screen_w = (int)((ndc.x + 1.0) * 0.5 * width);
      // screen_h = (int)((1.0 - ndc.y) * 0.5 * height);

      screen_w = fminf(
          width - 1, fmaxf(0, (int)((ndc.x + 1.0f) * 0.5f * width)));
      screen_h =
          fminf(height - 1,
                fmaxf(0, (int)((1.0f - ndc.y) * 0.5f * height)));

      // If current point is nearer to the camera, then:
      // - minZvalue is updated
      // curr_tile is also updated
      if (ndc.z < minZvalue[screen_h][screen_w])
      {
        minZvalue[screen_h][screen_w] = ndc.z;
        curr_tile[screen_h][screen_w] = tile_id;
      }
    }
  }
  // After we have known each pixel hold the point of which tile, we
  // count.
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      if (curr_tile[i][j] >= 0)
      {
        pixel_count[curr_tile[i][j]]++;
      }
    }
  }

  for (int i = 0; i < height; i++)
    free(minZvalue[i]);
  free(minZvalue);
  for (int i = 0; i < height; i++)
    free(curr_tile[i]);
  free(curr_tile);
  return PCPREP_RET_SUCCESS;
}
