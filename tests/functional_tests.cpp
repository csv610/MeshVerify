#include <catch2/catch_test_macros.hpp>
#include "MeshVerifier.hpp"

TEST_CASE("MeshVerifier - isDelaunay", "[functional][verifier]") {
    Mesh2D mesh;
    mesh.vertices = {{0,0}, {1,0}, {1,1}, {0,1}};
    mesh.triangles = {{{0, 1, 2}}, {{0, 2, 3}}};
    CHECK(MeshVerifier::isDelaunay(mesh).success);

    // Non-Delaunay case
    Mesh2D mesh2;
    mesh2.vertices = {{0,2}, {2,2}, {2,0}, {1,1}};
    mesh2.triangles = {{{0, 1, 2}}, {{0, 2, 3}}};
    CHECK_FALSE(MeshVerifier::isDelaunay(mesh2).success);
}

TEST_CASE("MeshVerifier - isManifold", "[functional][verifier]") {
    Mesh2D mesh;
    mesh.vertices = {{0,0}, {1,0}, {0,1}, {1,1}, {0.5, 0.5}};
    // Non-manifold: three triangles sharing edge (0,4)
    mesh.triangles = {{{0, 1, 4}}, {{0, 2, 4}}, {{0, 3, 4}}};
    CHECK_FALSE(MeshVerifier::isManifold(mesh).success);
}

TEST_CASE("MeshVerifier - isConvexHull", "[functional][verifier]") {
    Mesh2D mesh;
    mesh.vertices = {{0,0}, {1,0}, {0,1}, {0.1, 0.1}};
    mesh.triangles = {{{0, 1, 3}}, {{1, 2, 3}}, {{2, 0, 3}}};
    CHECK(MeshVerifier::isConvexHull(mesh).success);
    
    // Add a point outside
    mesh.vertices.push_back({2, 2});
    CHECK_FALSE(MeshVerifier::isConvexHull(mesh).success);
}
