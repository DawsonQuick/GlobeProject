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
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        wireframe = !wireframe;
        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
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

    Globe globe;
    globe.generate();



    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

    Camera camera(1);
    LineRenderer lineRenderer(true);


    //RenderUtils renderUtils;

    InstancedRenderingOrchestrator instancedRenderer;

    Skybox skybox;
    skybox.loadSkyBox("./Resources/Skybox/Skybox1");

    Stopwatch<std::chrono::milliseconds> stopWatch;
    while (!glfwWindowShouldClose(window)) {
        LOG_MESSAGE(LogLevel::INFO,"Stress test for logging");
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //Process user peripheral input and update camera
        processInput(window);
        camera.update(window, glfwGetTime());


        ImGui_ImplGlfwGL3_NewFrame();

        /*---------------------------------------------------------------------------------------
         *                                Preformance Gui info
          ---------------------------------------------------------------------------------------*/
        //Ensure that this is rendered first thing to appear at the top of the window
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.3f));  // RGBA: Black with 50% opacity
        ImGui::Begin("Right Content", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Simulation loop exection time: %.3f ms/iteration",dtMrg.getSimulationFrameRate());
        if (ImGui::Button(dtMrg.pauseSimulation() ? "ON" : "OFF")) {
            dtMrg.pauseSimulation() = !dtMrg.pauseSimulation(); // Toggle the state
        }
        ImGui::End();
        ImGui::PopStyleColor();  // Restore the original background color
        //-----------------------------------------------------------------------------------------


        


        /*---------------------------------------------------------------------------------------
        *                                    Terrain Render
          ---------------------------------------------------------------------------------------*/
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        CameraInfo info = camera.getCameraInfo();
        globe.render(model, info.view, info.projection, info.m_CameraPos);
        renderImGui(window,camera);

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

            entt::entity entity = ChunkManager::getSelectedEntity();
            std::stringstream ss;
            ss << "Entity: " << static_cast<int>(entity);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));  // RGBA: Black with 50% opacity
            ImGui::Begin("Entity Properties");

            ImGui::Text(ss.str().c_str());
            ECS::renderEntiryGui(entity);

            //Add some spacing between the content and the close button
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            if(ImGui::Button("Close")) {
                ChunkManager::hasEntitySelected() = false;
            }

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
        //----------------------------------------------------------------------------------------



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