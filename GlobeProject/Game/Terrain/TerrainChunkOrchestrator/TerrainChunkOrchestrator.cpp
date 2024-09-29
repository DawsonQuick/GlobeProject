#include "TerrainChunkOrchestrator.h"

TerrainChunkOrchestrator::TerrainChunkOrchestrator()
{
	m_ShaderProgram = createShaderProgram();
	

}

TerrainChunkOrchestrator::~TerrainChunkOrchestrator() {
	
}

void TerrainChunkOrchestrator::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
	static glm::vec3 lightPos = glm::vec3(0.0, 100000.0, 100000.0);
	static int minTessLevel = 4;
	static int maxTessLevel = 1000;
	static float minTessDistance = 30;
	static float maxTessDistance = 10000;
	static float steepnessValue = 0.5;

	float generationTotalTime = 0.0f; //ms
	float processAndSetupTotalTime = 0.0f; //ms

	if (!isInstanceSet) {
		ComputeShaderPlanarTerrainGeneration tempTerrainGenerator;
		
		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < 4; i++) {
				stopWatchA.start();
				ComputeTerrainGenInfo_IN inData(69420, 512, j, i, 1000, 5, 100.0f, 0.25f, 0.0001f, 3.5f, 100.0f);
				ComputeTerrainGenInfo_OUT outdata;
				tempTerrainGenerator.performOperations(inData, outdata);
				generationTotalTime += stopWatchA.stopReturn();
				stopWatchB.start();
				std::unique_ptr<ChunkInstance> tmp = std::make_unique<ChunkInstance>(outdata, inData.xOffset, inData.yOffset);
				instances.push_back(std::move(tmp));
				outdata.free();
				processAndSetupTotalTime += stopWatchB.stopReturn();
			}
		}
		tempTerrainGenerator.~ComputeShaderPlanarTerrainGeneration();
		isInstanceSet = true;

		LOG_MESSAGE(LogLevel::INFO, "Total Generation time: " + std::to_string(generationTotalTime) + " ms");
		LOG_MESSAGE(LogLevel::INFO, "Total process and setup time: " + std::to_string(processAndSetupTotalTime) + " ms");
	}

	if (isInstanceSet) {
			
				glUseProgram(m_ShaderProgram);
				glUniform3fv(glGetUniformLocation(m_ShaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
				unsigned int modelLoc = glGetUniformLocation(m_ShaderProgram, "model");
				unsigned int viewLoc = glGetUniformLocation(m_ShaderProgram, "view");
				unsigned int projLoc = glGetUniformLocation(m_ShaderProgram, "projection");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			
				glUniform1i(glGetUniformLocation(m_ShaderProgram,"heightMap"), 0);
				glUniform1i(glGetUniformLocation(m_ShaderProgram,"normalMap"), 1);
				glUniform1i(glGetUniformLocation(m_ShaderProgram,"gradientMap"), 2);
				glUniform1i(glGetUniformLocation(m_ShaderProgram, "minTessLevel"), minTessLevel);
				glUniform1i(glGetUniformLocation(m_ShaderProgram, "maxTessLevel"), maxTessLevel);
				glUniform1f(glGetUniformLocation(m_ShaderProgram, "minTessDistance"), minTessDistance);
				glUniform1f(glGetUniformLocation(m_ShaderProgram, "maxTessDistance"), maxTessDistance);
				glUniform1f(glGetUniformLocation(m_ShaderProgram, "steepnessValue"), steepnessValue);
				
			for (int i = 0; i < instances.size(); i++) {
				auto& instance = instances[i];
				instance->render();

			}

			//renderGui(lightPos);
			ImGui::Begin("Right Content", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
			if (ImGui::TreeNode("Terrain Chunk Properties")) {
				ImGui::DragFloat3("Light Position", glm::value_ptr(lightPos), 1000.0f);
				ImGui::DragInt("minTessLevel", &minTessLevel, 10);
				ImGui::DragInt("maxTessLevel", &maxTessLevel, 10);
				ImGui::DragFloat("minTessDistance", &minTessDistance, 10.0f);
				ImGui::DragFloat("maxTessDistance", &maxTessDistance, 1000.0f);
				ImGui::DragFloat("steepnessValue", &steepnessValue, 0.01f);
				ImGui::TreePop();
			}
			ImGui::End();

	}
	

}