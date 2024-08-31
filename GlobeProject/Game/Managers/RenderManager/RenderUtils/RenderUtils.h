#pragma once
#ifndef RENDERUTILS_H
#define RENDERUTILS_H

#include <GL/glew.h>
#include <vector>
#include <iostream>
#include "./../../../../Common/Vendor/glm/glm.hpp"
#include "./../../../../Common/Vendor/glm/gtc/matrix_transform.hpp"
#include "./../../../../Common/Vendor/glm/gtc/type_ptr.hpp"
#include "./../../../../Common/OpenGLUtilities/UniveralStructs.h"
#include "./../../../../Common/SphereGeneration/SphereGeneration.h"
#include "./../../../../Common/ModelManager/ModelManager.h"

// Draw commands
struct DrawElementsIndirectCommand {
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
};

struct InstanceData {
    glm::mat4 modelMatrix;
    GLuint baseVertex;
};

class RenderUtils {
public:
    RenderUtils(int modelId);
    ~RenderUtils();

    void generateAndBindBuffers();
    void render(glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos);
    void updateInstanceData(std::vector<RenderTransferData>& updatedData);

private:
    std::vector<glm::mat4> m_instanceMatrices;

    size_t m_NumOfInstances;

    GLuint m_vao, m_vbo, m_ebo;
    GLuint m_instanceVBO;


    // Cube vertices
    std::vector<Vertex> m_Vertices;
    // Cube indices
    std::vector<unsigned int> m_Indices;


};

#endif