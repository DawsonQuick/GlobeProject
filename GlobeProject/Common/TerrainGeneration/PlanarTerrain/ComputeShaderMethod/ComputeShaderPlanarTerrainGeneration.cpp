#include "ComputeShaderPlanarTerrainGeneration.h"
#include <GL/glew.h>

//Constructor
ComputeShaderPlanarTerrainGeneration::ComputeShaderPlanarTerrainGeneration() {
    shaderProgram = glCreateProgram();
    compileAndAttachShader(computeShaderSource);

    glGenBuffers(1, &ssboComputeInfo);
    glGenBuffers(1, &ssboOutput);
}


//Deconstructor
ComputeShaderPlanarTerrainGeneration::~ComputeShaderPlanarTerrainGeneration() {
    glDeleteProgram(shaderProgram);

    glDeleteBuffers(1, &ssboComputeInfo);
    glDeleteBuffers(1, &ssboOutput);
}

//Main call point for executing the compute shader
void ComputeShaderPlanarTerrainGeneration::performOperations(ComputeTerrainGenInfo_IN& settings , ComputeTerrainGenInfo_OUT& outData) {


   

    if (previousPSize != settings.p.size()) { 
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboComputeInfo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ComputeTerrainGenInfo_IN) + settings.p.size() * sizeof(int), NULL, GL_DYNAMIC_DRAW);
        previousPSize = settings.p.size();
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboComputeInfo);
    void* ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(ComputeTerrainGenInfo_IN), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    memcpy(ptr, &settings, sizeof(ComputeTerrainGenInfo_IN));
    memcpy(static_cast<char*>(ptr) + sizeof(ComputeTerrainGenInfo_IN), settings.p.data(), settings.p.size() * sizeof(int));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboComputeInfo);

    if (settings.chunkSize != previousChunkSize) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboOutput);
        glBufferData(GL_SHADER_STORAGE_BUFFER, settings.chunkSize * settings.chunkSize * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboOutput);


        previousChunkSize = settings.chunkSize;
    }


    dispatchComputeShader(settings.chunkSize);
    unpackData(outData, settings.chunkSize);


}

void ComputeShaderPlanarTerrainGeneration::unpackData(ComputeTerrainGenInfo_OUT& outData, int chunkSize) {
    // Allocate space in vectors to avoid reallocation during memcpy
    outData.outData.reserve(chunkSize * chunkSize);
    outData.outData.resize(chunkSize * chunkSize);

    outData.width = chunkSize;
    outData.height = chunkSize;

    // Copy height data
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboOutput);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (chunkSize * chunkSize) * sizeof(glm::vec4), outData.outData.data());


}

//Function to actually call and execute the compute shader
void ComputeShaderPlanarTerrainGeneration::dispatchComputeShader(size_t chunkSize) {
    glUseProgram(shaderProgram);

    // Calculate number of work groups for both dimensions
    GLuint numWorkGroupsX = (chunkSize + 9) / 10;  // Adjust as per your work group size
    GLuint numWorkGroupsY = (chunkSize + 9) / 10;

    glDispatchCompute(numWorkGroupsX, numWorkGroupsY, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

// Standard template for compiling and binding the compute shader code to a buffer
void ComputeShaderPlanarTerrainGeneration::compileAndAttachShader(const std::string& shaderCode) {
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);

    const char* shaderCodeCStr = shaderCode.c_str();
    glShaderSource(shader, 1, &shaderCodeCStr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        LOG_MESSAGE(LogLevel::ERROR, "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" + std::string(infoLog));
    }

    glAttachShader(shaderProgram, shader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        LOG_MESSAGE(LogLevel::ERROR, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string(infoLog));
    }

    glDeleteShader(shader);
}