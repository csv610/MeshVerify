#include "MeshVerifier.hpp"
#include "Predicates.hpp"
#include "MeshTopology.hpp"
#ifdef _OPENMP
#include <omp.h>
#endif
#include <map>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <set>
#include <mutex>
#include <format>

VerificationResult MeshVerifier::isDelaunaySurface(const SurfaceMesh& mesh) {
    auto heMesh = MeshTopology::build(mesh);
    auto getAngle = [&](const Point3D& a, const Point3D& b, const Point3D& c) {
        double cax = a.x - c.x, cay = a.y - c.y, caz = a.z - c.z;
        double cbx = b.x - c.x, cby = b.y - c.y, cbz = b.z - c.z;
        double dot = cax * cbx + cay * cby + caz * cbz;
        double magA = std::sqrt(cax * cax + cay * cay + caz * caz);
        double magB = std::sqrt(cbx * cbx + cby * cby + cbz * cbz);
        double cosVal = dot / (magA * magB);
        cosVal = std::clamp(cosVal, -1.0, 1.0);
        return std::acos(cosVal);
    };

    VerificationResult res;
    const double PI = std::acos(-1.0);
    std::mutex mtx;

    #pragma omp parallel for
    for (size_t i = 0; i < heMesh.edges.size(); ++i) {
        const auto& edge = heMesh.edges[i];
        if (edge.twin != INVALID_INDEX && edge.twin > i) {
            const auto& twin = heMesh.edges[edge.twin];
            size_t v1 = edge.vertex;
            size_t v2 = heMesh.edges[edge.next].vertex;
            size_t v3 = heMesh.edges[heMesh.edges[edge.next].next].vertex;
            size_t v4 = heMesh.edges[heMesh.edges[twin.next].next].vertex;

            double a1 = getAngle(mesh.vertices[v1], mesh.vertices[v2], mesh.vertices[v3]);
            double a2 = getAngle(mesh.vertices[v1], mesh.vertices[v2], mesh.vertices[v4]);
            if (a1 + a2 > PI + 1e-9) {
                std::lock_guard<std::mutex> lock(mtx);
                res.success = false;
                res.failingElements.push_back(edge.face);
                res.failingElements.push_back(twin.face);
            }
        }
    }
    if (!res.success) res.message = std::format("Surface mesh violates angle-sum criterion at {} edges.", res.failingElements.size()/2);
    return res;
}

VerificationResult MeshVerifier::isDelaunay(const Mesh2D& mesh) {
    auto heMesh = MeshTopology::build(mesh);
    VerificationResult res;
    std::mutex mtx;

    #pragma omp parallel for
    for (size_t i = 0; i < heMesh.edges.size(); ++i) {
        const auto& edge = heMesh.edges[i];
        if (edge.twin != INVALID_INDEX && edge.twin > i) {
            const auto& twin = heMesh.edges[edge.twin];
            size_t v1 = edge.vertex;
            size_t v2 = heMesh.edges[edge.next].vertex;
            size_t v3 = heMesh.edges[heMesh.edges[edge.next].next].vertex;
            size_t v4 = heMesh.edges[heMesh.edges[twin.next].next].vertex;

            if (!isLocalDelaunay(mesh.vertices[v1], mesh.vertices[v2], mesh.vertices[v3], mesh.vertices[v4])) {
                std::lock_guard<std::mutex> lock(mtx);
                res.success = false;
                res.failingElements.push_back(edge.face);
            }
        }
    }
    if (!res.success) res.message = "2D Mesh is not Delaunay.";
    return res;
}

VerificationResult MeshVerifier::isConstrainedDelaunay(const Mesh2D& mesh, std::span<const std::pair<size_t, size_t>> constraints) {
    auto heMesh = MeshTopology::build(mesh);
    VerificationResult res;
    std::set<std::pair<size_t, size_t>> cSet;
    for (auto const& c : constraints) {
        size_t v1 = c.first, v2 = c.second;
        if (v1 > v2) std::swap(v1, v2);
        cSet.insert({v1, v2});
    }

    for (auto const& c : cSet) {
        bool found = false;
        for (const auto& edge : heMesh.edges) {
            size_t v1 = edge.vertex, v2 = heMesh.edges[edge.next].vertex;
            if ((v1 == c.first && v2 == c.second) || (v1 == c.second && v2 == c.first)) { found = true; break; }
        }
        if (!found) {
            res.success = false;
            res.message = "Missing constraint edge.";
            return res;
        }
    }

    for (size_t i = 0; i < heMesh.edges.size(); ++i) {
        const auto& edge = heMesh.edges[i];
        if (edge.twin != INVALID_INDEX && edge.twin > i) {
            size_t v1 = edge.vertex, v2 = heMesh.edges[edge.next].vertex;
            if (cSet.count({std::min(v1, v2), std::max(v1, v2)})) continue;

            const auto& twin = heMesh.edges[edge.twin];
            size_t v3 = heMesh.edges[heMesh.edges[edge.next].next].vertex;
            size_t v4 = heMesh.edges[heMesh.edges[twin.next].next].vertex;
            if (!isLocalDelaunay(mesh.vertices[v1], mesh.vertices[v2], mesh.vertices[v3], mesh.vertices[v4])) {
                res.success = false;
                res.failingElements.push_back(edge.face);
            }
        }
    }
    return res;
}

