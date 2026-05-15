# MeshVerify

MeshVerify is a high-performance C++20 library and command-line tool designed to verify the geometric integrity and topological properties of 2D and 3D meshes. It provides robust implementations for checking Delaunay, Constrained Delaunay, Voronoi, Manifoldness, and Convex Hull properties.

## Key Features

- **Robust Numerical Foundation**: Uses Shewchuk-inspired adaptive precision predicates for guaranteed geometric correctness.
- **High Performance**: Powered by an internal **Half-Edge Data Structure** for $O(1)$ adjacency lookups and **OpenMP** for multi-core parallelization.
- **Diagnostic Reporting**: Returns detailed failure reports, including descriptive messages and the indices of offending elements.
- **Modern Python Bindings**: Fully integrated Python API using `nanobind` for seamless research and automation workflows.
- **Broad Format Support**: Leverages **Assimp** to read standard mesh formats (.obj, .ply, .off, .stl, etc.).

## Quick Start

### 1. Build the Project
```bash
mkdir build && cd build
cmake ..
make
```
This produces the `mesh_verifier` CLI and the `mesh_verify_py` Python module.

### 2. Run Tests
```bash
./unit_tests
```

### 3. Verify a Mesh (CLI)
```bash
# Basic Delaunay check
./mesh_verifier mesh.obj

# 3D Surface check
./mesh_verifier surface.obj --surface

# Voronoi check with sites
./mesh_verifier cells.obj --voronoi --sites sites.txt
```

## Python API

```python
import mesh_verify_py as mv

mesh = mv.Mesh2D()
mesh.vertices = [mv.Point(0,0), mv.Point(1,0), mv.Point(0,1)]
mesh.triangles = [mv.Triangle([0, 1, 2])]

result = mv.MeshVerifier.is_delaunay(mesh)
if not result.success:
    print(f"Failed: {result.message}")
```

## Documentation
For detailed information on command-line flags, mathematical definitions, and API references, please see the [User Guide](UserGuide.md).

## License
MIT
