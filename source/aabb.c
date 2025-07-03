#include <pcprep/aabb.h>
#include <pcprep/vec3u.h>

pcp_ret_t pcp_aabb_init(pcp_aabb_t *self)
{
  *self         = (pcp_aabb_t){0};
  self->to_mesh = pcp_aabb_to_mesh;
  return PCPREP_RET_SUCCESS;
}

pcp_ret_t pcp_aabb_free(pcp_aabb_t *self)
{
  *self = (pcp_aabb_t){0};
  return PCPREP_RET_SUCCESS;
}

pcp_ret_t pcp_aabb_to_mesh(pcp_aabb_t *self, pcp_mesh_t *mesh)
{
  // Check if the AABB is valid
  if (self->min.x > self->max.x || self->min.y > self->max.y ||
      self->min.z > self->max.z)
  {
    return PCPREP_RET_FAIL;
  }

  pcp_vec3f_t *vertices = PCPREP_NULL;
  pcp_vec3u_t *faces    = PCPREP_NULL;

  mesh->alloc(mesh, 8, 12 * 3);

  vertices    = (pcp_vec3f_t *)mesh->pos;

  vertices[0] = self->min;
  vertices[1] = pcp_vec3f_set(self->min.x, self->min.y, self->max.z);
  vertices[2] = pcp_vec3f_set(self->min.x, self->max.y, self->min.z);
  vertices[3] = pcp_vec3f_set(self->min.x, self->max.y, self->max.z);
  vertices[4] = pcp_vec3f_set(self->max.x, self->min.y, self->min.z);
  vertices[5] = pcp_vec3f_set(self->max.x, self->min.y, self->max.z);
  vertices[6] = pcp_vec3f_set(self->max.x, self->max.y, self->min.z);
  vertices[7] = self->max;

  faces       = (pcp_vec3u_t *)mesh->indices;
  faces[0]    = pcp_vec3u_set(0, 1, 3);
  faces[1]    = pcp_vec3u_set(0, 3, 2);
  faces[2]    = pcp_vec3u_set(4, 7, 5);
  faces[3]    = pcp_vec3u_set(4, 6, 7);
  faces[4]    = pcp_vec3u_set(0, 6, 4);
  faces[5]    = pcp_vec3u_set(0, 2, 6);
  faces[6]    = pcp_vec3u_set(1, 5, 7);
  faces[7]    = pcp_vec3u_set(1, 7, 3);
  faces[8]    = pcp_vec3u_set(0, 5, 1);
  faces[9]    = pcp_vec3u_set(0, 4, 5);
  faces[10]   = pcp_vec3u_set(2, 3, 7);
  faces[11]   = pcp_vec3u_set(2, 7, 6);
  return PCPREP_RET_SUCCESS;
}