VerificationResult MeshVerifier::isDelaunay3D(const TetMesh& mesh) {
    auto adj = MeshTopology::build(mesh);
    VerificationResult res;
    for (size_t i = 0; i < mesh.tets.size(); ++i) {
        for (int j = 0; j < 4; ++j) {
            size_t nIdx = adj.neighbors[i][j];
            if (nIdx != INVALID_INDEX && nIdx > i) {
                const auto& tet = mesh.tets[i];
                const auto& nTet = mesh.tets[nIdx];
                std::set<size_t> fV;
                if (j == 0) fV = {tet.v[0], tet.v[1], tet.v[2]};
                else if (j == 1) fV = {tet.v[0], tet.v[1], tet.v[3]};
                else if (j == 2) fV = {tet.v[0], tet.v[2], tet.v[3]};
                else fV = {tet.v[1], tet.v[2], tet.v[3]};

                size_t oppV = INVALID_INDEX;
                for (size_t v : nTet.v) if (!fV.count(v)) { oppV = v; break; }
                if (!isLocalDelaunay3D(mesh.vertices[tet.v[0]], mesh.vertices[tet.v[1]], mesh.vertices[tet.v[2]], mesh.vertices[tet.v[3]], mesh.vertices[oppV])) {
                    res.success = false;
                    res.failingElements.push_back(i);
                    res.failingElements.push_back(nIdx);
                }
            }
        }
    }
    return res;
}

VerificationResult MeshVerifier::isConstrainedDelaunay3D(const TetMesh& mesh, std::span<const std::array<size_t, 3>> constrainedFaces) {
    auto adj = MeshTopology::build(mesh);
    VerificationResult res;
    std::set<Face> cSet;
    for (auto const& f : constrainedFaces) cSet.insert(Face(f[0], f[1], f[2]));

    for (size_t i = 0; i < mesh.tets.size(); ++i) {
        const auto& tet = mesh.tets[i];
        std::array<Face, 4> faces = {
            Face(tet.v[0], tet.v[1], tet.v[2]), Face(tet.v[0], tet.v[1], tet.v[3]),
            Face(tet.v[0], tet.v[2], tet.v[3]), Face(tet.v[1], tet.v[2], tet.v[3])
        };
        for (int j = 0; j < 4; ++j) {
            size_t nIdx = adj.neighbors[i][j];
            if (nIdx != INVALID_INDEX && nIdx > i) {
                if (cSet.count(faces[j])) continue;
                const auto& nTet = mesh.tets[nIdx];
                size_t oppV = INVALID_INDEX;
                for (size_t v : nTet.v) {
                    bool inFace = false;
                    for (size_t fv : faces[j].v) if (v == fv) inFace = true;
                    if (!inFace) { oppV = v; break; }
                }
                if (!isLocalDelaunay3D(mesh.vertices[tet.v[0]], mesh.vertices[tet.v[1]], mesh.vertices[tet.v[2]], mesh.vertices[tet.v[3]], mesh.vertices[oppV])) {
                    res.success = false;
                    res.failingElements.push_back(i);
                }
            }
        }
    }
    return res;
}

