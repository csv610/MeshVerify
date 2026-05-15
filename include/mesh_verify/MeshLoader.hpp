#ifndef MESH_LOADER_HPP
#define MESH_LOADER_HPP

#include "Mesh.hpp"
#include <string>

class MeshLoader {
public:
    static bool loadMesh(const std::string& filename, Mesh2D& mesh);
    
    /**
     * @brief Loads a surface mesh (triangles with 3D vertices).
     */
    static bool loadSurfaceMesh(const std::string& filename, SurfaceMesh& mesh);

    /**
     * @brief Loads a tetrahedral volume mesh.
     */
    static bool loadVolumeMesh(const std::string& filename, TetMesh& mesh);

    /**
     * @brief Loads a polygonal mesh for Voronoi verification.
     */
    static bool loadVoronoiCells(const std::string& filename, VoronoiMesh& mesh);

    /**
     * @brief Loads sites from a simple text file (x y per line).
     */
    static bool loadSites(const std::string& filename, std::vector<Point>& sites);
};

#endif // MESH_LOADER_HPP
