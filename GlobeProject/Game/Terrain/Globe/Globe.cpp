#include "Globe.h"
#include "GlobeGui/GlobeGui.h"



Globe::Globe() {

}

Globe::~Globe() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Globe::generate() {
    shaderProgram = Globe::createShaderProgram();

    stopWatch.start();

    //generateSphere(6378.1f, (18 * 4), (18 * 2), vertices, indices);
    /*
    generateSpherifiedCubeSphere(6378.1f, (12 * 4), vertices, indices);

   LOG_MESSAGE(LogLevel::INFO,"Verticies: " + std::to_string(vertices.size()));
   LOG_MESSAGE(LogLevel::INFO,"Indices: " + std::to_string(indices.size()));
    //m_boundingBox = calculateBoundingBox(vertices, size_t(8),size_t(3));

    int tempChunkSize = ChunkManager::getChunkSize();
    std::vector<float> tempVertices; 
    for (int i = 0; i < vertices.size(); i += 8) {
        glm::vec3 tmpPoint = glm::vec3(vertices.at(i + 0), vertices.at(i + 1), vertices.at(i + 2));
        tempVertices.push_back(vertices.at(i + 0));
        tempVertices.push_back(vertices.at(i + 1));
        tempVertices.push_back(vertices.at(i + 2));
        glm::ivec3 tempChunk = glm::ivec3(std::floor((tmpPoint.x / tempChunkSize) + 0.5), std::floor((tmpPoint.y / tempChunkSize) + 0.5), std::floor((tmpPoint.z / tempChunkSize) + 0.5));
        ChunkManager::addTerrainPointToChunk(tempChunk, tmpPoint);
    }
    */
    int id = ModelManager::loadModel("./Resources/Models/CrytekSponza/sponza.obj");

    bvhNode = ModelManager::getModel(id)->bvhNode;

    LOG_MESSAGE(LogLevel::INFO, "Elapsed time building terrain: " + std::to_string(stopWatch.stopReturn()) + " ms");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, ModelManager::getModel(id)->vertices.size() * sizeof(Vertex), &ModelManager::getModel(id)->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ModelManager::getModel(id)->indices.size() * sizeof(unsigned int), &ModelManager::getModel(id)->indices[0], GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    // Shader uniforms
    glUseProgram(shaderProgram);

    indiceSize = ModelManager::getModel(id)->indices.size();

    vertices.clear();
    indices.clear();

    std::vector<float>().swap(vertices);
    std::vector<unsigned int>().swap(indices);

    //renderBVHNode(m_BoundingBoxPoints, bvhNode, glm::mat4(50.0f));

}


void Globe::render(LineRenderer& render, glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos) {
    glUseProgram(shaderProgram);

    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(globeProperties.lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(globeProperties.lightColor));
    glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(globeProperties.objectColor));

    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");

    glm::mat4 newModel = glm::mat4(50.0f);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(newModel));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indiceSize, GL_UNSIGNED_INT, 0);


    //render.appendLines(m_BoundingBoxPoints, false);
    //render.render(newModel, view, projection);


    globeGuiRender(globeProperties);

}

unsigned int Globe::compileShader(unsigned int type, const char* source) {
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

unsigned int Globe::createShaderProgram() {

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

BoundingBox& Globe::getBoundingBoxInfo() {
    return m_boundingBox;
}

GlobeRenderProperties& Globe::getGlobeRenderProperties() {
    return globeProperties;
}