#ifndef PCPREP_DEFS_H
#define PCPREP_DEFS_H

#include "pcprep/pcprep_export.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint32_t pcp_ret_t;

#define PCPREP_RET_FAIL    0x00
#define PCPREP_RET_SUCCESS 0x01

#ifdef __cplusplus
#define PCPREP_NULL nullptr
#else
#define PCPREP_NULL ((void *)0)
#endif

typedef struct pcp_aabb_t        pcp_aabb_t;
typedef struct pcp_canvas_t      pcp_canvas_t;
typedef struct pcp_mesh_t        pcp_mesh_t;
typedef struct pcp_point_cloud_t pcp_point_cloud_t;
typedef struct pcp_vec3f_t       pcp_vec3f_t;
typedef struct pcp_vec3u_t       pcp_vec3u_t;
typedef struct pcp_vec3uc_t      pcp_vec3uc_t;

#endif
