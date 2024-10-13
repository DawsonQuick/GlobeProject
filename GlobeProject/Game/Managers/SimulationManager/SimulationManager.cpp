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
	std::uniform_real_distribution<double> dis2(20000.0, 45000.0);
	std::uniform_real_distribution<float> colorDis(0.0, 1.0);

	/* --------------------------------------------------------------------------------------------------------------------------------------------------------
	*																Temp code for generating models
	   -------------------------------------------------------------------------------------------------------------------------------------------------------- */
	/*
	int totalSuccess = 0;
	int totalFailed = 0;

	int totalVertices = 0;
	int totalIndices = 0;

	for (int j = 0; j < 100; j++) {
		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		generateSpherifiedCubeSphere(100, (2 * 2), vertices, indices);
		int id = ModelManager::loadModel(vertices, indices, "astroid"+std::to_string(j));

		auto model2 = ModelManager::getModel(id);
		if (!model2) {
			totalFailed++;
			return;
		}
		totalSuccess++;
		for (int i = 0; i < 50; i++) {
			totalVertices += vertices.size();
			totalIndices += indices.size();

			ECS::Entity entity = ECS::create();
			entity.addComponent<PositionComponent>(glm::vec3(0.0f, 0.0f, 0.0f));
			entity.addComponent<TransformComponent>(glm::mat4(1.0),id,glm::vec3(colorDis(gen), colorDis(gen), colorDis(gen)));
			//entity.addComponent<CircularOrbitComponent>(dis2(gen), dis(gen) * 3.1415 / 180, dis(gen) * 3.1415 / 180);
			entity.addComponent<CircularOrbitComponent>(dis2(gen), dis(gen) * 3.1415 / 180, 45.0 * 3.1415 / 180);
			
			Scene::getInstance().addEntity(entity.getRawEntity(), entity);
		}

	}
	
	LOG_MESSAGE(LogLevel::INFO,"Total Success: " + std::to_string(totalSuccess));
	LOG_MESSAGE(LogLevel::INFO,"Total Failed: " + std::to_string(totalFailed));
	LOG_MESSAGE(LogLevel::INFO,"Total Vertices: " + std::to_string(totalVertices));
	LOG_MESSAGE(LogLevel::INFO,"Total Indices: " + std::to_string(totalIndices));
	*/
	/* --------------------------------------------------------------------------------------------------------------------------------------------------------*/

	while (true) {

		if (dtMrg.terminationSignal()) {
			break;
		}
		//------------------------------------------------------------------
		//   Conditional Checks before running the simulation loop
		//------------------------------------------------------------------
		if (dtMrg.pauseSimulation()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		else if (!(ECS::registry.storage<entt::entity>().in_use())) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		//------------------------------------------------------------------
		//If all conditions are met, run the simulation loop
		else {
			stopWatch.start();
			std::vector<RenderTransferDataTemp>& tmpBoundingBoxResults = ECS::updateOrbitPositions();
			dtMrg.getDoubleBuffer(DataTransferTypes::TEST)->getInactiveBuffer()->assign(tmpBoundingBoxResults.begin(), tmpBoundingBoxResults.end());
			dtMrg.getDoubleBuffer(DataTransferTypes::TEST)->swapBuffers();
			dtMrg.getSimulationFrameRate() = stopWatch.stopReturn();
		}
	}

}
