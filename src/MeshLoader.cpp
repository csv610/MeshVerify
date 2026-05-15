#include "mesh_verify/MeshLoader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <fstream>
#include <sstream>

bool MeshLoader::loadMesh(const std::string& filename, Mesh2D& mesh) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, 
        aiProcess_Triangulate | 
        aiProcess_JoinIdenticalVertices | 
        aiProcess_SortByPType);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    if (scene->mNumMeshes == 0) {
        std::cerr << "No meshes found in file: " << filename << std::endl;
        return false;
    }

    aiMesh* aiM = scene->mMeshes[0];
    mesh.vertices.reserve(aiM->mNumVertices);
    for (unsigned int i = 0; i < aiM->mNumVertices; ++i) {
        mesh.vertices.push_back({aiM->mVertices[i].x, aiM->mVertices[i].y});
    }

    mesh.triangles.reserve(aiM->mNumFaces);
    for (unsigned int i = 0; i < aiM->mNumFaces; ++i) {
        aiFace face = aiM->mFaces[i];
        if (face.mNumIndices == 3) {
            mesh.triangles.push_back({{face.mIndices[0], face.mIndices[1], face.mIndices[2]}});
        }
    }

    return true;
}

bool MeshLoader::loadSurfaceMesh(const std::string& filename, SurfaceMesh& mesh) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, 
        aiProcess_Triangulate | 
        aiProcess_JoinIdenticalVertices | 
        aiProcess_SortByPType);

    if (!scene || !scene->mRootNode || scene->mNumMeshes == 0) return false;

    aiMesh* aiM = scene->mMeshes[0];
    mesh.vertices.reserve(aiM->mNumVertices);
    for (unsigned int i = 0; i < aiM->mNumVertices; ++i) {
        mesh.vertices.push_back({aiM->mVertices[i].x, aiM->mVertices[i].y, aiM->mVertices[i].z});
    }

    mesh.triangles.reserve(aiM->mNumFaces);
    for (unsigned int i = 0; i < aiM->mNumFaces; ++i) {
        aiFace face = aiM->mFaces[i];
        if (face.mNumIndices == 3) {
            mesh.triangles.push_back({{face.mIndices[0], face.mIndices[1], face.mIndices[2]}});
        }
    }
    return true;
}

bool MeshLoader::loadVolumeMesh(const std::string& filename, TetMesh& mesh) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_JoinIdenticalVertices);

    if (!scene || !scene->mRootNode || scene->mNumMeshes == 0) return false;

    aiMesh* aiM = scene->mMeshes[0];
    mesh.vertices.reserve(aiM->mNumVertices);
    for (unsigned int i = 0; i < aiM->mNumVertices; ++i) {
        mesh.vertices.push_back({aiM->mVertices[i].x, aiM->mVertices[i].y, aiM->mVertices[i].z});
    }

    for (unsigned int i = 0; i < aiM->mNumFaces; ++i) {
        aiFace face = aiM->mFaces[i];
        if (face.mNumIndices == 4) {
            mesh.tets.push_back({{face.mIndices[0], face.mIndices[1], face.mIndices[2], face.mIndices[3]}});
        }
    }

    if (mesh.tets.empty()) {
        std::cerr << "No tetrahedra (faces with 4 indices) found in " << filename << std::endl;
        return false;
    }

    return true;
}

bool MeshLoader::loadVoronoiCells(const std::string& filename, VoronoiMesh& mesh) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, 
        aiProcess_JoinIdenticalVertices | 
        aiProcess_SortByPType);

    if (!scene || !scene->mRootNode) return false;

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        aiMesh* aiM = scene->mMeshes[m];
        size_t vertexOffset = mesh.vertices.size();

        for (unsigned int i = 0; i < aiM->mNumVertices; ++i) {
            mesh.vertices.push_back({aiM->mVertices[i].x, aiM->mVertices[i].y});
        }

        for (unsigned int i = 0; i < aiM->mNumFaces; ++i) {
            aiFace face = aiM->mFaces[i];
            VoronoiCell cell;
            cell.siteIndex = m;
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                cell.vertexIndices.push_back(vertexOffset + face.mIndices[j]);
            }
            mesh.cells.push_back(cell);
        }
    }
    return true;
}

bool MeshLoader::loadSites(const std::string& filename, std::vector<Point>& sites) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        double x, y;
        if (ss >> x >> y) {
            sites.push_back({x, y});
        }
    }
    return true;
}
