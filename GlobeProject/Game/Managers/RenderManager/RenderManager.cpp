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

    TrajectoryManager trajMrg;
    trajMrg.generate();


    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

    Camera camera(1);
    LineRenderer lineRenderer(true);


    RenderUtils renderUtils;

    Stopwatch<std::chrono::milliseconds> stopWatch;
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        camera.update(window, glfwGetTime());
        ImGui_ImplGlfwGL3_NewFrame();

        ImGui::Begin("Right Content", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Simulation loop exection time: %.3f ms/iteration",dtMrg.getSimulationFrameRate());
        if (ImGui::Button(dtMrg.pauseSimulation() ? "ON" : "OFF")) {
            dtMrg.pauseSimulation() = !dtMrg.pauseSimulation(); // Toggle the state
        }
        ImGui::End();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        CameraInfo info = camera.getCameraInfo();

        globe.render(model, info.view, info.projection, info.m_CameraPos);
       // trajMrg.render(model, info.view, info.projection);
        //info.renderRay();
        renderImGui(window,camera);
        //std::cout << "Active Buffer Size: " << test.size() << std::endl;
        //lineRenderer.appendLines(dtMrg.getDoubleBuffer(DataTransferTypes::TEST)->getActiveBuffer());
        //lineRenderer.render(model, info.view, info.projection);
        renderUtils.updateInstanceData(dtMrg.getDoubleBuffer(DataTransferTypes::TEST)->getActiveBuffer());
        renderUtils.render(info.view, info.projection , globe.getGlobeRenderProperties().lightPos);

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
      

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}