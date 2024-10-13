#include "WorldInteraction.h"


std::pair<bool, float> WorldInteraction::rayIntersectsBoundingBox(const Ray& ray, const BoundingBox& bbox , const glm::mat4 transform) {
        // Implementation of ray-box intersection
        glm::vec3 invDirection = 1.0f / ray.direction;
        glm::vec3 tMin = (glm::vec3(transform * glm::vec4(bbox.LBB, 1.0)) - ray.origin) * invDirection;
        glm::vec3 tMax = (glm::vec3(transform * glm::vec4(bbox.RFT, 1.0)) - ray.origin) * invDirection;

        glm::vec3 t1 = glm::min(tMin, tMax);
        glm::vec3 t2 = glm::max(tMin, tMax);

        float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
        float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

        if (tNear > tFar || tFar < 0) {
            return std::make_pair(false, 0.0f);
        }

        // Return true for intersection and tNear as the distance from the ray's origin
        return std::make_pair(true, tNear);
}


BVHNode* WorldInteraction::generateBVHForModel(std::vector<float> vertices, std::vector<unsigned int> indices) {

    std::vector<Triangle> triangles = generateTriangles(vertices, indices);

    return buildBVH(triangles, 0, triangles.size());
}

std::vector<Triangle> WorldInteraction::generateTriangles(std::vector<float> vertices, std::vector<unsigned int> indices) {
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

BVHNode* WorldInteraction::buildBVH(std::vector<Triangle>& triangles, int start, int end) {
    BVHNode* node = new BVHNode();

    // Step 1: Compute the bounding box for all triangles in this range
    BoundingBox boundingBox = triangles[start].boundingBox;
    for (int i = start + 1; i < end; ++i) {
        boundingBox.expand(triangles[i].boundingBox);
    }

    // Step 2: Base case: Create a leaf node if the number of triangles is small
    int numTriangles = end - start;
    if (numTriangles <= 2) {
        return createLeafNode(triangles, start, end, boundingBox);
    }

    node->boundingBox = boundingBox;

    // Step 3: Decide on the splitting axis based on the longest bounding box axis
    int splitAxis = boundingBox.maxExtent();

    // Step 4: Choose the partitioning method (Spatial Median or SAH)
    // You can toggle between the spatial median or SAH split strategies
    // For demonstration, we'll use SAH if the number of triangles exceeds a threshold
    int mid = (numTriangles > 10) ?
        partitionBySAH(triangles, start, end, splitAxis, boundingBox) :
        partitionBySpatialMedian(triangles, start, end, splitAxis);

    // If partitioning fails or the split doesn't work, fallback to the midpoint
    if (mid == start || mid == end) {
        mid = (start + end) / 2;
    }

    // Step 5: Recursively build the left and right children
    node->leftChild = buildBVH(triangles, start, mid);
    node->rightChild = buildBVH(triangles, mid, end);

    return node;
}



// Function to create a leaf node and assign triangles
BVHNode* WorldInteraction::createLeafNode(std::vector<Triangle>& triangles, int start, int end, const BoundingBox& boundingBox) {
    BVHNode* leafNode = new BVHNode();
    leafNode->triangles.assign(triangles.begin() + start, triangles.begin() + end);
    leafNode->boundingBox = boundingBox;
    leafNode->leftChild = nullptr;
    leafNode->rightChild = nullptr;

    // Optional: Set color for the bounding box based on the average normal of triangles
    glm::vec3 averageNormal(0.0f);
    for (const Triangle& triangle : leafNode->triangles) {
        averageNormal += glm::normalize(triangle.normal);
    }
    averageNormal = glm::normalize(averageNormal);
    leafNode->boundingBox.setColor(averageNormal);

    return leafNode;
}

// Partition triangles using the spatial median along the chosen axis
int WorldInteraction::partitionBySpatialMedian(std::vector<Triangle>& triangles, int start, int end, int axis) {
    float medianPoint = (triangles[start].centroid[axis] + triangles[end - 1].centroid[axis]) / 2.0f;

    // Partition triangles into two groups based on the median value
    auto midIt = std::partition(triangles.begin() + start, triangles.begin() + end,
        [axis, medianPoint](const Triangle& tri) {
            return tri.centroid[axis] < medianPoint;
        });

    return std::distance(triangles.begin(), midIt);
}

// Surface Area Heuristic (SAH) based partitioning
int WorldInteraction::partitionBySAH(std::vector<Triangle>& triangles, int start, int end, int axis, const BoundingBox& boundingBox) {
    const int numTriangles = end - start;

    // Step 1: Sort triangles based on the centroid along the given axis
    std::sort(triangles.begin() + start, triangles.begin() + end, [axis](const Triangle& a, const Triangle& b) {
        return a.centroid[axis] < b.centroid[axis];
        });

    // Step 2: Compute the bounding boxes from left to right and right to left
    std::vector<BoundingBox> leftBoundingBoxes(numTriangles);
    std::vector<BoundingBox> rightBoundingBoxes(numTriangles);

    leftBoundingBoxes[0] = triangles[start].boundingBox;
    for (int i = 1; i < numTriangles; ++i) {
        leftBoundingBoxes[i] = leftBoundingBoxes[i - 1];
        leftBoundingBoxes[i].expand(triangles[start + i].boundingBox);
    }

    rightBoundingBoxes[numTriangles - 1] = triangles[end - 1].boundingBox;
    for (int i = numTriangles - 2; i >= 0; --i) {
        rightBoundingBoxes[i] = rightBoundingBoxes[i + 1];
        rightBoundingBoxes[i].expand(triangles[start + i].boundingBox);
    }

    // Step 3: Calculate SAH for each potential split point and find the minimum SAH
    float minSAH = std::numeric_limits<float>::infinity();
    int bestSplit = start;

    for (int i = 0; i < numTriangles - 1; ++i) {
        float leftArea = leftBoundingBoxes[i].surfaceArea();
        float rightArea = rightBoundingBoxes[i + 1].surfaceArea();
        float sah = leftArea * (i + 1) + rightArea * (numTriangles - (i + 1));  // SAH cost

        if (sah < minSAH) {
            minSAH = sah;
            bestSplit = start + i + 1;  // The index of the first element in the right partition
        }
    }

    return bestSplit;
}