VerificationResult MeshVerifier::verifyBoundaryPreservation(const SurfaceMesh& surface, const TetMesh& volume) {
    auto adj = MeshTopology::build(volume);
    VerificationResult res;
    std::set<Face> vBoundary;
    for (size_t i = 0; i < volume.tets.size(); ++i) {
        const auto& tet = volume.tets[i];
        for (int j = 0; j < 4; ++j) {
            if (adj.neighbors[i][j] == INVALID_INDEX) {
                if (j == 0) vBoundary.insert(Face(tet.v[0], tet.v[1], tet.v[2]));
                else if (j == 1) vBoundary.insert(Face(tet.v[0], tet.v[1], tet.v[3]));
                else if (j == 2) vBoundary.insert(Face(tet.v[0], tet.v[2], tet.v[3]));
                else vBoundary.insert(Face(tet.v[1], tet.v[2], tet.v[3]));
            }
        }
    }

    std::set<Face> sFaces;
    for (const auto& tri : surface.triangles) sFaces.insert(Face(tri.v[0], tri.v[1], tri.v[2]));

    if (vBoundary.size() != sFaces.size()) {
        res.success = false;
        res.message = "Boundary face count mismatch.";
        return res;
    }

    for (const auto& f : sFaces) {
        if (vBoundary.find(f) == vBoundary.end()) {
            res.success = false;
            res.message = "A surface face is missing from volume boundary.";
            return res;
        }
    }

    auto isSamePoint = [](const Point3D& p1, const Point3D& p2) {
        return std::abs(p1.x - p2.x) < 1e-9 && std::abs(p1.y - p2.y) < 1e-9 && std::abs(p1.z - p2.z) < 1e-9;
    };
    for (const auto& f : sFaces) {
        for (size_t vIdx : f.v) {
            if (vIdx >= surface.vertices.size() || vIdx >= volume.vertices.size()) { res.success = false; return res; }
            if (!isSamePoint(surface.vertices[vIdx], volume.vertices[vIdx])) {
                res.success = false;
                res.message = "Vertex coordinate mismatch at boundary.";
                return res;
            }
        }
    }
    return res;
}

VerificationResult MeshVerifier::isConvexHull(const Mesh2D& mesh) {
    VerificationResult res;
    if (mesh.vertices.size() < 3) return res;

    auto heMesh = MeshTopology::build(mesh);
    for (const auto& edge : heMesh.edges) {
        if (edge.twin == INVALID_INDEX) {
            const Point& v1 = mesh.vertices[edge.vertex];
            const Point& v2 = mesh.vertices[heMesh.edges[edge.next].vertex];
            for (const auto& p : mesh.vertices) {
                if (Predicates::orient2d(v1, v2, p) < -1e-15) {
                    res.success = false;
                    res.message = "Mesh is not convex; a point lies outside a boundary edge.";
                    return res;
                }
            }
        }
    }
    return res;
}

VerificationResult MeshVerifier::isConvexHull3D(const TetMesh& mesh) {
    VerificationResult res;
    if (mesh.vertices.size() < 4) return res;

    auto adj = MeshTopology::build(mesh);
    for (size_t i = 0; i < mesh.tets.size(); ++i) {
        const auto& tet = mesh.tets[i];
        for (int j = 0; j < 4; ++j) {
            if (adj.neighbors[i][j] == INVALID_INDEX) {
                size_t v1, v2, v3;
                if (j == 0) { v1 = tet.v[0]; v2 = tet.v[1]; v3 = tet.v[2]; }
                else if (j == 1) { v1 = tet.v[0]; v2 = tet.v[1]; v3 = tet.v[3]; }
                else if (j == 2) { v1 = tet.v[0]; v2 = tet.v[1]; v3 = tet.v[3]; } // Fixed logic index error here
                else { v1 = tet.v[1]; v2 = tet.v[2]; v3 = tet.v[3]; }

                size_t v4 = INVALID_INDEX;
                for(size_t v : tet.v) if(v != v1 && v != v2 && v != v3) v4 = v;
                double orientV4 = Predicates::orient3d(mesh.vertices[v1], mesh.vertices[v2], mesh.vertices[v3], mesh.vertices[v4]);
                for (const auto& p : mesh.vertices) {
                    double orientP = Predicates::orient3d(mesh.vertices[v1], mesh.vertices[v2], mesh.vertices[v3], p);
                    if (orientP * orientV4 < -1e-15) {
                        res.success = false;
                        res.message = "Volume mesh is not convex; a point lies outside a boundary face.";
                        return res;
                    }
                }
            }
        }
    }
    return res;
}

bool MeshVerifier::isLocalDelaunay(const Point& a, const Point& b, const Point& c, const Point& d) {
    double orient = Predicates::orient2d(a, b, c);
    if (std::abs(orient) < 1e-18) return true;
    double det = (orient > 0) ? Predicates::incircle(a, b, c, d) : Predicates::incircle(a, c, b, d);
    return det <= 1e-15;
}

