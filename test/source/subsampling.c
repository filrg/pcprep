#include <pcprep/pointcloud.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void samplef(int *input, int input_size, int *output, int output_size)
{
  if (output_size > input_size)
  {
    fprintf(stderr,
            "Error: output_size cannot be larger than input_size.\n");
    return;
  }

  // Create an array to keep track of selected indices
  int *selected = calloc(input_size, sizeof(int));
  if (!selected)
  {
    fprintf(stderr, "Memory allocation failed.\n");
    return;
  }

  srand((unsigned int)time(NULL)); // Seed the random number generator

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

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    printf("Usage: %s <input_file_path> <subsample_percentage> "
           "<output_file_path>\n",
           argv[0]);
    return 1;
  }

  pointcloud_t pc           = {0};
  pointcloud_t sub          = {0};
  float        NvertPercent = 0;
  int          subVerts     = 0;
  int         *index_arr    = NULL;
  int         *sample       = NULL;

  pointcloud_load(&pc, argv[1]);
  NvertPercent = atof(argv[2]);
  subVerts     = (int)(pc.size * NvertPercent);

  pointcloud_init(&sub, subVerts);
  index_arr = (int *)malloc(sizeof(int) * pc.size);
  sample    = (int *)malloc(subVerts * sizeof(int));
  srand((unsigned int)time(NULL)); // Seed the random number generator

  for (int i = 0; i < pc.size; i++)
    index_arr[i] = i;

  samplef(index_arr, pc.size, sample, subVerts);

  for (int i = 0; i < subVerts; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      sub.pos[i * 3 + j] = pc.pos[sample[i] * 3 + j];
      sub.rgb[i * 3 + j] = pc.rgb[sample[i] * 3 + j];
    }
  }
  pointcloud_write(sub, argv[3], 1);

  pointcloud_free(&pc);
  pointcloud_free(&sub);
  free(index_arr);
  free(sample);
}
