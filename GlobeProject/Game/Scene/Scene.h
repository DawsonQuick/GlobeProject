#pragma once
#include <entt/entt.hpp>
#include <unordered_map>
#include "./../../Common/ECS/ECSWrapper.h"
#include "./../../Common/Camera/Camera.h"
#include "./../../Common/IO/IO.h"
class Scene {
public:
    //--------------------------------------------------------------------------
    // Singleton Logic
    //--------------------------------------------------------------------------
        // Delete copy constructor and assignment operator to ensure singleton behavior
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        // Method to get the singleton instance
        static Scene& getInstance() {
            static Scene instance; // Guaranteed to be destroyed and instantiated on first use
            return instance;
        }
    //--------------------------------------------------------------------------
    // 
    //--------------------------------------------------------------------------
    // Entity Logic
    //--------------------------------------------------------------------------
        // Method to add an existing ECS::Entity and associate it with an entt::entity
        void addEntity(entt::entity entity, const ECS::Entity& ecsEntity) {
            entityMap[entity] = ecsEntity; // Map the entt::entity to the ECS::Entity
        }

        // Method to retrieve an existing ECS::Entity from the map
        ECS::Entity& getEntity(entt::entity entity) {
            return entityMap.at(entity); // Throws an exception if entity not found
        }

        // Optional: Method to get the total number of entities
        size_t entityCount() const {
            return entityMap.size();
        }

        std::unordered_map<entt::entity, ECS::Entity>& getEntites() {
            return entityMap;
        }
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Camera Logic
    //--------------------------------------------------------------------------

        void addCamera(const std::string& id, std::unique_ptr<Camera> camera) {
            sceneCameraMap[id] = std::move(camera); // Transfer ownership

            if (activeCamera == nullptr) {
                activeCamera = sceneCameraMap[id].get();
                IO::setActiveCamera(activeCamera);
            }
        }

        void makeCameraActive(const std::string& id) {
            if (activeCamera != nullptr) {
                //TODO: If active camera is set, need to wipe out keybinds and such to rebind new ones
                //      Then the new camera can be set
            }
            else {
                auto it = sceneCameraMap.find(id);
                if (it != sceneCameraMap.end()) {
                    activeCamera = it->second.get(); // Return raw pointer
                }
            }
        }

        //Get a list of all camera's for the scene
        const std::unordered_map<std::string, std::unique_ptr<Camera>>& getCameras() const {
            return sceneCameraMap;
        }

        bool isActiveCameraSet() {
            return (activeCamera != nullptr);
        }

        Camera* getActiveCamera() {
            return activeCamera;
        }


    //--------------------------------------------------------------------------
    //
        //Remove all things from the current scene
        void clear() {
            for (auto it = entityMap.begin(); it != entityMap.end(); ++it) {
                ECS::destroyEntity(it->second);
            }

            entityMap.clear();
            ChunkManager::clearEntityToChunkMap();
        }

        void setSceneWindow(GLFWwindow* window) {
            activeWindow = window;
        }

        GLFWwindow* getCurrentSceneWindow() {
            return activeWindow;
        }


private:
    // Private constructor to prevent instantiation
    Scene() = default;


    GLFWwindow* activeWindow;
    // Static map to hold entities
    std::unordered_map<entt::entity, ECS::Entity> entityMap;
    std::unordered_map<std::string, std::unique_ptr<Camera>> sceneCameraMap;

    Camera* activeCamera = nullptr;

};