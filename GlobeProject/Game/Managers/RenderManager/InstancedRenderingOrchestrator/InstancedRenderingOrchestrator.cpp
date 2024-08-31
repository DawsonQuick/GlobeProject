#include "InstancedRenderingOrchestrator.h"
InstancedRenderingOrchestrator::InstancedRenderingOrchestrator() {
	m_shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

}

InstancedRenderingOrchestrator::~InstancedRenderingOrchestrator() {
	glDeleteProgram(m_shaderProgram);
}

void InstancedRenderingOrchestrator::updateAndRender(std::vector<RenderTransferDataTemp>& instancedModelData, glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos, glm::vec3 lightColor) {
	//------------------------------------------------------------------------
	//       Check conditionals to not have redundent rendering
	//------------------------------------------------------------------------
	if (!(ECS::registry.storage<entt::entity>().in_use())) {
		return;
	}
	if (instancedModelData.size() <= 0) {
		return;
	}
	//------------------------------------------------------------------------

	glUseProgram(m_shaderProgram);
	GLuint lightDirLocation = glGetUniformLocation(m_shaderProgram, "lightPos");
	GLuint lightColorLocation = glGetUniformLocation(m_shaderProgram, "lightColor");
	glUniform3fv(lightDirLocation, 1, &lightPos[0]);
	glUniform3fv(lightColorLocation, 1, &lightColor[0]);
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	for (int i = 0; i < instancedModelData.size(); i++) {
		auto it = modelToRenderer.find(instancedModelData[i].modelId);
		if (it != modelToRenderer.end()) {
			//Model already initialized and exist
			modelToRenderer[instancedModelData[i].modelId]->updateInstanceData(instancedModelData[i].getInstanceInformation());
			modelToRenderer[instancedModelData[i].modelId]->render(view, projection, lightPos);
		}
		else {
			auto model = ModelManager::getModel(instancedModelData[i].modelId);
			if (!model) {
				LOG_MESSAGE(LogLevel::WARN,"Model not found for ID: " + std::to_string(instancedModelData[i].modelId) );
				// Handle error, possibly return or continue to avoid the exception
				return;
			}
			if (ModelManager::getModel(instancedModelData[i].modelId)->vertices.size() > 0) {
				LOG_MESSAGE(LogLevel::DEBUG,"Adding new model for ID: " + std::to_string(instancedModelData[i].modelId));
				//Model does not exist yet and needs to be initialized
				modelToRenderer[instancedModelData[i].modelId] = std::make_unique<RenderUtils>(instancedModelData[i].modelId);
			}
		}
	}
	glUseProgram(0);
}