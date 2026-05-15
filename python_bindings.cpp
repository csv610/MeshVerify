#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/pair.h>
#include "MeshVerifier.hpp"

namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(mesh_verify_py, m) {
    m.doc() = "MeshVerify Python bindings using nanobind";

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

    nb::class_<VerificationResult>(m, "VerificationResult")
        .def_ro("success", &VerificationResult::success)
        .def_ro("message", &VerificationResult::message)
        .def_ro("failing_elements", &VerificationResult::failingElements)
        .def("__bool__", [](const VerificationResult &res) { return res.success; });

    nb::class_<Mesh2D>(m, "Mesh2D")
        .def(nb::init<>())
        .def_rw("vertices", &Mesh2D::vertices)
        .def_rw("triangles", &Mesh2D::triangles);

    nb::class_<SurfaceMesh>(m, "SurfaceMesh")
        .def(nb::init<>())
        .def_rw("vertices", &SurfaceMesh::vertices)
        .def_rw("triangles", &SurfaceMesh::triangles);

    nb::class_<TetMesh>(m, "TetMesh")
        .def(nb::init<>())
        .def_rw("vertices", &TetMesh::vertices)
        .def_rw("tets", &TetMesh::tets);

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
