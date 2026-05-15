#include <catch2/catch_test_macros.hpp>
#include "mesh_verify/MeshLoader.hpp"
#include "mesh_verify/MeshVerifier.hpp"
#include <fstream>

TEST_CASE("Integration - Load and Verify OBJ", "[integration]") {
    // Create a temporary OBJ file
    const std::string filename = "temp_test.obj";
    std::ofstream out(filename);
    out << "v 0.0 0.0 0.0\n"
        << "v 1.0 0.0 0.0\n"
        << "v 0.0 1.0 0.0\n"
        << "f 1 2 3\n";
    out.close();

    Mesh2D mesh;
    REQUIRE(MeshLoader::loadMesh(filename, mesh));
    CHECK(mesh.vertices.size() == 3);
    CHECK(mesh.triangles.size() == 1);

    auto result = MeshVerifier::isDelaunay(mesh);
    CHECK(result.success);

    std::remove(filename.c_str());
}

TEST_CASE("Integration - Voronoi with Sites File", "[integration]") {
    const std::string meshFile = "temp_voronoi.obj";
    const std::string sitesFile = "temp_sites.txt";
    
    std::ofstream outM(meshFile);
    outM << "v 1.0 1.0 0.0\nv 1.0 -1.0 0.0\nf 1 2\n"; // Minimal 1D edge in 3D file for Voronoi logic
    outM.close();

    std::ofstream outS(sitesFile);
    outS << "0.0 0.0\n2.0 0.0\n";
    outS.close();

    VoronoiMesh mesh;
    REQUIRE(MeshLoader::loadVoronoiCells(meshFile, mesh));
    REQUIRE(MeshLoader::loadSites(sitesFile, mesh.sites));

    auto result = MeshVerifier::isVoronoi(mesh);
    CHECK(result.success);

    std::remove(meshFile.c_str());
    std::remove(sitesFile.c_str());
}
