#define _POSIX_C_SOURCE 199309L
#include <cJSON.h>
#include <math.h>
#include <pcprep/point_cloud.h>
#include <pcprep/utils.h>
#include <pcprep/vec3f.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define MAX_POINTS 10 // Maximum points after clipping

int float_error(float a, float b, float e)
{
  return a - b < e && a - b > -e;
}

int float_equal(float a, float b)
{
  return float_error(a, b, PCP_FLOAT_ERROR);
}

// Function to compute the area of a polygon given its vertices
static float polygon_area(vec2f_t *points, int n)
{
  if (n < 3)
    return 0.0; // No valid area if less than 3 points
  float area = 0.0;
  for (int i = 0; i < n; i++)
  {
    int j = (i + 1) % n;
    area += points[i].x * points[j].y - points[j].x * points[i].y;
  }
  return 0.5 * fabs(area);
}
// Function to check if a point is inside the given boundary
static int inside(vec2f_t p, int edge)
{
  switch (edge)
  {
  case 0:
    return p.x >= -1; // Left edge
  case 1:
    return p.x <= 1; // Right edge
  case 2:
    return p.y >= -1; // Bottom edge
  case 3:
    return p.y <= 1; // Top edge
  }
  return 0;
}
// Function to compute intersection of a line with a square boundary
static vec2f_t intersection(vec2f_t p1, vec2f_t p2, int edge)
{
  vec2f_t inter;
  float   m;
  if (float_equal(p1.x, p2.x))
    m = 1e9; // Avoid division by zero for vertical lines
  else
    m = (p2.y - p1.y) / (p2.x - p1.x);
  switch (edge)
  {
  case 0: // Left edge (x = -1)
    inter.x = -1;
    inter.y = p1.y + m * (-1 - p1.x);
    break;
  case 1: // Right edge (x = 1)
    inter.x = 1;
    inter.y = p1.y + m * (1 - p1.x);
    break;
  case 2: // Bottom edge (y = -1)
    inter.y = -1;
    inter.x = p1.x + (inter.y - p1.y) / m;
    break;
  case 3: // Top edge (y = 1)
    inter.y = 1;
    inter.x = p1.x + (inter.y - p1.y) / m;
    break;
  }
  return inter;
}
// Function to clip a polygon against a square
static int
clip_polygon(vec2f_t *in, int in_len, vec2f_t *out, int edge)
{
  if (in_len == 0)
    return 0;
  int     out_len     = 0;
  vec2f_t prev        = in[in_len - 1];
  int     prev_inside = inside(prev, edge);
  for (int i = 0; i < in_len; i++)
  {
    vec2f_t curr        = in[i];
    int     curr_inside = inside(curr, edge);
    if (curr_inside)
    {
      if (!prev_inside)
      {
        out[out_len++] =
            intersection(prev, curr, edge); // Entering intersection
      }
      out[out_len++] = curr; // Current point is inside
    }
    else if (prev_inside)
    {
      out[out_len++] =
          intersection(prev, curr, edge); // Exiting intersection
    }
    prev        = curr;
    prev_inside = curr_inside;
  }
  return out_len;
}

long long get_current_time_ms(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return (ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);
}

