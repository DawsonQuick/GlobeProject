#pragma once
#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H
#include <entt/entt.hpp>
#include <iostream>
#include <map>
#include <vector>
#include "./../../Common/Vendor/glm/glm.hpp"
#include "./../../Common/WorldInteraction/WorldInteraction.h"
#include "./../../Common/ECS/Systems/ECSRegistry/Registry.h"
#include "./../../Common/Logger/Logger.h"
#include "./../../Common/TerrainGeneration/PlanarTerrain/ComputeShaderMethod/ComputeShaderPlanarTerrainGeneration.h"
#include "./../../Common/Utilities/Stopwatch/Stopwatch.h"
#include "./../../Common/ModelManager/ModelManager.h"
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

//Section to make private variables for the ChunkManager Namespace
namespace {
   static int chunkSize = 5000;
   bool isDebugModeEnabled = true;
   Stopwatch<std::chrono::milliseconds> sw;
}


//Section to make private methods for the ChunkManagerNamespace
namespace {
    glm::ivec3 getChunkFromPosition(glm::vec3 position) {
        return glm::ivec3(std::floor((position.x / chunkSize) + 0.5), std::floor((position.y / chunkSize) + 0.5), std::floor((position.z / chunkSize) + 0.5));
    }

    void generateChunkRenderBoundingBox(std::vector<float>& renderData, glm::ivec3 chunk, const glm::vec3& color) {
        // Center the chunk around the origin, with half the chunk size extending in each direction
        glm::vec3 chunkOrigin = glm::vec3(chunk) * static_cast<float>(chunkSize);

        glm::vec3 halfSize = glm::vec3(chunkSize * 0.5f);

        glm::vec3 LBB = chunkOrigin - halfSize;
        glm::vec3 RFT = chunkOrigin + halfSize;

        glm::vec3 LBF = LBB + glm::vec3(0, 0, chunkSize);
        glm::vec3 LTB = LBB + glm::vec3(0, chunkSize, 0);
        glm::vec3 LTF = LBB + glm::vec3(0, chunkSize, chunkSize);
        glm::vec3 RBB = LBB + glm::vec3(chunkSize, 0, 0);
        glm::vec3 RBF = RBB + glm::vec3(0, 0, chunkSize);
        glm::vec3 RTB = RBB + glm::vec3(0, chunkSize, 0);

        std::vector<float> edges = {
            // Bottom square (LBB, LBF, RBB, RBF)
            LBB.x, LBB.y, LBB.z, color.r, color.g, color.b,  RBB.x, RBB.y, RBB.z, color.r, color.g, color.b,
            RBB.x, RBB.y, RBB.z, color.r, color.g, color.b,  RBF.x, RBF.y, RBF.z, color.r, color.g, color.b,
            RBF.x, RBF.y, RBF.z, color.r, color.g, color.b,  LBF.x, LBF.y, LBF.z, color.r, color.g, color.b,
            LBF.x, LBF.y, LBF.z, color.r, color.g, color.b,  LBB.x, LBB.y, LBB.z, color.r, color.g, color.b,

            // Top square (LTB, LTF, RTB, RFT)
            LTB.x, LTB.y, LTB.z, color.r, color.g, color.b,  RTB.x, RTB.y, RTB.z, color.r, color.g, color.b,
            RTB.x, RTB.y, RTB.z, color.r, color.g, color.b,  RFT.x, RFT.y, RFT.z, color.r, color.g, color.b,
            RFT.x, RFT.y, RFT.z, color.r, color.g, color.b,  LTF.x, LTF.y, LTF.z, color.r, color.g, color.b,
            LTF.x, LTF.y, LTF.z, color.r, color.g, color.b,  LTB.x, LTB.y, LTB.z, color.r, color.g, color.b,

            // Vertical edges
            LBB.x, LBB.y, LBB.z, color.r, color.g, color.b,  LTB.x, LTB.y, LTB.z, color.r, color.g, color.b,
            RBB.x, RBB.y, RBB.z, color.r, color.g, color.b,  RTB.x, RTB.y, RTB.z, color.r, color.g, color.b,
            RBF.x, RBF.y, RBF.z, color.r, color.g, color.b,  RFT.x, RFT.y, RFT.z, color.r, color.g, color.b,
            LBF.x, LBF.y, LBF.z, color.r, color.g, color.b,  LTF.x, LTF.y, LTF.z, color.r, color.g, color.b
        };

        renderData.insert(renderData.end(), edges.begin(), edges.end());
    }


