#include "AssimpLoader.h"
Assimp::Importer AssimpLoader::importer;

void AssimpLoader::loadModel(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const std::string filePath) {
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenSmoothNormals);
        // Assume a single mesh for simplicity (you may need to loop through multiple meshes)
        for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
            const aiMesh* mesh = scene->mMeshes[meshIndex];
            // Store the starting index for each mesh
            unsigned int baseIndex = vertices.size();

            // Extract vertices
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                Vertex vertex;
                // Extract position
                vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                // Extract normal
                vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
                // Extract texture coordinates (if available)
                if (mesh->HasTextureCoords(0)) {
                    vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                }
                else {
                    vertex.texCoords = glm::vec2(0.0f, 0.0f);
                }


                vertices.push_back(vertex);
            }

            // Extract indices
            for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
                const aiFace& face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                    indices.push_back(baseIndex + face.mIndices[j]);
                }
            }
        }
    std::cout << vertices.size() << std::endl;
    std::cout << indices.size() << std::endl;
}