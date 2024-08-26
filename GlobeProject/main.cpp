#include <iostream>
#include "./Common/Utilities/DoubleBuffer/DoubleBuffer.h"
#include "./Common/Utilities/ThreadWrapper/ThreadWrapper.h"

#include "Game/Managers/RenderManager/RenderManager.h"
#include "Game/Managers/SimulationManager/SimulationManager.h"

/*
* Starting point for the program
*/
int main() {

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }
    //Create the simulation and render managers and start them on their own threads
    ThreadWrapper renderManagerThread;
    RenderManager renderManager;

    ThreadWrapper simulationThread;
    SimulationManager simulationManager;

    renderManagerThread.start(&RenderManager::startRenderThread, &renderManager);
    simulationThread.start(&SimulationManager::startSimulationThread, &simulationManager);

    //Wait for the threads to end before shutting down the program
    renderManagerThread.join();
    simulationThread.join();
    
}
