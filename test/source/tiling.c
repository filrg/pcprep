#include <pcprep/pointcloud.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  if (argc < 7)
  {
    printf("Usage: %s <input_file_path> <n_x> <n_y> <n_z> "
           "<is_binary> <output_folder>\n",
           argv[0]);
    return 1;
  }
  // Parse command-line arguments
  const char   *input_file_path = argv[1];
  int           n_x             = atoi(argv[2]);
  int           n_y             = atoi(argv[3]);
  int           n_z             = atoi(argv[4]);
  int           isBinary        = atoi(argv[5]);
  char         *out_folder      = argv[6];
  char          out_file_name[1024];
  pointcloud_t  pc         = {0};
  pointcloud_t *tiles      = NULL;
  struct stat   st         = {0};
  int           tile_count = 0;

  pointcloud_load(&pc, input_file_path);
  tile_count = pointcloud_tile(pc, n_x, n_y, n_z, &tiles);

  if (stat(out_folder, &st) == -1)
  {
    mkdir(out_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }

  for (int t = 0; t < n_x * n_y * n_z; t++)
  {
    if (tiles[t].size == 0)
    {
      printf("Tile %d have no points, skip writing...\n", t);
    }
    snprintf(out_file_name,
             sizeof(out_file_name),
             "%s/tile%.04d.ply",
             out_folder,
             t);
    pointcloud_write(tiles[t], out_file_name, isBinary);
  }

  pointcloud_free(&pc);
  for (int i = 0; i < tile_count; i++)
    pointcloud_free(&tiles[i]);
  free(tiles);
}
