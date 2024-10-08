#pragma once
#ifndef BOUNDINGBOXGENERATOR_H
#define BOUNDINGBOXGENERATOR_H

#include "./../../../../Common/Vendor/glm/glm.hpp"
#include "./../AssimpLoader/AssimpLoader.h"
#include <vector>
struct ModelBoundingBox {
    glm::vec3 RBT;
    glm::vec3 RBB;
    glm::vec3 RFT;
    glm::vec3 RFB;

    glm::vec3 LBT;
    glm::vec3 LBB;
    glm::vec3 LFT;
    glm::vec3 LFB;

};

inline std::vector<float> generateEdges(const ModelBoundingBox& bbox) {
    std::vector<float> edges;

    auto addEdge = [&](const glm::vec3& start, const glm::vec3& end) {
        edges.push_back(start.x); edges.push_back(start.y); edges.push_back(start.z);
        edges.push_back(end.x);   edges.push_back(end.y);   edges.push_back(end.z);
        };

    // Define the edges of the bounding box
    addEdge(bbox.RBB, bbox.RBT); // Right Back Bottom to Right Back Top
    addEdge(bbox.RBT, bbox.RFT); // Right Back Top to Right Front Top
    addEdge(bbox.RFT, bbox.RFB); // Right Front Top to Right Front Bottom
    addEdge(bbox.RFB, bbox.RBB); // Right Front Bottom to Right Back Bottom

    addEdge(bbox.LBB, bbox.LBT); // Left Back Bottom to Left Back Top
    addEdge(bbox.LBT, bbox.LFT); // Left Back Top to Left Front Top
    addEdge(bbox.LFT, bbox.LFB); // Left Front Top to Left Front Bottom
    addEdge(bbox.LFB, bbox.LBB); // Left Front Bottom to Left Back Bottom

    addEdge(bbox.RBB, bbox.LBB); // Right Back Bottom to Left Back Bottom
    addEdge(bbox.RBT, bbox.LBT); // Right Back Top to Left Back Top
    addEdge(bbox.RFT, bbox.LFT); // Right Front Top to Left Front Top
    addEdge(bbox.RFB, bbox.LFB); // Right Front Bottom to Left Front Bottom

    return edges;
}

inline ModelBoundingBox calculateBoundingBox(const std::vector<Vertex>& tmpVertices) {
    ModelBoundingBox newBoundingBox;
    bool firstPass = true;
    float minX = 0.0f;
    float maxX = 0.0f;
    float minY = 0.0f;
    float maxY = 0.0f;
    float minZ = 0.0f;
    float maxZ = 0.0f;
    float scale = 1.0;
    for (const Vertex& vertex : tmpVertices) {
        if (firstPass) {
            firstPass = false;
            minX = maxX = vertex.position.x;
            minY = maxY = vertex.position.y;
            minZ = maxZ = vertex.position.z;
        }
        else {
            if (vertex.position.x < minX) {
                minX = vertex.position.x;
            }
            else if (vertex.position.x > maxX) {
                maxX = vertex.position.x;
            }

            if (vertex.position.y < minY) {
                minY = vertex.position.y;
            }
            else if (vertex.position.y > maxY) {
                maxY = vertex.position.y;
            }

            if (vertex.position.z < minZ) {
                minZ = vertex.position.z;
            }
            else if (vertex.position.z > maxZ) {
                maxZ = vertex.position.z;
            }
        }
    }

    newBoundingBox.RBB = glm::vec3(maxX, minY, minZ) * scale;
    newBoundingBox.RBT = glm::vec3(maxX, maxY, minZ) * scale;
    newBoundingBox.RFT = glm::vec3(maxX, maxY, maxZ) * scale;
    newBoundingBox.RFB = glm::vec3(maxX, minY, maxZ) * scale;
    newBoundingBox.LBB = glm::vec3(minX, minY, minZ) * scale;
    newBoundingBox.LBT = glm::vec3(minX, maxY, minZ) * scale;
    newBoundingBox.LFT = glm::vec3(minX, maxY, maxZ) * scale;
    newBoundingBox.LFB = glm::vec3(minX, minY, maxZ) * scale;
 
    return newBoundingBox;
}

inline ModelBoundingBox calculateBoundingBox(const std::vector<float>& vertices, size_t setSize = 8, size_t valuesToExtract = 3) {
    // Ensure the input vector size is a multiple of the set size
    if (vertices.size() % setSize != 0) {
        throw std::runtime_error("Input vector size must be a multiple of the set size.");
    }

    std::vector<Vertex> tmpVertices;

    // Iterate through the input vector in steps of `setSize`
    for (size_t i = 0; i < vertices.size(); i += setSize) {
        glm::vec3 position(vertices[i], vertices[i + 1], vertices[i + 2]);
        Vertex vertex;
        vertex.position = position;
        tmpVertices.push_back(vertex);
    }

    return calculateBoundingBox(tmpVertices);
}

#endif // BOUNDINGBOXGENERATOR_H
