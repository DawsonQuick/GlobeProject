#pragma once
#include "./CommonDataStrutures/CommonDataStructures.h"
#include "./../ECS/Components/Components.h"

#include <algorithm>


class WorldInteraction {
public:
    static std::pair<bool, float> rayIntersectsBoundingBox(const Ray& ray, const BoundingBox& bbox, const glm::mat4 transform);
    static BVHNode* generateBVHForModel(std::vector<float> vertices, std::vector<unsigned int> indices);

private:
    static std::vector<Triangle> generateTriangles(std::vector<float> vertices, std::vector<unsigned int> indices);
    static BVHNode* buildBVH(std::vector<Triangle>& triangles, int start, int end);
    static BVHNode* createLeafNode(std::vector<Triangle>& triangles, int start, int end, const BoundingBox& boundingBox);
    static int partitionBySpatialMedian(std::vector<Triangle>& triangles, int start, int end, int axis);
    static int partitionBySAH(std::vector<Triangle>& triangles, int start, int end, int axis, const BoundingBox& boundingBox);
};