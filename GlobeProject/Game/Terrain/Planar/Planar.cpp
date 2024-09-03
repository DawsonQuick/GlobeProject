#pragma optimize("", off) 
#include "Planar.h"
#include "PlanarGui/PlanarGui.h"

PlanarTerrain::PlanarTerrain() {

}

PlanarTerrain::~PlanarTerrain() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
void PlanarTerrain::generate() {
    shaderProgram = PlanarTerrain::createShaderProgram();

    stopWatch.start();

    generatePlanarTerrain(PlanarTerrainGenerationSettings(123, 15000 * 1.5, 1000 ,5, 50.0f, 0.1f,0.005f,1.9f,100.0f), vertices, indices);

   LOG_MESSAGE(LogLevel::INFO,"Verticies: " + std::to_string(vertices.size()));
   LOG_MESSAGE(LogLevel::INFO, "Indices: " + std::to_string(indices.size()));
    //m_boundingBox = calculateBoundingBox(vertices, size_t(8), size_t(3));

    int tempChunkSize = ChunkManager::getChunkSize();
    for (int i = 0; i < vertices.size(); i += 3) {
        glm::vec3 tmpPoint = glm::vec3(vertices.at(i + 0), vertices.at(i + 1), vertices.at(i + 2));
        glm::ivec3 tempChunk = glm::ivec3(std::floor((tmpPoint.x / tempChunkSize) + 0.5), std::floor((tmpPoint.y / tempChunkSize) + 0.5), std::floor((tmpPoint.z / tempChunkSize) + 0.5));
        ChunkManager::addTerrainPointToChunk(tempChunk, tmpPoint);
    }
    LOG_MESSAGE(LogLevel::INFO, "Time Elapsed (Planar Terrain Generation) : " + std::to_string(stopWatch.stopReturn()));
    

    glGenVertexArrays(1, &VAO); 
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    // Shader uniforms
    glUseProgram(shaderProgram);




}
void PlanarTerrain::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos) {
    glUseProgram(shaderProgram);

    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(planarTerrainProperties.lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(planarTerrainProperties.lightColor));
    glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(planarTerrainProperties.objectColor));

    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(viewPos));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    planarTerrainGuiRender(planarTerrainProperties);

}

unsigned int PlanarTerrain::compileShader(unsigned int type, const char* source) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);

    int success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return id;
}

unsigned int PlanarTerrain::createShaderProgram() {

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

BoundingBox& PlanarTerrain::getBoundingBoxInfo() {
    return m_boundingBox;
}

PlanarTerrainRenderProperties& PlanarTerrain::getPlanarTerrainRenderProperties() {
    return planarTerrainProperties;
}
#pragma optimize("", on) 