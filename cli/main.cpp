#include "mesh_verify/MeshVerifier.hpp"
#include "mesh_verify/MeshLoader.hpp"
#include <iostream>
#include <vector>
#include <cassert>

void runTests() {
    // Case 1: Simple Delaunay (Square)
    Mesh2D mesh1;
    mesh1.vertices = {{0,0}, {1,0}, {1,1}, {0,1}};
    mesh1.triangles = {{{0, 1, 2}}, {{0, 2, 3}}};
    VerificationResult result1 = MeshVerifier::isDelaunay(mesh1);
    std::cout << "Test 1 (Square): " << (result1 ? "PASSED" : "FAILED") << std::endl;
    assert(result1.success == true);

    // Case 2: Non-Delaunay
    Mesh2D mesh2;
    mesh2.vertices = {{0,2}, {2,2}, {2,0}, {1,1}};
    mesh2.triangles = {{{0, 1, 2}}, {{0, 2, 3}}};
    VerificationResult result2 = MeshVerifier::isDelaunay(mesh2);
    std::cout << "Test 2 (Non-Delaunay): " << (!result2 ? "PASSED" : "FAILED") << std::endl;
    assert(result2.success == false);

    // Case 3: Constrained Delaunay
    std::vector<std::pair<size_t, size_t>> constraints = {{0, 2}};
    VerificationResult result3 = MeshVerifier::isConstrainedDelaunay(mesh2, constraints);
    std::cout << "Test 3 (Constrained Delaunay): " << (result3 ? "PASSED" : "FAILED") << std::endl;
    assert(result3.success == true);

    // Case 4: 3D Delaunay
    TetMesh tetMesh4;
    tetMesh4.vertices = {{0,0,0}, {1,0,0}, {0,1,0}, {0,0,1}, {1,1,1}};
    tetMesh4.tets = {{{0, 1, 2, 3}}, {{1, 2, 3, 4}}};
    VerificationResult result4 = MeshVerifier::isDelaunay3D(tetMesh4);
    std::cout << "Test 4 (3D Delaunay): " << (result4 ? "PASSED" : "FAILED") << std::endl;
    assert(result4.success == true);

    // Case 5: 3D Constrained Delaunay
    TetMesh tetMesh5;
    tetMesh5.vertices = {{0,0,0}, {1,0,0}, {0,1,0}, {0,0,1}, {0.1, 0.1, 0.1}};
    tetMesh5.tets = {{{0, 1, 2, 3}}, {{1, 2, 3, 4}}};
    std::vector<std::array<size_t, 3>> cFaces = {{{1, 2, 3}}};
    VerificationResult result5_d = MeshVerifier::isDelaunay3D(tetMesh5);
    VerificationResult result5_cdt = MeshVerifier::isConstrainedDelaunay3D(tetMesh5, cFaces);
    std::cout << "Test 5 (3D CDT): " << (result5_cdt && !result5_d ? "PASSED" : "FAILED") << std::endl;
    assert(result5_cdt.success == true && result5_d.success == false);

    // Case 6: Voronoi
    VoronoiMesh vMesh;
    vMesh.vertices = {{1, 1}, {1, -1}};
    vMesh.sites = {{0, 0}, {2, 0}};
    vMesh.cells = {{0, {0, 1}}, {1, {0, 1}}};
    VerificationResult result6 = MeshVerifier::isVoronoi(vMesh);
    std::cout << "Test 6 (Voronoi): " << (result6 ? "PASSED" : "FAILED") << std::endl;
    assert(result6.success == true);

    // Case 7: 3D Voronoi
    VoronoiMesh3D vMesh3D;
    vMesh3D.vertices = {{1, 1, 1}, {1, -1, 1}, {1, -1, -1}, {1, 1, -1}};
    vMesh3D.sites = {{0, 0, 0}, {2, 0, 0}};
    vMesh3D.cells = {{0, {{0, 1, 2, 3}}}, {1, {{0, 1, 2, 3}}}};
    VerificationResult result7 = MeshVerifier::isVoronoi3D(vMesh3D);
    std::cout << "Test 7 (3D Voronoi): " << (result7 ? "PASSED" : "FAILED") << std::endl;
    assert(result7.success == true);

    // Case 8: Surface Mesh Delaunay
    SurfaceMesh sMesh8;
    sMesh8.vertices = {{0,0,0}, {1,0,0}, {0,1,0}, {0.5, 0.5, 1.0}};
    sMesh8.triangles = {{{0, 1, 2}}, {{0, 1, 3}}, {{1, 2, 3}}, {{2, 0, 3}}};
    VerificationResult result8 = MeshVerifier::isDelaunaySurface(sMesh8);
    std::cout << "Test 8 (Surface Delaunay): " << (result8 ? "PASSED" : "FAILED") << std::endl;
    assert(result8.success == true);

    // Case 9: Boundary Preservation
    SurfaceMesh sMesh9;
    sMesh9.vertices = {{0,0,0}, {1,0,0}, {0,1,0}, {0,0,1}};
    sMesh9.triangles = {{{0, 1, 2}}, {{0, 1, 3}}, {{0, 2, 3}}, {{1, 2, 3}}};
    TetMesh vMesh9;
    vMesh9.vertices = {{0,0,0}, {1,0,0}, {0,1,0}, {0,0,1}};
    vMesh9.tets = {{{0, 1, 2, 3}}};
    VerificationResult result9 = MeshVerifier::verifyBoundaryPreservation(sMesh9, vMesh9);
    std::cout << "Test 9 (Boundary Preservation): " << (result9 ? "PASSED" : "FAILED") << std::endl;
    assert(result9.success == true);

    // Case 10: Convex Hull 2D
    Mesh2D cMesh10;
    cMesh10.vertices = {{0,0}, {1,0}, {0,1}, {0.1, 0.1}};
    cMesh10.triangles = {{{0, 1, 3}}, {{1, 2, 3}}, {{2, 0, 3}}};
    VerificationResult res10 = MeshVerifier::isConvexHull(cMesh10);
    std::cout << "Test 10 (Convex Hull 2D): " << (res10 ? "PASSED" : "FAILED") << std::endl;
    assert(res10.success == true);

    // Case 11: Convex Hull 3D
    TetMesh cMesh11;
    cMesh11.vertices = {{0,0,0}, {1,0,0}, {0,1,0}, {0,0,1}, {0.1, 0.1, 0.1}};
    cMesh11.tets = {{{0, 1, 2, 4}}, {{0, 1, 3, 4}}, {{1, 2, 3, 4}}, {{2, 0, 3, 4}}};
    VerificationResult res11 = MeshVerifier::isConvexHull3D(cMesh11);
    std::cout << "Test 11 (Convex Hull 3D): " << (res11 ? "PASSED" : "FAILED") << std::endl;
    assert(res11.success == true);

    std::cout << "All internal tests passed!" << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <mesh_file> [--voronoi --sites <sites_file> | --surface | --verify-boundary <surface_file>]" << std::endl;
        std::cout << "Running internal tests instead..." << std::endl;
        runTests();
        return 0;
    }

    std::string filename = argv[1];
    bool voronoiMode = false;
    bool surfaceMode = false;
    bool boundaryMode = false;
    std::string sitesFile = "";
    std::string surfaceFile = "";

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--voronoi") voronoiMode = true;
        else if (arg == "--surface") surfaceMode = true;
        else if (arg == "--verify-boundary" && i + 1 < argc) {
            boundaryMode = true;
            surfaceFile = argv[++i];
        }
        else if (arg == "--sites" && i + 1 < argc) {
            sitesFile = argv[++i];
        }
    }

    if (voronoiMode) {
        VoronoiMesh mesh;
        if (!MeshLoader::loadVoronoiCells(filename, mesh)) return 1;
        if (!sitesFile.empty() && !MeshLoader::loadSites(sitesFile, mesh.sites)) return 1;
        if (mesh.sites.empty()) { std::cerr << "Sites required." << std::endl; return 1; }
        VerificationResult result = MeshVerifier::isVoronoi(mesh);
        std::cout << "Voronoi is " << (result ? "VALID" : "INVALID") << std::endl;
    } else if (boundaryMode) {
        SurfaceMesh sMesh;
        TetMesh vMesh;
        if (!MeshLoader::loadSurfaceMesh(surfaceFile, sMesh)) return 1;
        if (!MeshLoader::loadVolumeMesh(filename, vMesh)) return 1;
        VerificationResult result = MeshVerifier::verifyBoundaryPreservation(sMesh, vMesh);
        std::cout << "Boundary is " << (result ? "PRESERVED" : "NOT preserved") << std::endl;
    } else if (surfaceMode) {
        SurfaceMesh mesh;
        if (!MeshLoader::loadSurfaceMesh(filename, mesh)) return 1;
        VerificationResult result = MeshVerifier::isDelaunaySurface(mesh);
        std::cout << "Surface is " << (result ? "DELAUNAY" : "NOT DELAUNAY") << std::endl;
    } else {
        Mesh2D mesh;
        if (!MeshLoader::loadMesh(filename, mesh)) return 1;
        VerificationResult result = MeshVerifier::isDelaunay(mesh);
        std::cout << "Mesh is " << (result ? "DELAUNAY" : "NOT DELAUNAY") << std::endl;
    }

    return 0;
}