    void generateRenderSelectedEntityBoundingBox(std::vector<float>& renderData,BVHNode *bvhNode, const glm::vec3& color , glm::mat4 trans) {
            if (!bvhNode) return;  // Base case: if the node is null, stop recursion

            glm::vec3 LBB = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.LBB, 1.0));
            glm::vec3 RBB = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.RBB, 1.0));
            glm::vec3 RFB = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.RFB, 1.0));
            glm::vec3 LFB = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.LFB, 1.0));
            glm::vec3 LBT = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.LBT, 1.0));
            glm::vec3 RFT = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.RFT, 1.0));
            glm::vec3 LFT = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.LFT, 1.0));
            glm::vec3 RBT = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.RBT, 1.0));

            std::vector<float> edges = {
                // Bottom square (LBB, LBF, RBB, RBF)
                LBB.x, LBB.y, LBB.z, color.r, color.g, color.b,  RBB.x, RBB.y , RBB.z, color.r, color.g, color.b,
                RBB.x, RBB.y, RBB.z, color.r, color.g, color.b,  RFB.x, RFB.y , RFB.z, color.r, color.g, color.b,
                RFB.x, RFB.y, RFB.z, color.r, color.g, color.b,  LFB.x, LFB.y , LFB.z, color.r, color.g, color.b,
                LFB.x, LFB.y, LFB.z, color.r, color.g, color.b,  LBB.x, LBB.y , LBB.z, color.r, color.g, color.b,

                // Top square (LTB, LTF, RTB, RFT)
                LBT.x, LBT.y, LBT.z, color.r, color.g, color.b,  RBT.x, RBT.y , RBT.z, color.r, color.g, color.b,
                RBT.x, RBT.y, RBT.z, color.r, color.g, color.b,  RFT.x, RFT.y , RFT.z, color.r, color.g, color.b,
                RFT.x, RFT.y, RFT.z, color.r, color.g, color.b,  LFT.x, LFT.y , LFT.z, color.r, color.g, color.b,
                LFT.x, LFT.y, LFT.z, color.r, color.g, color.b,  LBT.x, LBT.y , LBT.z, color.r, color.g, color.b,

                // Vertical edges
                LBB.x, LBB.y, LBB.z, color.r, color.g, color.b,  LBT.x, LBT.y , LBT.z, color.r, color.g, color.b,
                RBB.x, RBB.y, RBB.z, color.r, color.g, color.b,  RBT.x, RBT.y , RBT.z, color.r, color.g, color.b,
                RFB.x, RFB.y, RFB.z, color.r, color.g, color.b,  RFT.x, RFT.y , RFT.z, color.r, color.g, color.b,
                LFB.x, LFB.y, LFB.z, color.r, color.g, color.b,  LFT.x, LFT.y , LFT.z, color.r, color.g, color.b
            };

            renderData.insert(renderData.end(), edges.begin(), edges.end());


            // Recursively render bounding boxes for the left and right children
            if (bvhNode->leftChild) {
                generateRenderSelectedEntityBoundingBox(renderData, bvhNode->leftChild, color,trans);
            }
            if (bvhNode->rightChild) {
                generateRenderSelectedEntityBoundingBox(renderData, bvhNode->rightChild, color, trans);
            }
        

    }

    void generateRenderRay(std::vector<float>& renderData, const Ray& ray) {
        renderData.push_back(ray.origin.x);
        renderData.push_back(ray.origin.y);
        renderData.push_back(ray.origin.z);
        renderData.push_back(1.0);
        renderData.push_back(1.0);
        renderData.push_back(1.0);
        renderData.push_back(ray.end.x);
        renderData.push_back(ray.end.y);
        renderData.push_back(ray.end.z);
        renderData.push_back(1.0);
        renderData.push_back(1.0);
        renderData.push_back(1.0);
    }



}

