#ifndef MESH_TOPOLOGY_HPP
#define MESH_TOPOLOGY_HPP

#include "Mesh.hpp"
#include <map>

class MeshTopology {
public:
    /**
     * @brief Builds a Half-Edge structure from a 2D triangle mesh.
     */
    static HalfEdgeMesh build(const Mesh2D& mesh);

    /**
     * @brief Builds a Half-Edge structure from a 3D surface mesh.
     */
    static HalfEdgeMesh build(const SurfaceMesh& mesh);

    /**
     * @brief Builds a TetAdjacency structure from a tetrahedral mesh.
     */
    static TetAdjacency build(const TetMesh& mesh);

private:
    struct EdgeKey {
        size_t v1, v2;
        EdgeKey(size_t a, size_t b) {
            v1 = std::min(a, b);
            v2 = std::max(a, b);
        }
        bool operator<(const EdgeKey& other) const {
            if (v1 != other.v1) return v1 < other.v1;
            return v2 < other.v2;
        }
    };

    struct FaceKey {
        std::array<size_t, 3> v;
        FaceKey(size_t a, size_t b, size_t c) {
            v = {a, b, c};
            std::sort(v.begin(), v.end());
        }
        bool operator<(const FaceKey& other) const { return v < other.v; }
    };
};

#endif // MESH_TOPOLOGY_HPP
