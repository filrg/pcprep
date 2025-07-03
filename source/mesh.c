#include <pcprep/mesh.h>
#include <pcprep/utils.h>
#include <pcprep/vec3f.h>
#include <pcprep/wrapper.h>
#define MSH_PLY_INCLUDE_LIBC_HEADERS
#define MSH_PLY_IMPLEMENTATION
#include <msh_ply.h>

pcp_ret_t pcp_mesh_init(pcp_mesh_t *self)
{
  *self                  = (pcp_mesh_t){0};
  self->alloc            = pcp_mesh_alloc;
  self->load             = pcp_mesh_load;
  self->write            = pcp_mesh_write;
  self->get_screen_ratio = pcp_mesh_get_screen_ratio;
  return PCPREP_RET_SUCCESS;
}

pcp_ret_t pcp_mesh_alloc(pcp_mesh_t *self,
                         uint32_t    num_verts,
                         uint32_t    num_indices)
{
  if (num_verts < 0 || num_indices < 0)
  {
    return PCPREP_RET_FAIL;
  }
  self->pos       = (float *)malloc(sizeof(float) * 3 * num_verts);
  self->num_verts = num_verts;

  self->indices = (uint32_t *)malloc(sizeof(uint32_t) * num_indices);
  self->num_indices = num_indices;
  return PCPREP_RET_SUCCESS;
}
pcp_ret_t pcp_mesh_free(pcp_mesh_t *self)
{
  if (self == NULL)
  {
    return PCPREP_RET_FAIL;
  }
  if (self->pos)
  {
    free(self->pos);
    self->pos = NULL;
  }
  if (self->indices)
  {
    free(self->indices);
    self->indices = NULL;
  }
  return PCPREP_RET_SUCCESS;
}
pcp_ret_t pcp_mesh_load(pcp_mesh_t *self, const char *filename)
{
  self->alloc(
      self, ply_count_vertex(filename), ply_count_face(filename) * 3);
  if (!ply_mesh_loader(filename, self->pos, self->indices))
  {
    return PCPREP_RET_FAIL;
  }
  return PCPREP_RET_SUCCESS;
}
pcp_ret_t
pcp_mesh_write(pcp_mesh_t *self, const char *filename, int binary)
{
  msh_ply_desc_t descriptors[2] = {0};
  descriptors[0]                = (msh_ply_desc_t){
                     .element_name   = "vertex",
                     .property_names = (const char *[]){"x", "y", "z"},
                     .num_properties = 3,
                     .data_type      = MSH_PLY_FLOAT,
                     .data           = &(self->pos),
                     .data_count     = &(self->num_verts)
  };

  uint32_t num_faces = self->num_indices / 3;
  descriptors[1]     = (msh_ply_desc_t){
          .element_name   = "face",
          .property_names = (const char *[]){"vertex_indices"},
          .num_properties = 1,
          .data_type      = MSH_PLY_INT32,
          .list_type      = MSH_PLY_UINT8,
          .data           = &(self->indices),
          .data_count     = &num_faces,
          .list_size_hint = 3};

  const char *write_format = binary ? "wb" : "w";
  msh_ply_t  *pf           = msh_ply_open(filename, write_format);
  if (pf)
  {
    msh_ply_add_descriptor(pf, &descriptors[0]);
    msh_ply_add_descriptor(pf, &descriptors[1]);
    msh_ply_write(pf);
  }
  msh_ply_close(pf);
  return PCPREP_RET_SUCCESS;
}

static int is_toward(vec2f_t a, vec2f_t b, vec2f_t c)
{
  return (b.x - a.x) * (c.y - a.y) > (c.x - a.x) * (b.y - a.y);
}

pcp_ret_t pcp_mesh_get_screen_ratio(pcp_mesh_t *self,
                                    float      *mvp,
                                    float      *screen_ratio)
{
  pcp_vec3f_t *vertices = PCPREP_NULL;
  pcp_vec3f_t *ndcs     = PCPREP_NULL;

  *screen_ratio         = 0;

  vertices              = (pcp_vec3f_t *)self->pos;
  ndcs = (pcp_vec3f_t *)malloc(sizeof(pcp_vec3f_t) * self->num_verts);
  for (int i = 0; i < self->num_verts; i++)
  {
    ndcs[i] = pcp_vec3f_mvp_mul(vertices[i], mvp);
  }

  for (int i = 0; i < self->num_indices / 3; i++)
  {
    int idx0 = self->indices[i * 3];
    int idx1 = self->indices[i * 3 + 1];
    int idx2 = self->indices[i * 3 + 2];
    if (ndcs[idx0].z >= 0 && ndcs[idx0].z <= 1 && ndcs[idx1].z >= 0 &&
        ndcs[idx1].z <= 1 && ndcs[idx2].z >= 0 && ndcs[idx2].z <= 1 &&
        is_toward((vec2f_t){ndcs[idx0].x, ndcs[idx0].y},
                  (vec2f_t){ndcs[idx1].x, ndcs[idx1].y},
                  (vec2f_t){ndcs[idx2].x, ndcs[idx2].y}))
    {
      *screen_ratio += clipped_triangle_area(
          (vec2f_t){ndcs[idx0].x, ndcs[idx0].y},
          (vec2f_t){ndcs[idx1].x, ndcs[idx1].y},
          (vec2f_t){ndcs[idx2].x, ndcs[idx2].y});
    }
  }
  // since the screen in ndc is a 2 x 2 square
  *screen_ratio /= 4;
  free(ndcs);
  return PCPREP_RET_SUCCESS;
}
