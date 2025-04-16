#include <pcprep/aabb.h>
#include <pcprep/vec3u.h>

int aabb_to_mesh(aabb_t aabb, mesh_t *mesh)
{
  // Check if the AABB is valid
  if (aabb.min.x > aabb.max.x || aabb.min.y > aabb.max.y ||
      aabb.min.z > aabb.max.z)
  {
    return -1;
  }
  mesh_init(mesh, 8, 12 * 3);

  vec3f_t *vertices = (vec3f_t *)mesh->pos;

  vertices[0]       = aabb.min;
  vertices[1]       = vec3f_set(aabb.min.x, aabb.min.y, aabb.max.z);
  vertices[2]       = vec3f_set(aabb.min.x, aabb.max.y, aabb.min.z);
  vertices[3]       = vec3f_set(aabb.min.x, aabb.max.y, aabb.max.z);
  vertices[4]       = vec3f_set(aabb.max.x, aabb.min.y, aabb.min.z);
  vertices[5]       = vec3f_set(aabb.max.x, aabb.min.y, aabb.max.z);
  vertices[6]       = vec3f_set(aabb.max.x, aabb.max.y, aabb.min.z);
  vertices[7]       = aabb.max;

  vec3u_t *faces    = (vec3u_t *)mesh->indices;
  faces[0]          = vec3u_set(0, 1, 3);
  faces[1]          = vec3u_set(0, 3, 2);
  faces[2]          = vec3u_set(4, 7, 5);
  faces[3]          = vec3u_set(4, 6, 7);
  faces[4]          = vec3u_set(0, 6, 4);
  faces[5]          = vec3u_set(0, 2, 6);
  faces[6]          = vec3u_set(1, 5, 7);
  faces[7]          = vec3u_set(1, 7, 3);
  faces[8]          = vec3u_set(0, 5, 1);
  faces[9]          = vec3u_set(0, 4, 5);
  faces[10]         = vec3u_set(2, 3, 7);
  faces[11]         = vec3u_set(2, 7, 6);
  return 0;
}