namespace ChunkManager {
	inline std::unordered_map<glm::ivec3, std::vector<entt::entity>, Vec3Hasher, Vec3Equal> chunkToEntityMap;
    inline std::unordered_map<glm::ivec3, std::vector<glm::vec3>, Vec3Hasher, Vec3Equal> chunkToTerrainMap;

    inline std::vector<float> debugRenderInformation;
    inline std::vector<float> allChunkBoundingBoxes;
    inline bool forceRefresh = false;

    inline bool isEntitySelected;
    inline entt::entity selectedEntity;

    inline glm::ivec3 currentCameraChunk;
    inline bool hasCameraChunkChanged = false;
    inline bool hasNewChunkBeenLoaded = false;

    inline bool hasChunkBeenLoaded = false;
    inline bool hasChunkBeenReceived = false;

    inline bool& getIsDebugEnabled() {
        return isDebugModeEnabled;
    }

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

    inline void updateCameraPosition(glm::vec3& cameraPosition) {
        currentCameraChunk = getChunkFromPosition(cameraPosition);
    }

    inline glm::ivec3& getCurrentCameraChunk() {
        return currentCameraChunk;
    }

    

    /*---------------------------------------------------------------------------------------------------------------
*                                               Terrain Loading
  ---------------------------------------------------------------------------------------------------------------*/
    inline void removeTerrainPointFromChunk(const glm::ivec3& chunk, const glm::vec3& point) {
        // Access the chunk directly
        auto& entityVec = chunkToTerrainMap.at(chunk);

        // Find the entity in the vector
        auto entityIt = std::find(entityVec.begin(), entityVec.end(), point);

        if (entityIt != entityVec.end()) {
            // Remove the entity from the vector
            entityVec.erase(entityIt);

            // If the vector is empty after removal, erase the key from the map
            if (entityVec.empty()) {
                chunkToTerrainMap.erase(chunk);
            }
        }
        else {
            // Entity not found in the vector
           LOG_MESSAGE(LogLevel::ERROR,"Entity not found in the chunk!");
        }
    }


    inline void addTerrainPointToChunk(glm::ivec3& chunk, glm::vec3 point) {
        // Check if the chunk exists in the map
        auto it = chunkToTerrainMap.find(chunk);

        if (it != chunkToTerrainMap.end()) {
            // If the chunk exists, append the entity to the existing vector
            it->second.push_back(point);
        }
        else {
            // If the chunk does not exist, create a new entry with the chunk key
            // and initialize it with a vector containing the entity
            generateChunkRenderBoundingBox(allChunkBoundingBoxes, chunk, glm::vec3(1.0, 1.0, 1.0));
            chunkToTerrainMap[chunk] = std::vector<glm::vec3>{ point };
        }
    }
    //---------------------------------------------------------------------------------------------------------------




