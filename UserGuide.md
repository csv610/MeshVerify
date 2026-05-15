# MeshVerify User Guide

This guide provides in-depth information on using the MeshVerify library and command-line tool.

## Table of Contents
1. [Command Line Interface](#command-line-interface)
2. [Mathematical Definitions](#mathematical-definitions)
3. [C++ API Reference](#cpp-api-reference)
4. [Python API Reference](#python-api-reference)
5. [Numerical Robustness](#numerical-robustness)
6. [Topological Sanity](#topological-sanity)

---

## Command Line Interface

The `mesh_verifier` executable supports several modes of operation.

### General Usage
```bash
./mesh_verifier <mesh_file> [options]
```

### Options
- `--surface`: Verify a 3D surface mesh (triangles in 3D space) instead of a planar 2D mesh.
- `--voronoi`: Verify if the input represents a Voronoi diagram. Requires `--sites`.
- `--sites <file>`: Specify the generator points (sites) for Voronoi verification.
- `--verify-boundary <surface_file>`: Verify that the boundary of the volume mesh exactly matches the provided surface mesh.

---

## Mathematical Definitions

### Delaunay Property
- **2D/3D:** A mesh is Delaunay if the circumcircle (2D) or circumsphere (3D) of every element contains no other vertices of the mesh in its interior.
- **Surface:** For surface meshes, we use the **Angle-Sum Criterion**. An interior edge is locally Delaunay if the sum of the angles opposite to the edge in the two incident triangles is $\le \pi$.

### Constrained Delaunay (CDT)
A CDT is a triangulation that incorporates fixed "constraint" edges (2D) or faces (3D). It is Delaunay in the sense that non-constraint facets must still satisfy the local Delaunay condition.

### Voronoi Diagram
A polygonal (2D) or polyhedral (3D) partition where each cell contains all points closer to its generator site than to any other site. MeshVerify checks:
1. Every vertex is equidistant to all sites sharing that vertex.
2. Every shared edge/face lies on the perpendicular bisector of the two incident sites.

---

## C++ API Reference

### Core Classes
- `Mesh2D`, `SurfaceMesh`, `TetMesh`: Standard containers for vertex and connectivity data.
- `MeshVerifier`: Static methods for all verification logic.
- `MeshTopology`: Internal engine that builds Half-Edge or Adjacency structures for $O(1)$ navigation.

### Example: Custom Constraint Verification
```cpp
#include "MeshVerifier.hpp"

Mesh2D myMesh = ...;
std::vector<std::pair<size_t, size_t>> constraints = {{0, 5}, {2, 8}};

VerificationResult res = MeshVerifier::isConstrainedDelaunay(myMesh, constraints);
if (!res.success) {
    std::cout << "Error: " << res.message << std::endl;
}
```

---

## Python API Reference

The Python module `mesh_verify_py` provides a 1-to-1 mapping of the C++ API.

### Key Classes
- `mv.Mesh2D()`, `mv.TetMesh()`, `mv.SurfaceMesh()`
- `mv.Point(x, y)`, `mv.Point3D(x, y, z)`
- `mv.Triangle([v1, v2, v3])`, `mv.Tetrahedron([v1, v2, v3, v4])`

---

## Numerical Robustness

MeshVerify uses **Exact Predicates** to prevent failures caused by floating-point precision. This is critical for:
- Points that are nearly collinear.
- Four points that are nearly cospherical.
- Extremely thin "sliver" elements.

The logic implements **Adaptive Precision Arithmetic** (Shewchuk-inspired), ensuring that the sign of every geometric determinant is 100% correct.

---

## Topological Sanity

Before running geometric tests, MeshVerify can validate the **Manifoldness** of your mesh:
- **Edge Manifold:** No more than two triangles share any edge.
- **Face Manifold (3D):** No more than two tetrahedra share any face.
- **Boundary Preservation:** Ensuring no "holes" or new facets were introduced during volume mesh generation.