int sample_union(int *input,
                 int  input_size,
                 int *output,
                 int  output_size)
{
  if (output_size > input_size)
  {
    return -1;
  }
  // Create an array to keep track of selected indices
  int *selected = calloc(input_size, sizeof(int));
  if (!selected)
  {
    return -1;
  }
  // Seed the random number generator
  srand((unsigned int)time(NULL));
  int selected_count = 0;
  while (selected_count < output_size)
  {
    int index = rand() % input_size;
    // Ensure no duplicates
    if (!selected[index])
    {
      selected[index]          = 1;
      output[selected_count++] = input[index];
    }
  }
  free(selected);
}
float quantize(float x, float q)
{
  return q * floor(x / q + 0.5f);
}
char *read_file(const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    fprintf(stderr, "Could not open file: %s\n", filename);
    return NULL;
  }
  // Get file size
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);
  // Allocate buffer
  char *buffer = (char *)malloc(length + 1);
  if (buffer == NULL)
  {
    fprintf(stderr, "Memory allocation failed.\n");
    fclose(file);
    return NULL;
  }
  // Read file into buffer
  size_t read_length  = fread(buffer, 1, length, file);
  buffer[read_length] = '\0'; // Null-terminate the string
  fclose(file);
  return buffer;
}
void json_write_to_file(const char *filename, void *json)
{
  cJSON *cjson       = (cJSON *)json;
  char  *json_string = cJSON_Print(cjson);
  FILE  *file        = fopen(filename, "w");
  if (file)
  {
    fprintf(file, "%s", json_string);
    fclose(file);
  }
  else
  {
    printf("Error writing to file: %s\n", filename);
  }
  free(json_string);
}
int json_parse_cam_matrix(char   *filepath,
                          float  *mvps,
                          size_t  mvps_size,
                          size_t *width,
                          size_t *height)
{
  char  *json_buff = read_file(filepath);
  cJSON *json      = cJSON_Parse(json_buff);
  cJSON *camera    = cJSON_GetObjectItem(json, "camera");
  cJSON *screen    = cJSON_GetObjectItem(camera, "screen");
  *width  = (size_t)cJSON_GetObjectItem(screen, "width")->valueint;
  *height = (size_t)cJSON_GetObjectItem(screen, "height")->valueint;
  cJSON *mvp_array        = cJSON_GetObjectItem(camera, "mvp");
  int    actual_mvp_count = cJSON_GetArraySize(mvp_array);
  // if the allocated is smaller than the actual count, then the
  // allocated is used. else if it is larger, the actual count is
  // used, the parsed mvp count is returned.
  if (mvps_size < actual_mvp_count)
    actual_mvp_count = mvps_size;
  for (int f = 0; f < actual_mvp_count; f++)
  {
    cJSON *matrix = cJSON_GetArrayItem(mvp_array, f);
    for (int i = 0; i < 4; i++)
    {
      cJSON *row = cJSON_GetArrayItem(matrix, i);
      for (int j = 0; j < 4; j++)
      {
        mvps[f * 4 * 4 + i * 4 + j] =
            (float)cJSON_GetArrayItem(row, j)->valuedouble;
      }
    }
  }
  cJSON_Delete(json);
  free(json_buff);
  return actual_mvp_count;
}
int json_write_tiles_pixel(char  *outpath,
                           int    num_tile,
                           int    num_view,
                           int  **pixel_count_per_tile,
                           size_t total_pixel)
{
  cJSON *view      = NULL;
  cJSON *viewArray = NULL;

  view             = cJSON_CreateObject();
  viewArray        = cJSON_CreateArray();

  for (int v = 0; v < num_view; v++)
  {
    cJSON *view_item = cJSON_CreateObject();
    cJSON_AddNumberToObject(view_item, "id", v);
    cJSON *tile_visi_array = cJSON_CreateArray();
    for (int t = 0; t < num_tile; t++)
    {
      cJSON *tile_item = cJSON_CreateObject();
      cJSON_AddNumberToObject(tile_item, "id", t);
      cJSON_AddNumberToObject(
          tile_item, "pixel-count", pixel_count_per_tile[v][t]);
      cJSON_AddNumberToObject(tile_item,
                              "screen-ratio",
                              pixel_count_per_tile[v][t] * 1.0f /
                                  total_pixel);
      cJSON_AddItemToArray(tile_visi_array, tile_item);
    }
    cJSON_AddItemToObject(
        view_item, "tile-visibility", tile_visi_array);
    cJSON_AddItemToArray(viewArray, view_item);
  }
  cJSON_AddItemToObject(view, "view", viewArray);
  json_write_to_file(outpath, view);

  cJSON_Delete(view);
}

int json_write_screen_area_estimation(char  *outpath,
                                      int    num_view,
                                      size_t width,
                                      size_t height,
                                      float *screen_ratio)
{
  cJSON *view = cJSON_CreateObject();
  cJSON_AddNumberToObject(view, "width", width);
  cJSON_AddNumberToObject(view, "height", height);

  cJSON *viewArray = cJSON_CreateArray();
  for (int v = 0; v < num_view; v++)
  {
    cJSON *view_item = cJSON_CreateObject();
    cJSON_AddNumberToObject(view_item, "id", v);
    cJSON_AddNumberToObject(
        view_item, "screen-ratio", screen_ratio[v]);
    // cJSON_AddNumberToObject(view_item, "screen-area", width *
    // height * screen_ratio[v]);
    cJSON_AddItemToArray(viewArray, view_item);
  }
  cJSON_AddItemToObject(view, "view", viewArray);
  json_write_to_file(outpath, view);
  cJSON_Delete(view);
}

