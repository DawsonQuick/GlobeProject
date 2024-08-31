#include "RenderUtils.h"

RenderUtils::RenderUtils(int modelId) {

    m_Vertices = ModelManager::getModel(modelId)->vertices;
    m_Indices = ModelManager::getModel(modelId)->indices;
   // std::cout << m_Vertices.size() << std::endl;
   // std::cout << m_Indices.size() << std::endl;
    generateAndBindBuffers();
    m_NumOfInstances = 1;
}

RenderUtils::~RenderUtils() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_instanceVBO);
    glDeleteVertexArrays(1, &m_vao);

}

void RenderUtils::generateAndBindBuffers() {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * (8 * sizeof(float)), m_Vertices.data(), GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture coord attribute (Fixed to 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

    std::vector<RenderTransferData> tmp(1, RenderTransferData(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 1.0f)));

    //Set the default to 100 instances
    glGenBuffers(1, &m_instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, tmp.size() * sizeof(RenderTransferData), tmp.data(), GL_DYNAMIC_DRAW);

    for (int i = 0; i < 4; i++) {
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(RenderTransferData), (void*)(i * sizeof(glm::vec4)));
        glEnableVertexAttribArray(3 + i);
        glVertexAttribDivisor(3 + i, 1); // Update per instance
    }

    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(RenderTransferData), (void*)(sizeof(glm::mat4)));
    glEnableVertexAttribArray(7);
    glVertexAttribDivisor(7, 1); // Update per instance
}
void RenderUtils::updateInstanceData( std::vector<RenderTransferData>& updatedData) {
    if (m_NumOfInstances != updatedData.size()) {
         
        m_NumOfInstances = updatedData.size();
        //std::cout << "New Number of instances: " << m_NumOfInstances << std::endl;
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
    

 
    glBindVertexArray(m_vao);
    glDrawElementsInstanced(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0, m_NumOfInstances);
    //glBindVertexArray(0);

}