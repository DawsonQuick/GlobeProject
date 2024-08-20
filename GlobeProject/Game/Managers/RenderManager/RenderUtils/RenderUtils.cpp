#include "RenderUtils.h"

RenderUtils::RenderUtils() {
	m_shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    generateAndBindBuffers();
    m_NumOfInstances = 1;
}

RenderUtils::~RenderUtils() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_instanceVBO);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteProgram(m_shaderProgram);
}

void RenderUtils::generateAndBindBuffers() {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(glm::vec3), cubeVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(GLuint), cubeIndices.data(), GL_STATIC_DRAW);


    std::vector<RenderTransferData> tmp = { RenderTransferData(glm::scale(glm::mat4(1.0) , glm::vec3(1000)), glm::vec3(1.0f,1.0f,1.0f)) };

    //Set the default to 100 instances
    glGenBuffers(1, &m_instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(RenderTransferData), tmp.data(), GL_DYNAMIC_DRAW);

    for (int i = 0; i < 4; i++) {
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(RenderTransferData), (void*)(i * sizeof(glm::vec4)));
        glEnableVertexAttribArray(2 + i);
        glVertexAttribDivisor(2 + i, 1); // Update per instance
    }

    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(RenderTransferData), (void*)(sizeof(glm::mat4)));
    glEnableVertexAttribArray(6);
    glVertexAttribDivisor(6, 1); // Update per instance


}

void RenderUtils::updateInstanceData(std::vector<RenderTransferData>& updatedData) {
    
    /*for (glm::mat4 mat : updatedData) {
        std::cout << mat[0][0] << " " << mat[1][0] << " " << mat[2][0] << " " << mat[3][0] << std::endl;
        std::cout << mat[0][1] << " " << mat[1][1] << " " << mat[2][1] << " " << mat[3][1] << std::endl;
        std::cout << mat[0][2] << " " << mat[1][2] << " " << mat[2][2] << " " << mat[3][2] << std::endl;
        std::cout << mat[0][3] << " " << mat[1][3] << " " << mat[2][3] << " " << mat[3][3] << std::endl;
        std::cout << "------------------------------------------------------" << std::endl;
    } */
    
    
    if (m_NumOfInstances != updatedData.size()) {
         
        m_NumOfInstances = updatedData.size();
        std::cout << "New Number of instances: " << m_NumOfInstances << std::endl;
        //Rebind the buffer with the new number of instances
        glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, m_NumOfInstances * sizeof(RenderTransferData), nullptr, GL_DYNAMIC_DRAW);


    }

    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    void* ptrInstances = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    if (ptrInstances) {
        std::memcpy(ptrInstances, updatedData.data(), m_NumOfInstances * sizeof(RenderTransferData));
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
    else {
        std::cerr << "Failed to map buffer for updating instance data." << std::endl;
    }
     



}

void RenderUtils::render(glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos) {
    glUseProgram(m_shaderProgram);

    GLuint lightDirLocation = glGetUniformLocation(m_shaderProgram, "lightPos");
    glUniform3fv(lightDirLocation, 1, &lightPos[0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glBindVertexArray(m_vao);
    glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)cubeIndices.size(), GL_UNSIGNED_INT, 0, (GLsizei)m_NumOfInstances);
    glBindVertexArray(0);
    glUseProgram(0);
}