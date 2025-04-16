#include <pcprep/core.h>
#include <pcprep/mesh.h>
#include <pcprep/vec3f.h>
#include <pcprep/wrapper.h>
#define MSH_PLY_INCLUDE_LIBC_HEADERS
#define MSH_PLY_IMPLEMENTATION
#include <msh_ply.h>

int mesh_init(mesh_t *mesh, uint32_t num_verts, uint32_t num_indices)
{
  if (num_verts < 0 || num_indices < 0)
    return -1;
  mesh->pos       = (float *)malloc(sizeof(float) * 3 * num_verts);
  mesh->num_verts = num_verts;

  mesh->indices = (uint32_t *)malloc(sizeof(uint32_t) * num_indices);
  mesh->num_indices = num_indices;
  return 1;
}
int mesh_free(mesh_t *mesh)
{
  if (mesh == NULL)
    return 1;
  if (mesh->pos)
  {
    free(mesh->pos);
    mesh->pos = NULL;
  }
  if (mesh->indices)
  {
    free(mesh->indices);
    mesh->indices = NULL;
  }
  return 1;
}
int mesh_load(mesh_t *mesh, const char *filename)
{
  mesh_init(
      mesh, ply_count_vertex(filename), ply_count_face(filename) * 3);
  return ply_mesh_loader(filename, mesh->pos, mesh->indices);
}
int mesh_write(mesh_t mesh, const char *filename, int binary)
{
  msh_ply_desc_t descriptors[2];
  descriptors[0] = (msh_ply_desc_t){
      .element_name   = "vertex",
      .property_names = (const char *[]){"x", "y", "z"},
      .num_properties = 3,
      .data_type      = MSH_PLY_FLOAT,
      .data           = &mesh.pos,
      .data_count     = &mesh.num_verts
  };

  uint32_t num_faces = mesh.num_indices / 3;
  descriptors[1]     = (msh_ply_desc_t){
          .element_name   = "face",
          .property_names = (const char *[]){"vertex_indices"},
          .num_properties = 1,
          .data_type      = MSH_PLY_INT32,
          .list_type      = MSH_PLY_UINT8,
          .data           = &mesh.indices,
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
}

static int is_toward(vec2f_t a, vec2f_t b, vec2f_t c)
{
  return (b.x - a.x) * (c.y - a.y) > (c.x - a.x) * (b.y - a.y);
}

int mesh_screen_ratio(mesh_t mesh, float *mvp, float *screen_ratio)
{
  *screen_ratio     = 0;

  vec3f_t *vertices = (vec3f_t *)mesh.pos;
  vec3f_t *ndcs = (vec3f_t *)malloc(sizeof(vec3f_t) * mesh.num_verts);
  for (int i = 0; i < mesh.num_verts; i++)
  {
    ndcs[i] = vec3f_mvp_mul(vertices[i], mvp);
  }

  for (int i = 0; i < mesh.num_indices / 3; i++)
  {
    int idx0 = mesh.indices[i * 3];
    int idx1 = mesh.indices[i * 3 + 1];
    int idx2 = mesh.indices[i * 3 + 2];
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
}
