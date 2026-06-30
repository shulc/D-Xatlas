/// D bindings for the D-Xatlas C shim.
///
/// Mirrors include/xatlas_c.h exactly — one opaque struct per handle,
/// one extern(C) declaration per entry point. Higher-level wrappers
/// (RAII handles, range-based vertex iteration) belong in a sibling
/// module so this raw surface stays available for performance-sensitive
/// callers that manage lifetimes manually.
///
/// UV convention: UVs returned via xtl_result_copy are in atlas-pixel
/// space (0 .. atlas_width / atlas_height). Divide by atlas dimensions
/// to get normalized [0,1] texture coordinates.
module xatlas.c;

extern (C) @nogc nothrow:

/// Opaque atlas context. Created by xtl_create, freed by xtl_destroy.
struct xtl_atlas;
alias xtl_atlas_t = xtl_atlas;

/// Create an empty atlas context.
xtl_atlas_t* xtl_create();

/// Add a triangle mesh to the atlas.
/// positions       — tightly-packed XYZ floats (3 per vertex)
/// vertex_count    — number of input vertices
/// indices         — uint32 triangle indices (3 per triangle)
/// index_count     — number of indices (must be a multiple of 3)
/// normals_or_null — optional XYZ normals (3 per vertex), used as charter hint
/// uvs_or_null     — optional UV hint (2 per vertex)
/// Returns 0 on success, non-zero on error.
int xtl_add_mesh(xtl_atlas_t* atlas,
                 const float* positions,
                 int vertex_count,
                 const uint* indices,
                 int index_count,
                 const float* normals_or_null,
                 const float* uvs_or_null);

/// Compute charts and pack them with default options (call after all xtl_add_mesh calls).
void xtl_generate(xtl_atlas_t* atlas);

/// Number of meshes in the atlas (equals the number of xtl_add_mesh calls).
int xtl_mesh_count(const xtl_atlas_t* atlas);

/// Number of output vertices for mesh i (valid after xtl_generate).
int xtl_result_vertex_count(const xtl_atlas_t* atlas, int mesh);

/// Number of output indices for mesh i (valid after xtl_generate).
int xtl_result_index_count(const xtl_atlas_t* atlas, int mesh);

/// Copy result arrays for mesh i.
///   uv_out    — float[2 * vertex_count], atlas-pixel UV; divide by
///               atlas width/height for normalized [0,1] coordinates
///   xref_out  — uint[vertex_count], original vertex index per output vertex
///   index_out — uint[index_count], output triangle index buffer
void xtl_result_copy(const xtl_atlas_t* atlas,
                     int mesh,
                     float* uv_out,
                     uint* xref_out,
                     uint* index_out);

/// Atlas dimensions in texels after xtl_generate.
void xtl_atlas_size(const xtl_atlas_t* atlas, int* width, int* height);

/// Free the atlas and all internally owned memory.
void xtl_destroy(xtl_atlas_t* atlas);
