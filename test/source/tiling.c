#include <pcprep/point_cloud.h>
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
  const char        *input_file_path = argv[1];
  int                n_x             = atoi(argv[2]);
  int                n_y             = atoi(argv[3]);
  int                n_z             = atoi(argv[4]);
  int                isBinary        = atoi(argv[5]);
  char              *out_folder      = argv[6];
  char               out_file_name[1024];
  pcp_point_cloud_t  pc         = {0};
  pcp_point_cloud_t *tiles      = NULL;
  struct stat        st         = {0};
  int                tile_count = 0;

  pcp_point_cloud_init(&pc);

  pc.load(&pc, input_file_path);
  tile_count = n_x * n_y * n_z;
  pc.tile(&pc, n_x, n_y, n_z, &tiles);

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
    tiles[t].write(&tiles[t], out_file_name, isBinary);
  }

  pcp_point_cloud_free(&pc);
  for (int i = 0; i < tile_count; i++)
    pcp_point_cloud_free(&tiles[i]);
  free(tiles);
}
