#pragma once
#include <vector>
#include <algorithm>
#include "./../../../Common/WorldInteraction/CommonDataStrutures/CommonDataStructures.h"

struct Triangle {
    glm::vec3 v0, v1, v2;  // The three vertices of the triangle
    glm::vec3 centroid;    // Precomputed centroid for sorting
    glm::vec3 normal;      // Normal vector for the triangle
    BoundingBox boundingBox; // Full bounding box for the triangle

    Triangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
        : v0(p0), v1(p1), v2(p2) {
        // Compute centroid
        centroid = (v0 + v1 + v2) / 3.0f;

        // Calculate min and max for the AABB
        glm::vec3 min = glm::min(v0, glm::min(v1, v2));
        glm::vec3 max = glm::max(v0, glm::max(v1, v2));

        // Initialize the BoundingBox using the AABB
        boundingBox.setFromAABB(min, max);

        // Compute the normal of the triangle using the cross product of two edges
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        normal = glm::normalize(glm::cross(edge1, edge2));  // Normalize to get the unit normal
    }
};

struct BVHNode {
    BoundingBox boundingBox;           // Axis-Aligned Bounding Box or other bounding volume
    BVHNode* leftChild;         // Pointer to the left child (or nullptr if a leaf node)
    BVHNode* rightChild;        // Pointer to the right child (or nullptr if a leaf node)
    std::vector<Triangle> triangles;  // Leaf node stores the actual triangles (or references)

    bool isLeaf() const {
        return leftChild == nullptr && rightChild == nullptr;
    }
};



class TerrainUtils {
public:
    static BVHNode* generateBVHForModel(std::vector<float> vertices, std::vector<unsigned int> indices);

private:
    static std::vector<Triangle> generateTriangles(std::vector<float> vertices, std::vector<unsigned int> indices);
    static BVHNode* buildBVH(std::vector<Triangle>& triangles, int start, int end);

};