#ifndef MESH_VERIFIER_HPP
#define MESH_VERIFIER_HPP

#include "Mesh.hpp"
#include <set>
#include <utility>
#include <algorithm>
#include <map>
#include <string>
#include <span>

class MeshVerifier {
public:
    /**
     * @brief Verifies if the given 2D triangle mesh is Delaunay.
     */
    static VerificationResult isDelaunay(const Mesh2D& mesh);

    /**
     * @brief Verifies if the given polygonal mesh is a Voronoi Diagram for the given sites.
     */
    static VerificationResult isVoronoi(const VoronoiMesh& mesh);

    /**
     * @brief Verifies if the given 3D polygonal mesh is a Voronoi Diagram for the given sites.
     */
    static VerificationResult isVoronoi3D(const VoronoiMesh3D& mesh);

    /**
     * @brief Verifies if the given 3D surface mesh (triangles in 3D) is Delaunay.
     */
    static VerificationResult isDelaunaySurface(const SurfaceMesh& mesh);

    /**
     * @brief Verifies if the given 3D tetrahedral mesh is Delaunay.
     */
    static VerificationResult isDelaunay3D(const TetMesh& mesh);

    /**
     * @brief Verifies if the given 3D tetrahedral mesh is a Constrained Delaunay Triangulation.
     */
    static VerificationResult isConstrainedDelaunay3D(const TetMesh& mesh, 
                                                     std::span<const std::array<size_t, 3>> constrainedFaces);

    /**
     * @brief Verifies if the given 2D triangle mesh is a Constrained Delaunay Triangulation.
     */
    static VerificationResult isConstrainedDelaunay(const Mesh2D& mesh, 
                                                   std::span<const std::pair<size_t, size_t>> constraints);

    /**
     * @brief Verifies if the boundary of a volume mesh matches the input surface mesh.
     */
    static VerificationResult verifyBoundaryPreservation(const SurfaceMesh& surface, const TetMesh& volume);

    /**
     * @brief Verifies if the given 2D mesh is the convex hull of its points.
     */
    static VerificationResult isConvexHull(const Mesh2D& mesh);

    /**
     * @brief Verifies if the given 3D tetrahedral mesh is the convex hull of its points.
     */
    static VerificationResult isConvexHull3D(const TetMesh& mesh);

    /**
     * @brief Verifies if a 2D/Surface mesh is manifold.
     */
    static VerificationResult isManifold(const Mesh2D& mesh);
    static VerificationResult isManifold(const SurfaceMesh& mesh);

    /**
     * @brief Verifies if a tetrahedral mesh is manifold (each face shared by max 2 tets).
     */
    static VerificationResult isManifold3D(const TetMesh& mesh);

private:
    struct Face {
        std::array<size_t, 3> v;
        Face(size_t v1, size_t v2, size_t v3) {
            v = {v1, v2, v3};
            std::sort(v.begin(), v.end());
        }
        bool operator<(const Face& other) const {
            return v < other.v;
        }
        bool operator==(const Face& other) const {
            return v == other.v;
        }
    };

    static bool isLocalDelaunay(const Point& a, const Point& b, const Point& c, const Point& d);
    static bool isLocalDelaunay3D(const Point3D& a, const Point3D& b, const Point3D& c, const Point3D& d, const Point3D& e);
};

#endif // MESH_VERIFIER_HPP
