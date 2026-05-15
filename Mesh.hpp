#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <array>
#include <cstddef>
#include <limits>

const size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

struct Point { double x, y; };
struct Point3D { double x, y, z; };
struct Triangle { std::array<size_t, 3> v; };
struct Tetrahedron { std::array<size_t, 4> v; };

// Half-Edge for 2D/Surface Manifolds
struct HEEdge {
    size_t vertex = INVALID_INDEX; // Origin vertex
    size_t face = INVALID_INDEX;   // Incident face
    size_t next = INVALID_INDEX;   // Next edge in face loop
    size_t twin = INVALID_INDEX;   // Opposite edge (neighbor)
};

struct HalfEdgeMesh {
    std::vector<HEEdge> edges;
    std::vector<size_t> faceToEdge;   // One edge per face
    std::vector<size_t> vertexToEdge; // One outgoing edge per vertex
    
    // Original data for coordinates
    const std::vector<Point>* vertices2D = nullptr;
    const std::vector<Point3D>* vertices3D = nullptr;
};

// Adjacency structure for Tetrahedral Volume Meshes
struct HalfFace {
    size_t tet = INVALID_INDEX;     // Incident tetrahedron
    size_t oppositeTet = INVALID_INDEX; // Neighbor tetrahedron
};

struct TetAdjacency {
    // Maps each tet's 4 faces to their adjacency info
    // Face indices in tet i: {0,1,2}, {0,1,3}, {0,2,3}, {1,2,3}
    std::vector<std::array<size_t, 4>> neighbors; 
    const std::vector<Point3D>* vertices = nullptr;
    const std::vector<Tetrahedron>* tets = nullptr;
};

struct VoronoiCell {
    size_t siteIndex;
    std::vector<size_t> vertexIndices;
};

struct VoronoiCell3D {
    size_t siteIndex;
    std::vector<std::vector<size_t>> faces;
};

// Mesh Classes
struct Mesh2D {
    std::vector<Point> vertices;
    std::vector<Triangle> triangles;
};

struct SurfaceMesh {
    std::vector<Point3D> vertices;
    std::vector<Triangle> triangles;
};

struct TetMesh {
    std::vector<Point3D> vertices;
    std::vector<Tetrahedron> tets;
};

struct VoronoiMesh {
    std::vector<Point> vertices;
    std::vector<Point> sites;
    std::vector<VoronoiCell> cells;
};

struct VoronoiMesh3D {
    std::vector<Point3D> vertices;
    std::vector<Point3D> sites;
    std::vector<VoronoiCell3D> cells;
};

// Verification Diagnostic Information
struct VerificationResult {
    bool success = true;
    std::string message;
    std::vector<size_t> failingElements; // Indices of triangles, tets, or edges
    
    operator bool() const { return success; }
};

#endif // MESH_HPP