    /*---------------------------------------------------------------------------------------------------------------
    *                                               Entity Loading
      ---------------------------------------------------------------------------------------------------------------*/
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
            LOG_MESSAGE(LogLevel::ERROR, "Entity not found in the chunk!");
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
            generateChunkRenderBoundingBox(allChunkBoundingBoxes, chunk, glm::vec3(1.0, 1.0, 1.0));
            chunkToEntityMap[chunk] = std::vector<entt::entity>{ entity };
        }
    }

    inline void clearEntityToChunkMap() {
        isEntitySelected = false;
        chunkToEntityMap.clear();
        debugRenderInformation.clear();
    }
    //---------------------------------------------------------------------------------------------------------------

    inline std::vector<float>& getDebugRenderInformation(){
        return debugRenderInformation;
    }

    inline bool getForceRefresh() {
        if (forceRefresh) {
            forceRefresh = false; //Reset the state, only needed for reset pulse when new debug info is generated not ever render loop
            return true;
        }
        else {
            return false;
        }
    }

    inline bool& hasEntitySelected() {
        return isEntitySelected;
    }

    inline entt::entity getSelectedEntity() {
        return selectedEntity;
    }





    //For now put the ray collition function inside of the ChunkManager
    inline void findRayIntersectEntity(const Ray& ray) {

        //Clear out the previous debug render info
        debugRenderInformation.clear();
        forceRefresh = true;
        generateRenderRay(debugRenderInformation, ray);
        //Set the starting ray position to the ray origin
        glm::vec3 rayPosition = ray.origin;
        //Set the current chunk to a large value setting its defaults
        glm::ivec3 currentChunk = glm::ivec3(9999999, 9999999, 9999999);
        int index = 0;

        while (index < 200) {

            //Propogate the reay forward by its direction a set amount of distance 
            rayPosition += ray.direction * static_cast<float>(chunkSize / 4);
            //See which chunk the new ray position lands
            glm::ivec3 newChunk = getChunkFromPosition(rayPosition);

            //If the new ray position is in a new chunk then check, if it is still in the same chunk as the previous step then ignore
            if (newChunk != currentChunk) {
                
                //Make sure that the new chunk is actually inside the cached map (meaning there is atleast 1 entity in that chunk)
                auto it = chunkToEntityMap.find(newChunk);
                if (it != chunkToEntityMap.end()) {

                    generateChunkRenderBoundingBox(debugRenderInformation, newChunk, glm::vec3(0.0f,1.0f,0.0f));  


                    /* -------------------------------------------------------------------------------------------------------------
                    *   Main collision check for entities inside of a chunk. Loop through all entities in the chunk and keep track of which one
                    *   was closest to the ray's origin. If there was no intersection made by any of the entities in the chunk, then continue the loop.
                    *   Else, if there was atleast one intersection, no need to continue further, break out of the loop and return.
                       -------------------------------------------------------------------------------------------------------------*/
                    entt::entity closestEntity;
                    float closestDistance = 9999999999999;
                    bool intersectionFound = false;
                    for (entt::entity ent : it->second) {
                        generateRenderSelectedEntityBoundingBox(debugRenderInformation, ModelManager::getModel(ECS::registry.get<TransformComponent>(ent).getModelId())->bvhNode, glm::vec3(1.0f, 1.0f, 1.0f), ECS::registry.get<TransformComponent>(ent).transform);
                        auto [intersects, distance] = WorldInteraction::rayIntersectsBoundingBox(ray, ModelManager::getModel(ECS::registry.get<TransformComponent>(ent).getModelId())->bvhNode->boundingBox, ECS::registry.get<TransformComponent>(ent).transform);
                        if (intersects) {
                            intersectionFound = true;
                            if (distance < closestDistance) {
                                closestEntity = ent;
                                closestDistance = distance;
                            }
                        }
                    }
                    if (intersectionFound) {
                        generateRenderSelectedEntityBoundingBox(debugRenderInformation, ModelManager::getModel(ECS::registry.get<TransformComponent>(closestEntity).getModelId())->bvhNode, glm::vec3(0.0f, 1.0f, 0.0f), ECS::registry.get<TransformComponent>(closestEntity).transform);
                        
                        selectedEntity = closestEntity;
                        isEntitySelected = true;
                        LOG_MESSAGE(LogLevel::INFO, "Entity Found : " + std::to_string(static_cast<int>(closestEntity)));
                        break;
                    }
                    //--------------------------------------------------------------------------------------------------------------
                }
                else {
                    //std::cout << " Chunk : " << newChunk.x << "," << newChunk.y << "," << newChunk.z << " not in the internal map, stepping again!" << std::endl;
                }
                currentChunk = newChunk;
            }
            else {
                //std::cout << "Still in the same chunk, stepping again" << std::endl;
            }
            index++;
        }

    }

    

}
#endif