#include "SimulationManager.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
SimulationManager::SimulationManager(): dtMrg(DataTransferManager::getInstance()){
	prev_time = std::chrono::high_resolution_clock::now();
}
SimulationManager::~SimulationManager() {

}
void SimulationManager::startSimulationThread() {

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	GLFWwindow* windowTest = glfwCreateWindow(1, 1, "Hidden", nullptr, nullptr);
	if (!windowTest) {
		std::cerr << "Failed to create window" << std::endl;
		return;
	}
	// Make the context current
	glfwMakeContextCurrent(windowTest);

	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
	}

	// Initialize compute shader
	ECS::initializeComputeShader();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dis(25.0, 45.0);
	std::uniform_real_distribution<double> dis2(7000.0, 20000.0);
	std::uniform_real_distribution<float> colorDis(0.0, 1.0);

	for (int i = 0; i < 10000; i++) {
		ECS::Entity entity = ECS::create();
		entity.addComponent<PositionComponent>(glm::vec3(0.0f, 0.0f, 0.0f));
		entity.addComponent<TransformComponent>(glm::mat4(1.0));
		entity.addComponent<BoundingBoxComponent>(ECS::createBoundingBox(glm::vec3(colorDis(gen), colorDis(gen), colorDis(gen))));
		//entity.addComponent<CircularOrbitComponent>(dis2(gen), dis(gen) * 3.1415 / 180, dis(gen) * 3.1415 / 180);
		entity.addComponent<CircularOrbitComponent>(dis2(gen), dis(gen) * 3.1415 / 180, 45.0 * 3.1415 / 180);
	}

	while (true) {
		if (dtMrg.pauseSimulation()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		else {
			stopWatch.start();
			/*
			auto currentTime = std::chrono::high_resolution_clock::now();
			auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - prev_time).count();
			float TS = static_cast<float>(elapsed_time) / 1000.0f;
			prev_time = currentTime;
			*/
			//ECS::updatePositions(TS);

			std::vector<RenderTransferData>& tmpBoundingBoxResults = ECS::updateOrbitPositions();
			dtMrg.getDoubleBuffer(DataTransferTypes::TEST)->getInactiveBuffer()->assign(tmpBoundingBoxResults.begin(), tmpBoundingBoxResults.end());
			dtMrg.getDoubleBuffer(DataTransferTypes::TEST)->swapBuffers();
			dtMrg.getSimulationFrameRate() = stopWatch.stopReturn();
		}
	}

}
