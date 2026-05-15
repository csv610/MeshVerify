#include "MeshTopology.hpp"
#include <map>
#include <iostream>
#include <algorithm>

HalfEdgeMesh MeshTopology::build(const Mesh2D& mesh) {
    HalfEdgeMesh heMesh;
    heMesh.vertices2D = &mesh.vertices;
    heMesh.faceToEdge.resize(mesh.triangles.size(), INVALID_INDEX);
    heMesh.vertexToEdge.resize(mesh.vertices.size(), INVALID_INDEX);
    heMesh.edges.reserve(mesh.triangles.size() * 3);

    std::map<EdgeKey, size_t> edgeMap;

    for (size_t i = 0; i < mesh.triangles.size(); ++i) {
        const auto& tri = mesh.triangles[i];
        size_t baseIdx = heMesh.edges.size();
        
        for (int j = 0; j < 3; ++j) {
            HEEdge edge;
            edge.vertex = tri.v[j];
            edge.face = i;
            edge.next = baseIdx + (j + 1) % 3;
            
            heMesh.edges.push_back(edge);
            heMesh.vertexToEdge[edge.vertex] = baseIdx + j;

            EdgeKey key(tri.v[j], tri.v[(j + 1) % 3]);
            if (edgeMap.count(key)) {
                size_t twinIdx = edgeMap[key];
                heMesh.edges[baseIdx + j].twin = twinIdx;
                heMesh.edges[twinIdx].twin = baseIdx + j;
            } else {
                edgeMap[key] = baseIdx + j;
            }
        }
        heMesh.faceToEdge[i] = baseIdx;
    }
    return heMesh;
}

HalfEdgeMesh MeshTopology::build(const SurfaceMesh& mesh) {
    HalfEdgeMesh heMesh;
    heMesh.vertices3D = &mesh.vertices;
    heMesh.faceToEdge.resize(mesh.triangles.size(), INVALID_INDEX);
    heMesh.vertexToEdge.resize(mesh.vertices.size(), INVALID_INDEX);
    heMesh.edges.reserve(mesh.triangles.size() * 3);

    std::map<EdgeKey, size_t> edgeMap;

    for (size_t i = 0; i < mesh.triangles.size(); ++i) {
        const auto& tri = mesh.triangles[i];
        size_t baseIdx = heMesh.edges.size();
        
        for (int j = 0; j < 3; ++j) {
            HEEdge edge;
            edge.vertex = tri.v[j];
            edge.face = i;
            edge.next = baseIdx + (j + 1) % 3;
            
            heMesh.edges.push_back(edge);
            heMesh.vertexToEdge[edge.vertex] = baseIdx + j;

            EdgeKey key(tri.v[j], tri.v[(j + 1) % 3]);
            if (edgeMap.count(key)) {
                size_t twinIdx = edgeMap[key];
                heMesh.edges[baseIdx + j].twin = twinIdx;
                heMesh.edges[twinIdx].twin = baseIdx + j;
            } else {
                edgeMap[key] = baseIdx + j;
            }
        }
        heMesh.faceToEdge[i] = baseIdx;
    }
    return heMesh;
}

TetAdjacency MeshTopology::build(const TetMesh& mesh) {
    TetAdjacency adj;
    adj.vertices = &mesh.vertices;
    adj.tets = &mesh.tets;
    adj.neighbors.resize(mesh.tets.size(), {INVALID_INDEX, INVALID_INDEX, INVALID_INDEX, INVALID_INDEX});

    std::map<FaceKey, std::pair<size_t, int>> faceMap;

    for (size_t i = 0; i < mesh.tets.size(); ++i) {
        const auto& tet = mesh.tets[i];
        std::array<std::array<size_t, 3>, 4> faces = {{
            {{tet.v[0], tet.v[1], tet.v[2]}},
            {{tet.v[0], tet.v[1], tet.v[3]}},
            {{tet.v[0], tet.v[2], tet.v[3]}},
            {{tet.v[1], tet.v[2], tet.v[3]}}
        }};

        for (int j = 0; j < 4; ++j) {
            FaceKey key(faces[j][0], faces[j][1], faces[j][2]);
            if (faceMap.count(key)) {
                auto other = faceMap[key];
                adj.neighbors[i][j] = other.first;
                adj.neighbors[other.first][other.second] = i;
            } else {
                faceMap[key] = {i, j};
            }
        }
    }
    return adj;
}
