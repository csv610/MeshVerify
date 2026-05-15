#include <catch2/catch_test_macros.hpp>
#include "Predicates.hpp"
#include "MeshTopology.hpp"

TEST_CASE("Predicates - orient2d", "[unit][predicates]") {
    Predicates::init();
    Point a{0, 0}, b{1, 0}, c{0, 1};
    CHECK(Predicates::orient2d(a, b, c) > 0);  // CCW
    CHECK(Predicates::orient2d(a, c, b) < 0);  // CW
    CHECK(Predicates::orient2d(a, b, {0.5, 0}) == 0); // Collinear
}

TEST_CASE("Predicates - incircle", "[unit][predicates]") {
    Predicates::init();
    Point a{0, 0}, b{2, 0}, c{1, 1}, d{1, 0.5}; // d is inside
    CHECK(Predicates::incircle(a, b, c, d) > 0);
    CHECK(Predicates::incircle(a, b, c, {1, 2}) < 0); // outside
}

TEST_CASE("MeshTopology - build Mesh2D", "[unit][connectivity]") {
    Mesh2D mesh;
    mesh.vertices = {{0,0}, {1,0}, {1,1}, {0,1}};
    mesh.triangles = {{{0, 1, 2}}, {{0, 2, 3}}};
    
    auto he = MeshTopology::build(mesh);
    REQUIRE(he.edges.size() == 6);
    // Find edge (0,2) in tri 0
    size_t shared_edge = INVALID_INDEX;
    for(size_t i=0; i<he.edges.size(); ++i) {
        if(he.edges[i].vertex == 0 && he.edges[he.edges[i].next].vertex == 2) shared_edge = i;
    }
    REQUIRE(shared_edge != INVALID_INDEX);
    CHECK(he.edges[shared_edge].twin != INVALID_INDEX);
}
