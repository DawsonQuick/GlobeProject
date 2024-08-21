#pragma once
#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H
#include <entt/entt.hpp>
#include <iostream>
#include <map>
#include <vector>
#include "./../../Common/Vendor/glm/glm.hpp"
#include "./../../Common/WorldInteraction/WorldInteraction.h"
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

//Place the variable outside the namespace to make it "private" and only accessible through the getChunkSize() function
namespace {
   static int chunkSize = 5000;
}

namespace ChunkManager {
	inline std::unordered_map<glm::ivec3, std::vector<entt::entity>, Vec3Hasher, Vec3Equal> chunkToEntityMap;



    // Getter that returns a reference
    inline int getChunkSize() {
        return chunkSize;
    }

    //If the chunk size is changed, need to clear out the current map due to the mapping being incorrect ratio
    inline void setChunkSize(int tmpChunkSize) {
        if (tmpChunkSize > 0) {
            chunkToEntityMap.clear();
            chunkSize = tmpChunkSize;
        }
    }
    
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

    //For now put the ray collition function inside of the ChunkManager
    inline entt::entity findRayIntersectEntity(const Ray& ray) {

        //TODO : algorithm to go through the chunk data to see which entity the ray intersects
        // First: calculate which chunk the ray originated from (ray.origin/chunksize)
        //      (when determining the chunk rule of thumb is to add 0.5 to the result and take the math.floor , this is to round anything above .5 to the higher value and below 0.5 to the lower)
        // Second loop through all the entities in the chunk to see if the ray intersects them directly
        //      (Possibly use a compute shader , determine if entity was hit and how far. want to select the closet to the origin)
        // If not, Third step the ray out using the direction and step out 1 additional chunksize (ray.direction * chunksize = newLocation)
        // Forth Then convert that new location to its chunk like in step 1
        // Fith Then repeat step two just with this new location
        // Repeat steps 3 - 5 till entity intersection
        // If a intercection occurs return the 
    }


    

}
#endif