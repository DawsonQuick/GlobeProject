#include "RenderManager.h"
unsigned int SCR_WIDTH = 2560;
unsigned int SCR_HEIGHT = 1440;


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    int iconified = glfwGetWindowAttrib(window, GLFW_ICONIFIED);
    if (!iconified) {
        SCR_WIDTH = width;
        SCR_HEIGHT = height;
    }

}
bool wireframe = false;
void toggleWireFrame() {
        wireframe = !wireframe;
        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
}

GLFWwindow* window;

RenderManager::RenderManager() : dtMrg(DataTransferManager::getInstance()) {

}
RenderManager::~RenderManager() {

}
int RenderManager::startRenderThread() {
    // Create a GLFWwindow object
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Sphere", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
    }
    glEnable(GL_DEPTH_TEST);

    Scene::getInstance().setSceneWindow(window);

    Globe globe;
    globe.generate();

    //PlanarTerrain planarTerrain;
    //planarTerrain.generate();

    //TerrainChunkOrchestrator terrainChunkOrchestrator;

    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

    LineRenderer lineRenderer(true);
    
    
    IO::addKeyToCheckFor("Toggle Log Screen", { GLFW_KEY_TAB }, [&]() { Logger::toggleImGuiLogger(); },IO::OnPress, IO::KeyboardInput, true);
    IO::addKeyToCheckFor("Pause Simulation Loop", { GLFW_KEY_LEFT_CONTROL, GLFW_KEY_P }, [&]() { dtMrg.pauseSimulation() = !dtMrg.pauseSimulation(); }, IO::OnPress, IO::KeyboardInput,false);
    IO::addKeyToCheckFor("Toggle WireFrame", { GLFW_KEY_V }, [&]() { toggleWireFrame(); }, IO::OnPress, IO::KeyboardInput,false);
    IO::addKeyToCheckFor("Toggle ChunkManager Debug Mode", { GLFW_KEY_LEFT_CONTROL, GLFW_KEY_C }, [&]() { ChunkManager::getIsDebugEnabled() = !ChunkManager::getIsDebugEnabled(); }, IO::OnPress, IO::KeyboardInput,false);
    IO::addKeyToCheckFor("Play Test Audio", { GLFW_KEY_LEFT_CONTROL , GLFW_KEY_LEFT_SHIFT, GLFW_KEY_A }, [&]() {AudioManager::playSound("./Resources/Audio/WeaponHit.mp3"); }, IO::OnPress, IO::KeyboardInput, false);
    IO::addKeyToCheckFor("Play Test Audio2", { GLFW_KEY_LEFT_CONTROL , GLFW_KEY_LEFT_SHIFT, GLFW_KEY_F }, [&]() {AudioManager::playSound("./Resources/Audio/Fireball.mp3"); }, IO::OnPress, IO::KeyboardInput, false);

    //RenderUtils renderUtils;

    InstancedRenderingOrchestrator instancedRenderer;

    Skybox skybox;
    skybox.loadSkyBox("./Resources/Skybox/Skybox1");

    Stopwatch<std::chrono::milliseconds> stopWatch;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplGlfwGL3_NewFrame();

        /*---------------------------------------------------------------------------------------
         *                                Preformance Gui info
          ---------------------------------------------------------------------------------------*/
        //Ensure that this is rendered first thing to appear at the top of the window
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.3f));  // RGBA: Black with 50% opacity
        ImGui::Begin("Right Content", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Simulation loop exection time: %.3f ms/iteration",dtMrg.getSimulationFrameRate());
        ImGui::End();
        ImGui::PopStyleColor();  // Restore the original background color
        //-----------------------------------------------------------------------------------------


        renderImGui(window);

        if (Scene::getInstance().isActiveCameraSet()) {
            IO::processWindowUpdate(window, glfwGetTime());
            CameraInfo info = Scene::getInstance().getActiveCamera()->getCameraInfo();



            /*---------------------------------------------------------------------------------------
            *                                    Terrain Render
              ---------------------------------------------------------------------------------------*/
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            globe.render(lineRenderer,model, info.view, info.projection, info.m_CameraPos);
            //planarTerrain.render(glm::mat4(1.0f), info.view, info.projection, info.m_CameraPos);
            //terrainChunkOrchestrator.render(glm::mat4(1.0f), info.view, info.projection);
            

            skybox.RenderSkyBox(info.view, info.projection);
            //---------------------------------------------------------------------------------------




            /*---------------------------------------------------------------------------------------
            *                                 Chunk Manager Render
              ---------------------------------------------------------------------------------------*/
            if (ChunkManager::getIsDebugEnabled()) {
                lineRenderer.appendLines(ChunkManager::getDebugRenderInformation(), ChunkManager::getForceRefresh());
                lineRenderer.render(glm::mat4(1.0f), info.view, info.projection);
            }

            /*
            * If there is a Entity currently selected render the interactable gui of the entitiy
            */
            if (ChunkManager::hasEntitySelected()) {
                ECS::Entity& entity = Scene::getInstance().getEntity(ChunkManager::getSelectedEntity());
                std::stringstream ss;
                ss << "Entity: " << static_cast<int>(entity.getRawEntity());
                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));  // RGBA: Black with 50% opacity
                ImGui::Begin("Entity Properties", &ChunkManager::hasEntitySelected());

                ImGui::Text(ss.str().c_str());
                entity.renderControls();

                ImGui::End();
                ImGui::PopStyleColor();  // Restore the original background color
            }
            //---------------------------------------------------------------------------------------



            /*---------------------------------------------------------------------------------------
            *                                     Entity Render
              ---------------------------------------------------------------------------------------*/
              //renderUtils.updateInstanceData(dtMrg.getDoubleBuffer(DataTransferTypes::TEST)->getActiveBuffer());
              //renderUtils.render(info.view, info.projection , globe.getGlobeRenderProperties().lightPos);
            instancedRenderer.updateAndRender(dtMrg.getDoubleBuffer(DataTransferTypes::TEST)->getActiveBuffer(), info.view, info.projection, globe.getGlobeRenderProperties().lightPos, globe.getGlobeRenderProperties().lightColor);
            //instancedRenderer.updateAndRender(dtMrg.getDoubleBuffer(DataTransferTypes::TEST)->getActiveBuffer(), info.view, info.projection, planarTerrain.getPlanarTerrainRenderProperties().lightPos, planarTerrain.getPlanarTerrainRenderProperties().lightColor);
            //----------------------------------------------------------------------------------------
        }


        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
      

        glfwSwapBuffers(window);
        
    }

    dtMrg.terminationSignal() = true; //Send termination signal to the simulation thread
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}