#include "TerrainUtils.h"

BVHNode* TerrainUtils::generateBVHForModel(std::vector<float> vertices, std::vector<unsigned int> indices) {

    std::vector<Triangle> triangles = generateTriangles(vertices, indices);
    
    return buildBVH(triangles, 0, triangles.size());
}

std::vector<Triangle> TerrainUtils::generateTriangles(std::vector<float> vertices, std::vector<unsigned int> indices) {
    std::vector<Triangle> triangles;
    for (size_t i = 0; i < indices.size(); i += 3) {
        int idx0 = indices[i] * 3;
        int idx1 = indices[i + 1] * 3;
        int idx2 = indices[i + 2] * 3; 

        glm::vec3 vertex1 = glm::vec3(vertices[idx0], vertices[idx0 + 1], vertices[idx0 + 2]);
        glm::vec3 vertex2 = glm::vec3(vertices[idx1], vertices[idx1 + 1], vertices[idx1 + 2]);
        glm::vec3 vertex3 = glm::vec3(vertices[idx2], vertices[idx2 + 1], vertices[idx2 + 2]);
        triangles.emplace_back(vertex1, vertex2, vertex3);
    }
    return triangles;
}

BVHNode* TerrainUtils::buildBVH(std::vector<Triangle>& triangles, int start, int end) {
    BVHNode* node = new BVHNode();

    // Step 1: Compute the bounding box for all triangles in this range
    BoundingBox boundingBox = triangles[start].boundingBox;
    for (int i = start + 1; i < end; ++i) {
        boundingBox.expand(triangles[i].boundingBox);
    }
   
    
    // Step 2: Base case: If we have a small number of triangles, make this a leaf node
    int numTriangles = end - start;
    if (numTriangles <= 50) {  // Threshold for making leaf nodes (e.g., 2 triangles)
        node->triangles.assign(triangles.begin() + start, triangles.begin() + end);

        glm::vec3 averageNormal;
        for (Triangle triangle : node->triangles) {
            averageNormal.x += triangle.normal.x;
            averageNormal.y += triangle.normal.y;
            averageNormal.z += triangle.normal.z;
        }
        averageNormal.x /= node->triangles.size();
        averageNormal.y /= node->triangles.size();
        averageNormal.z /= node->triangles.size();

        boundingBox.setColor(glm::normalize(averageNormal));

        node->boundingBox = boundingBox;
        node->leftChild = node->rightChild = nullptr;
        return node;
    }
    node->boundingBox = boundingBox;
    // Step 3: Otherwise, split the triangles into two groups
    // Sort by centroid along the longest axis of the bounding box
    int splitAxis = boundingBox.maxExtent(); // Axis with the greatest range
    std::sort(triangles.begin() + start, triangles.begin() + end, [splitAxis](const Triangle& a, const Triangle& b) {
        return a.centroid[splitAxis] < b.centroid[splitAxis];
        });

    // Step 4: Recursively build the left and right children
    int mid = (start + end) / 2;
    node->leftChild = buildBVH(triangles, start, mid);
    node->rightChild = buildBVH(triangles, mid, end);
    
    return node;
}