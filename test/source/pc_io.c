#include <pcprep/pointcloud.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
    pointcloud_t pc;
    pc.rgb = NULL;
    pc.pos = NULL;
    pc.size = 0;

    int prep = pointcloud_load(&pc, argv[1]);
    printf("%lu\n", pc.size);
    printf("%f\n", pc.pos[123]);

    pointcloud_write(pc, "out_msh.ply", 1);
    pointcloud_free(&pc);
}