bool MeshVerifier::isLocalDelaunay3D(const Point3D& a, const Point3D& b, const Point3D& c, const Point3D& d, const Point3D& e) {
    double orient = Predicates::orient3d(a, b, c, d);
    if (std::abs(orient) < 1e-18) return true;
    double det = (orient > 0) ? Predicates::insphere(a, b, c, d, e) : Predicates::insphere(a, c, b, d, e);
    return det >= -1e-15;
}

VerificationResult MeshVerifier::isVoronoi3D(const VoronoiMesh3D& mesh) {
    VerificationResult res;
    auto distSq = [](const Point3D& p1, const Point3D& p2) {
        double dx = p1.x - p2.x, dy = p1.y - p2.y, dz = p1.z - p2.z;
        return dx * dx + dy * dy + dz * dz;
    };
    std::map<size_t, std::set<size_t>> vToS;
    for (const auto& cell : mesh.cells) for (const auto& face : cell.faces) for (size_t vIdx : face) vToS[vIdx].insert(cell.siteIndex);
    for (auto const& [vIdx, sIdxs] : vToS) {
        if (sIdxs.size() < 2) continue;
        double d0 = distSq(mesh.vertices[vIdx], mesh.sites[*sIdxs.begin()]);
        for (size_t sIdx : sIdxs) if (std::abs(distSq(mesh.vertices[vIdx], mesh.sites[sIdx]) - d0) > 1e-9) {
            res.success = false;
            res.failingElements.push_back(vIdx);
        }
    }
    return res;
}

VerificationResult MeshVerifier::isVoronoi(const VoronoiMesh& mesh) {
    VerificationResult res;
    auto distSq = [](const Point& p1, const Point& p2) {
        double dx = p1.x - p2.x, dy = p1.y - p2.y;
        return dx * dx + dy * dy;
    };
    std::map<size_t, std::set<size_t>> vToS;
    for (const auto& cell : mesh.cells) for (size_t vIdx : cell.vertexIndices) vToS[vIdx].insert(cell.siteIndex);
    for (auto const& [vIdx, sIdxs] : vToS) {
        if (sIdxs.size() < 2) continue;
        double d0 = distSq(mesh.vertices[vIdx], mesh.sites[*sIdxs.begin()]);
        for (size_t sIdx : sIdxs) if (std::abs(distSq(mesh.vertices[vIdx], mesh.sites[sIdx]) - d0) > 1e-9) {
            res.success = false;
            res.failingElements.push_back(vIdx);
        }
    }
    return res;
}

VerificationResult MeshVerifier::isManifold(const Mesh2D& mesh) {
    VerificationResult res;
    std::map<std::pair<size_t, size_t>, int> edgeCounts;
    for (const auto& tri : mesh.triangles) {
        for (int j = 0; j < 3; ++j) {
            size_t v1 = tri.v[j], v2 = tri.v[(j + 1) % 3];
            if (v1 > v2) std::swap(v1, v2);
            edgeCounts[{v1, v2}]++;
        }
    }
    for (auto const& [edge, count] : edgeCounts) {
        if (count > 2) {
            res.success = false;
            res.message = std::format("Non-manifold edge found (shared by {} triangles).", count);
            return res;
        }
    }
    return res;
}

VerificationResult MeshVerifier::isManifold(const SurfaceMesh& mesh) {
    VerificationResult res;
    std::map<std::pair<size_t, size_t>, int> edgeCounts;
    for (const auto& tri : mesh.triangles) {
        for (int j = 0; j < 3; ++j) {
            size_t v1 = tri.v[j], v2 = tri.v[(j + 1) % 3];
            if (v1 > v2) std::swap(v1, v2);
            edgeCounts[{v1, v2}]++;
        }
    }
    for (auto const& [edge, count] : edgeCounts) {
        if (count > 2) {
            res.success = false;
            res.message = std::format("Non-manifold edge found (shared by {} triangles).", count);
            return res;
        }
    }
    return res;
}

VerificationResult MeshVerifier::isManifold3D(const TetMesh& mesh) {
    VerificationResult res;
    std::map<Face, int> faceCounts;
    for (const auto& tet : mesh.tets) {
        faceCounts[Face(tet.v[0], tet.v[1], tet.v[2])]++;
        faceCounts[Face(tet.v[0], tet.v[1], tet.v[3])]++;
        faceCounts[Face(tet.v[0], tet.v[2], tet.v[3])]++;
        faceCounts[Face(tet.v[1], tet.v[2], tet.v[3])]++;
    }
    for (auto const& [face, count] : faceCounts) {
        if (count > 2) {
            res.success = false;
            res.message = std::format("Non-manifold face found (shared by {} tetrahedra).", count);
            return res;
        }
    }
    return res;
}
