#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/pair.h>
#include "mesh_verify/MeshVerifier.hpp"

namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(mesh_verify_py, m) {
    m.doc() = "MeshVerify Python bindings - Robust Geometric Verification";

    // --- Core Primitives ---
    nb::class_<Point>(m, "Point")
        .def(nb::init<double, double>())
        .def_rw("x", &Point::x)
        .def_rw("y", &Point::y);

    nb::class_<Point3D>(m, "Point3D")
        .def(nb::init<double, double, double>())
        .def_rw("x", &Point3D::x)
        .def_rw("y", &Point3D::y)
        .def_rw("z", &Point3D::z);

    nb::class_<Triangle>(m, "Triangle")
        .def(nb::init<std::array<size_t, 3>>())
        .def_rw("v", &Triangle::v);

    nb::class_<Tetrahedron>(m, "Tetrahedron")
        .def(nb::init<std::array<size_t, 4>>())
        .def_rw("v", &Tetrahedron::v);

    // --- Results ---
    nb::class_<VerificationResult>(m, "VerificationResult")
        .def_ro("success", &VerificationResult::success)
        .def_ro("message", &VerificationResult::message)
        .def_ro("failing_elements", &VerificationResult::failingElements)
        .def("__bool__", [](const VerificationResult &res) { return res.success; })
        .def("__repr__", [](const VerificationResult &res) {
            return res.success ? "<VerificationResult: SUCCESS>" : "<VerificationResult: FAILED - " + res.message + ">";
        });

    // --- Clean Mesh Classes with Vertices ---

    nb::class_<Mesh2D>(m, "Mesh2D")
        .def(nb::init<>())
        .def("__init__", [](Mesh2D &mesh, const std::vector<std::pair<double, double>> &v, const std::vector<std::array<size_t, 3>> &f) {
            new (&mesh) Mesh2D();
            for (auto const& p : v) mesh.vertices.push_back({p.first, p.second});
            for (auto const& tri : f) mesh.triangles.push_back({tri});
        }, "vertices"_a, "triangles"_a)
        .def_rw("vertices", &Mesh2D::vertices)
        .def_rw("triangles", &Mesh2D::triangles);

    nb::class_<SurfaceMesh>(m, "SurfaceMesh")
        .def(nb::init<>())
        .def("__init__", [](SurfaceMesh &mesh, const std::vector<std::array<double, 3>> &v, const std::vector<std::array<size_t, 3>> &f) {
            new (&mesh) SurfaceMesh();
            for (auto const& p : v) mesh.vertices.push_back({p[0], p[1], p[2]});
            for (auto const& tri : f) mesh.triangles.push_back({tri});
        }, "vertices"_a, "triangles"_a)
        .def_rw("vertices", &SurfaceMesh::vertices)
        .def_rw("triangles", &SurfaceMesh::triangles);

    nb::class_<TetMesh>(m, "TetMesh")
        .def(nb::init<>())
        .def("__init__", [](TetMesh &mesh, const std::vector<std::array<double, 3>> &v, const std::vector<std::array<size_t, 4>> &f) {
            new (&mesh) TetMesh();
            for (auto const& p : v) mesh.vertices.push_back({p[0], p[1], p[2]});
            for (auto const& tet : f) mesh.tets.push_back({tet});
        }, "vertices"_a, "tets"_a)
        .def_rw("vertices", &TetMesh::vertices)
        .def_rw("tets", &TetMesh::tets);

    // --- Static Verifier Methods ---
    nb::class_<MeshVerifier>(m, "MeshVerifier")
        .def_static("is_delaunay", &MeshVerifier::isDelaunay, "mesh"_a)
        .def_static("is_delaunay_surface", &MeshVerifier::isDelaunaySurface, "mesh"_a)
        .def_static("is_delaunay_3d", &MeshVerifier::isDelaunay3D, "mesh"_a)
        .def_static("is_constrained_delaunay", &MeshVerifier::isConstrainedDelaunay, "mesh"_a, "constraints"_a)
        .def_static("is_constrained_delaunay_3d", &MeshVerifier::isConstrainedDelaunay3D, "mesh"_a, "constraints"_a)
        .def_static("verify_boundary_preservation", &MeshVerifier::verifyBoundaryPreservation, "surface"_a, "volume"_a)
        .def_static("is_manifold", nb::overload_cast<const Mesh2D&>(&MeshVerifier::isManifold), "mesh"_a)
        .def_static("is_manifold_surface", nb::overload_cast<const SurfaceMesh&>(&MeshVerifier::isManifold), "mesh"_a)
        .def_static("is_manifold_3d", &MeshVerifier::isManifold3D, "mesh"_a)
        .def_static("is_convex_hull", &MeshVerifier::isConvexHull, "mesh"_a)
        .def_static("is_convex_hull_3d", &MeshVerifier::isConvexHull3D, "mesh"_a);
}
