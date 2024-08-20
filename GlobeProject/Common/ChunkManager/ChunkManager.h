#pragma once
#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H
#include <entt/entt.hpp>
#include <iostream>
#include <map>
#include <vector>
#include "./../../Common/Vendor/glm/glm.hpp"
#include <algorithm> // For std::find

// Hash function for glm::ivec3
struct Vec3Hasher {
    std::size_t operator()(const glm::ivec3 &v) const {
        std::size_t h1 = std::hash<int>{}(v.x);
        std::size_t h2 = std::hash<int>{}(v.y);
        std::size_t h3 = std::hash<int>{}(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2); // Combine hash values
    }
};

// Equality function for glm::ivec3
struct Vec3Equal {
    bool operator()(const glm::ivec3 &lhs, const glm::ivec3 &rhs) const {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }
};


namespace ChunkManager {
	inline std::unordered_map<glm::ivec3, std::vector<entt::entity>, Vec3Hasher, Vec3Equal> chunkToEntityMap;

    
    inline void removeEntityFromChunk(const glm::ivec3& chunk, const entt::entity& entity) {
        // Access the chunk directly
        auto& entityVec = chunkToEntityMap.at(chunk);

        // Find the entity in the vector
        auto entityIt = std::find(entityVec.begin(), entityVec.end(), entity);

        if (entityIt != entityVec.end()) {
            // Remove the entity from the vector
            entityVec.erase(entityIt);

            // If the vector is empty after removal, erase the key from the map
            if (entityVec.empty()) {
                chunkToEntityMap.erase(chunk);
            }
        }
        else {
            // Entity not found in the vector
            std::cerr << "Entity not found in the chunk!" << std::endl;
        }
    }
    
    
    inline void addEntityToChunk(glm::ivec3& chunk, entt::entity& entity) {
        // Check if the chunk exists in the map
        auto it = chunkToEntityMap.find(chunk);

        if (it != chunkToEntityMap.end()) {
            // If the chunk exists, append the entity to the existing vector
            it->second.push_back(entity);
        }
        else {
            // If the chunk does not exist, create a new entry with the chunk key
            // and initialize it with a vector containing the entity
            chunkToEntityMap[chunk] = std::vector<entt::entity>{ entity };
        }
    }
    

}
#endif