// D-Xatlas — simplified C shim around jpcy/xatlas.
//
// Provides an opaque-handle C API (xtl_* prefix) over xatlas::Create /
// AddMesh / Generate so D code (and any plain-C caller) can drive UV atlas
// generation without touching C++ types.
//
// UV contract: xatlas returns UVs in atlas-pixel space (0 .. width/height).
// Normalize to [0,1] by dividing uv[0] by atlas width and uv[1] by atlas
// height (both returned by xtl_atlas_size).

#pragma once
#ifndef XATLAS_C_SHIM_H
#define XATLAS_C_SHIM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xtl_atlas xtl_atlas_t;

// Create an empty atlas context.
xtl_atlas_t* xtl_create(void);

// Add a triangle mesh. Positions are tightly-packed (3 floats/vertex).
// normals_or_null and uvs_or_null may be NULL; when provided they are
// used as hints by the charter (3 floats/vertex for normals, 2 for uvs).
// Indices are uint32 triplets (one triangle per 3 indices).
// Returns 0 on success, non-zero on error.
int xtl_add_mesh(xtl_atlas_t* atlas,
                 const float*        positions,
                 int                 vertex_count,
                 const unsigned int* indices,
                 int                 index_count,
                 const float*        normals_or_null,
                 const float*        uvs_or_null);

// Compute charts and pack them with default options.
void xtl_generate(xtl_atlas_t* atlas);

// Number of meshes (equals the number of xtl_add_mesh calls).
int xtl_mesh_count(const xtl_atlas_t* atlas);

// Output vertex / index counts for mesh i (after xtl_generate).
int xtl_result_vertex_count(const xtl_atlas_t* atlas, int mesh);
int xtl_result_index_count (const xtl_atlas_t* atlas, int mesh);

// Copy result arrays for mesh i into caller-owned buffers.
//   uv_out    float[2 * vertex_count]  — atlas-pixel UV, normalize by
//             atlas width/height for [0,1] texture coordinates
//   xref_out  uint[vertex_count]       — original vertex index per output vertex
//   index_out uint[index_count]        — triangle index buffer (references uv_out / xref_out)
void xtl_result_copy(const xtl_atlas_t* atlas,
                     int          mesh,
                     float*       uv_out,
                     unsigned int* xref_out,
                     unsigned int* index_out);

// Atlas dimensions in texels (valid after xtl_generate).
void xtl_atlas_size(const xtl_atlas_t* atlas, int* width, int* height);

// Free the atlas and all xatlas-owned memory.
void xtl_destroy(xtl_atlas_t* atlas);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // XATLAS_C_SHIM_H
