#pragma once
#include "./../Scene.h"

//This file will include all the ImGui related to managing the scene directly and all objects within

/*
* Create a window that host all the needed ImGui objects for interacting with the scene
*	NOTE: This window will either need to be triggered by some sort of keybind or Top menu button (or both)
* 
* First things to add
*	-Camera Functionality
*		-Create camera
*		-List active camera
*		-List all available camera
*		-The ability to swap between them
*	-Entity Functionality
*		-Create Entity
*			-Need the ability to add components before actually creating the entity
*				-EX. Create new Entity: Entity 1
*						   Component 1: Position
*											- X
*											- Y
*											- Z
*						   Component 2: Transform Data
*											- Model Id
*											- Scale
*											- Transform Matrix
*						   Component 3: Orbital Data
*											- radius
*											- inclination
*											- omega
*											- period
*										
*										Click to add another component
* 
*		-List all entities in the scene
*
*
*/


namespace {
    static int cameraCount = 0;
}

inline void renderSceneGui(GLFWwindow* window){
	ImGui::Text("TODO: Render Scene window");
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Add")) {
                if (ImGui::BeginMenu("Camera")) {
                    if (ImGui::MenuItem("Orbital")) {
                        Scene::getInstance().addCamera("Camera " + std::to_string(cameraCount++), std::make_unique<Camera>(window, CameraType::ORBITAL));
                    }
                    if (ImGui::MenuItem("Free Camera")) {
                        Scene::getInstance().addCamera("Camera " + std::to_string(cameraCount++), std::make_unique<Camera>(window, CameraType::FREECAM));
                    }
                    ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }


}