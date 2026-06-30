// D-Xatlas — C shim implementation.
//
// Wraps xatlas::Create / AddMesh / Generate in an opaque xtl_atlas handle.
// The entire xatlas C++ implementation is compiled into the same static
// archive (xatlas_c) so no separate xatlas library is needed.

#include "xatlas_c.h"  // xtl_* declarations  (include/)
#include "xatlas.h"    // xatlas C++ namespace (extern/xatlas/source/xatlas/)

#include <cstring>

struct xtl_atlas {
    xatlas::Atlas* atlas;
};

extern "C" {

xtl_atlas_t* xtl_create(void)
{
    xtl_atlas_t* h = new xtl_atlas_t;
    h->atlas = xatlas::Create();
    return h;
}

int xtl_add_mesh(xtl_atlas_t*        h,
                 const float*        positions,
                 int                 vertex_count,
                 const unsigned int* indices,
                 int                 index_count,
                 const float*        normals_or_null,
                 const float*        uvs_or_null)
{
    if (!h || !h->atlas) return -1;

    xatlas::MeshDecl decl;
    decl.vertexPositionData   = positions;
    decl.vertexPositionStride = 3 * sizeof(float);
    decl.vertexCount          = static_cast<unsigned>(vertex_count);

    if (normals_or_null) {
        decl.vertexNormalData   = normals_or_null;
        decl.vertexNormalStride = 3 * sizeof(float);
    }
    if (uvs_or_null) {
        decl.vertexUvData   = uvs_or_null;
        decl.vertexUvStride = 2 * sizeof(float);
    }

    decl.indexData   = indices;
    decl.indexCount  = static_cast<unsigned>(index_count);
    decl.indexFormat = xatlas::IndexFormat::UInt32;

    xatlas::AddMeshError err = xatlas::AddMesh(h->atlas, decl);
    return (err == xatlas::AddMeshError::Success) ? 0 : static_cast<int>(err);
}

void xtl_generate(xtl_atlas_t* h)
{
    if (h && h->atlas)
        xatlas::Generate(h->atlas);
}

int xtl_mesh_count(const xtl_atlas_t* h)
{
    if (!h || !h->atlas) return 0;
    return static_cast<int>(h->atlas->meshCount);
}

int xtl_result_vertex_count(const xtl_atlas_t* h, int mesh)
{
    if (!h || !h->atlas) return 0;
    if (mesh < 0 || static_cast<unsigned>(mesh) >= h->atlas->meshCount) return 0;
    return static_cast<int>(h->atlas->meshes[mesh].vertexCount);
}

int xtl_result_index_count(const xtl_atlas_t* h, int mesh)
{
    if (!h || !h->atlas) return 0;
    if (mesh < 0 || static_cast<unsigned>(mesh) >= h->atlas->meshCount) return 0;
    return static_cast<int>(h->atlas->meshes[mesh].indexCount);
}

void xtl_result_copy(const xtl_atlas_t* h,
                     int           mesh,
                     float*        uv_out,
                     unsigned int* xref_out,
                     unsigned int* index_out)
{
    if (!h || !h->atlas) return;
    if (mesh < 0 || static_cast<unsigned>(mesh) >= h->atlas->meshCount) return;

    const xatlas::Mesh& m = h->atlas->meshes[mesh];

    if (uv_out) {
        for (unsigned i = 0; i < m.vertexCount; i++) {
            uv_out[2 * i + 0] = m.vertexArray[i].uv[0];
            uv_out[2 * i + 1] = m.vertexArray[i].uv[1];
        }
    }
    if (xref_out) {
        for (unsigned i = 0; i < m.vertexCount; i++)
            xref_out[i] = m.vertexArray[i].xref;
    }
    if (index_out) {
        for (unsigned i = 0; i < m.indexCount; i++)
            index_out[i] = m.indexArray[i];
    }
}

void xtl_atlas_size(const xtl_atlas_t* h, int* width, int* height)
{
    if (!h || !h->atlas) {
        if (width)  *width  = 0;
        if (height) *height = 0;
        return;
    }
    if (width)  *width  = static_cast<int>(h->atlas->width);
    if (height) *height = static_cast<int>(h->atlas->height);
}

void xtl_destroy(xtl_atlas_t* h)
{
    if (!h) return;
    xatlas::Destroy(h->atlas);
    delete h;
}

} // extern "C"
