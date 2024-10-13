#pragma once
#include "./../../../Common/Vendor/glm/glm.hpp"
#include "./../../../Common/OpenGLUtilities/LineRenderer/LineRenderer.h"
#include "./../../../Common/ModelManager/ModelManager.h"
#include <GL/glew.h>

class StaticTerrain {
public:

	StaticTerrain() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
	}

	~StaticTerrain();

	void generate();
	void render(LineRenderer &lineRenderer, glm::mat4 model, glm::mat4 view, glm::mat4 projection);

private:
	unsigned int VBO, VAO, EBO;
	int m_indicesSize;
	std::vector<float> m_BoundingBoxPoints;
	BVHNode* bvhNode;

};