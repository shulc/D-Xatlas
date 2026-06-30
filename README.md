# D-Xatlas

D bindings for **[jpcy/xatlas](https://github.com/jpcy/xatlas)** — UV atlas
unwrapping and chart packing for 3D polygon meshes.

A thin C++ shim (`csrc/xatlas_c.cpp`) wraps the xatlas C++ API behind an
opaque `xtl_atlas_t*` handle with plain `extern "C"` linkage, so D code
(and any C caller) can drive atlas generation without touching C++ types.
Everything is compiled into a single static archive — no shared library, no
runtime dependency.

## Pinned upstream

xatlas has **no release tags**. The submodule is pinned to commit:

```
f700c7790aaa030e794b52ba7791a05c085faf0c  (master, 2022-04-11)
```

Verification: after `git submodule add`, the parent index entry was inspected
with `git ls-files --stage extern/xatlas` and confirmed to match
`git -C extern/xatlas rev-parse HEAD` — both show `f700c779...`.

## Layout

```
include/xatlas_c.h                 — C API (extern "C", xtl_* prefix)
csrc/xatlas_c.cpp                  — shim implementation, wraps xatlas C++ API
source/xatlas/c.d                  — D extern(C) bindings, 1-to-1 with xatlas_c.h
CMakeLists.txt                     — builds libxatlas_c.a (xatlas.cpp + xatlas_c.cpp)
extern/xatlas                      — git submodule, pinned to f700c779
examples/unwrap.d                  — smoke test: cube mesh → atlas, asserts + prints results
```

## First build

```sh
git submodule update --init
dub build
```

The `preBuildCommands-posix` hook in `dub.json` runs CMake + builds
`build/libxatlas_c.a` before dub compiles the D side, so a plain
`dub build` is enough.

## Run the example

```sh
dub build --config=unwrap
./unwrap
```

Expected output (sizes may vary with xatlas version / options):

```
atlas size       : 64 x 64 texels
mesh count       : 1
result vertices  : <N>  (input: 8)
result indices   : 36   (input: 36)
all UVs in [0,1] : OK
all xrefs valid  : OK
OK
```

## UV convention

`xtl_result_copy` writes UVs in **atlas-pixel space** (range `0..width` /
`0..height`). Divide by the atlas dimensions to get normalized `[0,1]`
texture coordinates:

```d
int w, h;
xtl_atlas_size(atlas, &w, &h);
float u_norm = uv[2*i+0] / w;
float v_norm = uv[2*i+1] / h;
```

## Static linking

`lflags` in `dub.json` point at `build/libxatlas_c.a`, which contains both
the shim and the full xatlas implementation. `libs` adds `stdc++` and `m`
(xatlas uses standard C++ math). The result binary has no `libxatlas*.so`
dependency (`ldd ./unwrap` confirms).

## License

MIT — see [LICENSE](LICENSE). Upstream xatlas is also MIT (Jonathan Young /
Thekla / NVIDIA); see `extern/xatlas/LICENSE`.