float clipped_triangle_area(vec2f_t p1, vec2f_t p2, vec2f_t p3)
{
  vec2f_t polygon[MAX_POINTS] = {p1, p2, p3};
  int     polygon_size        = 3;
  vec2f_t temp[MAX_POINTS];
  for (int edge = 0; edge < 4; edge++)
  {
    polygon_size = clip_polygon(polygon, polygon_size, temp, edge);
    for (int i = 0; i < polygon_size; i++)
      polygon[i] = temp[i]; // Copy back
  }
  return polygon_area(polygon, polygon_size);
}

int flip_image(unsigned char **row_pointers,
               unsigned char  *pixels,
               size_t          width,
               size_t          height)
{
  for (int y = 0; y < height; y++)
  {
    memcpy(row_pointers[y], &pixels[y * width * 3], width * 3);
  }
  for (int i = 0; i < height / 2; ++i)
  {
    unsigned char *temp          = row_pointers[i];
    row_pointers[i]              = row_pointers[height - 1 - i];
    row_pointers[height - 1 - i] = temp;
  }
}

int save_viewport(unsigned char **row_pointers,
                  int             width,
                  int             height,
                  const char     *filename)
{

  // Open the PNG file for writing
  FILE *fp = fopen(filename, "wb");
  if (!fp)
  {
    fprintf(stderr, "Error: could not open PNG file for writing\n");
    exit(1);
  }

  // Initialize the PNG writer
  png_structp png_ptr = png_create_write_struct(
      PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
  {
    fprintf(stderr,
            "Error: could not initialize PNG write structure\n");
    exit(1);
  }

  // Initialize the PNG info structure
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    fprintf(stderr,
            "Error: could not initialize PNG info structure\n");
    exit(1);
  }

  // Set the error handling for the PNG writer
  if (setjmp(png_jmpbuf(png_ptr)))
  {
    fprintf(stderr, "Error: PNG write failed\n");
    exit(1);
  }

  // Set the output file handle
  png_init_io(png_ptr, fp);

  // Set the image dimensions and format
  png_set_IHDR(png_ptr,
               info_ptr,
               width,
               height,
               8,
               PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  // Write the PNG header and image data
  png_write_info(png_ptr, info_ptr);
  png_write_image(png_ptr, row_pointers);
  png_write_end(png_ptr, NULL);

  // Clean up and close the file
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
}

int get_tile_id(pcp_vec3f_t n,
                pcp_vec3f_t min,
                pcp_vec3f_t max,
                pcp_vec3f_t v)
{
  if (v.x > max.x || v.y > max.y || v.z > max.z || v.x < min.x ||
      v.y < min.y || v.z < min.z)
    return -1;
  pcp_vec3f_t ans;
  // ans = (v - min) / ((max - min) / n);
  // or ans = (v - min) * (invs(max - min) / n);
  // or ans = (v - min) * (invs(max - min) * invs(n));
  // or ans = (v - min) * invs(max - min) * n);
  ans = pcp_vec3f_mul(
      pcp_vec3f_mul(pcp_vec3f_sub(v, min),
                    pcp_vec3f_inverse(pcp_vec3f_sub(max, min))),
      n);
  ans = (pcp_vec3f_t){(int)(ans.x < n.x ? ans.x : n.x - 1),
                      (int)(ans.y < n.y ? ans.y : n.y - 1),
                      (int)(ans.z < n.z ? ans.z : n.z - 1)};

  return ans.z + ans.y * n.z + ans.x * n.y * n.z;
}

int point_cloud_merge(pcp_point_cloud_t *pcs,
                      size_t             pc_count,
                      pcp_point_cloud_t *out)
{
  size_t total_size = 0;
  for (int i = 0; i < pc_count; i++)
    total_size += pcs[i].size;
  if (out->alloc(out, total_size) == PCPREP_RET_FAIL)
  {
    return -1; // Memory allocation failed
  }

  int curr = 0;
  for (int i = 0; i < pc_count; i++)
  {
    memcpy((char *)(out->pos + curr * 3),
           (char *)pcs[i].pos,
           pcs[i].size * 3 * sizeof(float));
    memcpy((char *)(out->rgb + curr * 3),
           (char *)pcs[i].rgb,
           pcs[i].size * 3 * sizeof(uint8_t));
    curr += pcs[i].size;
  }
  return 1;
}
