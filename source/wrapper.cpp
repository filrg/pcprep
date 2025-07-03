#include "pcprep/wrapper.h"
#include "miniply/miniply.h"
#include <cstring>
#include <stdint.h>
extern bool p_vert_col_ply_loader(const char    *filename,
                                  float         *pos,
                                  unsigned char *rgb)
{
  miniply::PLYReader reader(filename);
  if (!reader.valid())
  {
    return 0;
  }

  uint32_t propIdxs[3];
  bool     gotVerts = false;

  while (reader.has_element() && !gotVerts)
  {
    if (reader.element_is(miniply::kPLYVertexElement))
    {
      if (!reader.load_element() || !reader.find_pos(propIdxs))
      {
        break;
      }

      reader.extract_properties(
          propIdxs, 3, miniply::PLYPropertyType::Float, pos);
      if (reader.find_color(propIdxs))
      {
        reader.extract_properties(
            propIdxs, 3, miniply::PLYPropertyType::UChar, rgb);
      }
      gotVerts = true;
    }
    reader.next_element();
  }
  return 1;
}

extern bool p_pos_indices_ply_loader(const char *filename,
                                     float      *pos,
                                     int        *indices)
{
  miniply::PLYReader reader(filename);
  if (!reader.valid())
  {
    return 0;
  }
  uint32_t             faceIdxs[3];
  miniply::PLYElement *faceElem = reader.get_element(
      reader.find_element(miniply::kPLYFaceElement));
  if (faceElem == nullptr)
  {
    return 0;
  }
  faceElem->convert_list_to_fixed_size(
      faceElem->find_property("vertex_indices"), 3, faceIdxs);

  uint32_t indexes[3];
  bool     gotVerts = false, gotFaces = false;

  while (reader.has_element() && (!gotVerts || !gotFaces))
  {
    if (reader.element_is(miniply::kPLYVertexElement) &&
        reader.load_element() && reader.find_pos(indexes))
    {
      reader.extract_properties(
          indexes, 3, miniply::PLYPropertyType::Float, pos);
      gotVerts = true;
    }
    else if (!gotFaces &&
             reader.element_is(miniply::kPLYFaceElement) &&
             reader.load_element())
    {
      reader.extract_properties(
          faceIdxs, 3, miniply::PLYPropertyType::Int, indices);
      gotFaces = true;
    }
    if (gotVerts && gotFaces)
    {
      break;
    }
    reader.next_element();
  }
  if (!gotVerts || !gotFaces)
  {
    return 0;
  }
  return 1;
}

extern "C"
{
  int ply_count_vertex(const char *filename)
  {
    miniply::PLYReader reader(filename);
    if (!reader.valid())
    {
      return -1;
    }
    uint32_t elemIndex =
        reader.find_element(miniply::kPLYVertexElement);
    if (elemIndex == miniply::kInvalidIndex)
      return 0;
    return reader.get_element(elemIndex)->count;
  }
  int ply_count_face(const char *filename)
  {
    miniply::PLYReader reader(filename);
    if (!reader.valid())
    {
      return -1;
    }
    uint32_t elemIndex =
        reader.find_element(miniply::kPLYFaceElement);
    if (elemIndex == miniply::kInvalidIndex)
      return 0;
    return reader.get_element(elemIndex)->count;
  }
  // Wrapper implementation
  int ply_pcp_point_cloud_loader(const char    *filename,
                            float         *pos,
                            unsigned char *rgb)
  {
    return p_vert_col_ply_loader(filename, pos, rgb) ? 1 : 0;
  }
  int ply_mesh_loader(const char *filename, float *pos, int *indices)
  {
    return p_pos_indices_ply_loader(filename, pos, indices) ? 1 : 0;
  }
}
