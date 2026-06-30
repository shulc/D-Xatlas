/// Smoke test — build a non-planar cube mesh in code, unwrap it through
/// xatlas, then verify the atlas result is well-formed.
///
/// Checks:
///   - mesh count == 1
///   - result vertex and index counts are non-zero
///   - atlas dimensions are positive
///   - every UV (normalized) is in [0, 1]
///   - every xref is a valid original vertex index
///   - at least one chart was created (chartCount via total vert expansion)
module unwrap;

import std.stdio  : writefln, writeln;
import std.math   : isNaN;
import xatlas.c;

// Unit cube — 8 vertices, 12 triangles (2 per face × 6 faces).
// Non-planar: vertices span all 8 corners of the unit cube, so xatlas
// must create at least one chart with non-trivial parameterization.
immutable float[24] positions = [
     0, 0, 0,   1, 0, 0,   1, 1, 0,   0, 1, 0,  // front face
     0, 0, 1,   1, 0, 1,   1, 1, 1,   0, 1, 1,  // back face
];

// 6 faces × 2 triangles × 3 vertices = 36 indices
immutable uint[36] indices = [
    // front (-Z)
    0, 1, 2,   0, 2, 3,
    // back (+Z)
    5, 4, 7,   5, 7, 6,
    // left (-X)
    4, 0, 3,   4, 3, 7,
    // right (+X)
    1, 5, 6,   1, 6, 2,
    // bottom (-Y)
    4, 5, 1,   4, 1, 0,
    // top (+Y)
    3, 2, 6,   3, 6, 7,
];

void main()
{
    xtl_atlas_t* atlas = xtl_create();
    assert(atlas !is null, "xtl_create returned null");
    scope (exit) xtl_destroy(atlas);

    int err = xtl_add_mesh(atlas,
        positions.ptr, cast(int)(positions.length / 3),
        indices.ptr,   cast(int) indices.length,
        null,  // no normals
        null); // no UV hint
    assert(err == 0, "xtl_add_mesh failed");

    xtl_generate(atlas);

    int meshCount = xtl_mesh_count(atlas);
    assert(meshCount == 1, "expected 1 output mesh");

    int vcount = xtl_result_vertex_count(atlas, 0);
    int icount = xtl_result_index_count(atlas, 0);
    assert(vcount > 0, "result vertex count must be > 0");
    assert(icount > 0, "result index count must be > 0");

    int w, h;
    xtl_atlas_size(atlas, &w, &h);
    assert(w > 0 && h > 0, "atlas dimensions must be positive");

    auto uvs   = new float[](2 * vcount);
    auto xrefs = new uint[] (vcount);
    auto tris  = new uint[] (icount);
    xtl_result_copy(atlas, 0, uvs.ptr, xrefs.ptr, tris.ptr);

    // Verify all UVs normalized to [0,1] and xrefs in bounds.
    int origVertCount = cast(int)(positions.length / 3);
    for (int i = 0; i < vcount; i++) {
        float u = uvs[2*i+0] / w;
        float v = uvs[2*i+1] / h;
        assert(!isNaN(u) && u >= 0.0f && u <= 1.0f,
               "UV.u out of [0,1]: " ~ (cast(double)u).stringof);
        assert(!isNaN(v) && v >= 0.0f && v <= 1.0f,
               "UV.v out of [0,1]: " ~ (cast(double)v).stringof);
        assert(xrefs[i] < cast(uint)origVertCount,
               "xref out of range");
    }

    writefln("atlas size       : %d x %d texels", w, h);
    writefln("mesh count       : %d", meshCount);
    writefln("result vertices  : %d  (input: %d)", vcount, origVertCount);
    writefln("result indices   : %d  (input: %d)", icount, cast(int)indices.length);
    writeln("all UVs in [0,1] : OK");
    writeln("all xrefs valid  : OK");
    writeln("OK");
}
