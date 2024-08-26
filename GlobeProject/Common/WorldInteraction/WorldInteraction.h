#pragma once
#include "./CommonDataStrutures/CommonDataStructures.h"
#include "./../ECS/Components/Components.h"

inline bool rayIntersectsBoundingBox(const Ray& ray, const BoundingBox& bbox) {
    // Implementation of ray-box intersection
    glm::vec3 invDirection = 1.0f / ray.direction;
    glm::vec3 tMin = (bbox.LBB - ray.origin) * invDirection;
    glm::vec3 tMax = (bbox.RFT - ray.origin) * invDirection;

    glm::vec3 t1 = glm::min(tMin, tMax);
    glm::vec3 t2 = glm::max(tMin, tMax);

    float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
    float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

    if (tNear > tFar || tFar < 0) {
        return false;
    }

    return true;
}

inline std::pair<bool, float> rayIntersectsBoundingBox(const Ray& ray, const BoundingBoxComponent& bbox) {
    // Implementation of ray-box intersection
    glm::vec3 invDirection = 1.0f / ray.direction;
    glm::vec3 tMin = (bbox.LBB - ray.origin) * invDirection;
    glm::vec3 tMax = (bbox.RFT - ray.origin) * invDirection;

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