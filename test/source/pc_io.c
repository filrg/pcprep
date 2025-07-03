#include <pcprep/point_cloud.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
  pcp_point_cloud_t pc = {0};

  pcp_point_cloud_init(&pc);

  int prep = pc.load(&pc, argv[1]);
  printf("%lu\n", pc.size);
  printf("%f\n", pc.pos[123]);

  pc.write(&pc, "out_msh.ply", 1);
  pcp_point_cloud_free(&pc);
}
