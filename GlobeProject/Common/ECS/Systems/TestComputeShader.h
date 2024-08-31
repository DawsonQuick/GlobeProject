#pragma once
#ifndef COMPUTESHADERTEST_H
#define COMPUTESHADERTEST_H
#include <GL/glew.h>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <entt/entt.hpp>
#include "./../Components/Components.h"
#include "./ECSRegistry/Registry.h"
#include "./../../Stopwatch/Stopwatch.h"
#include "./../../OpenGLUtilities/UniveralStructs.h"
#include "./../../ChunkManager/ChunkManager.h"

struct OrbitData {
    glm::vec4 orbitParams; // Radius, Inclination, Omega, Period
    float currentTime;
    float padding[3];
};
struct PositionData {
    glm::vec3 position; // 12 bytes
    float scale;      // 4 bytes padding to align to 16 bytes
    glm::ivec3 chunk; // 12 bytes
    float doRender;      // 4 bytes padding to align to 16 bytes
};

class ComputeShader {
public:
    ComputeShader() {
        shaderProgram = glCreateProgram();
    }

    ComputeShader(const std::string& shaderCode) {
        shaderProgram = glCreateProgram();
        compileAndAttachShader(shaderCode);

        glGenBuffers(1, &positionBuffer);
        glGenBuffers(1, &orbitBuffer);
    }

    ~ComputeShader() {
        glDeleteProgram(shaderProgram);

        glDeleteBuffers(1, &positionBuffer);
        glDeleteBuffers(1, &orbitBuffer);
    }

    void performOperations(entt::registry& registry, std::vector<RenderTransferDataTemp>& returnRenderData) {
        auto view = registry.view<PositionComponent, CircularOrbitComponent, BoundingBoxComponent, TransformComponent>();
        size_t numberOfEntities = view.size_hint();

        if (prevNumOfEntries != numberOfEntities) {
            positions.reserve(numberOfEntities);
            positions.resize(numberOfEntities);
            orbits.reserve(numberOfEntities);
            orbits.resize(numberOfEntities);
            prevNumOfEntries = numberOfEntities;


            // Initialize buffers with GL_DYNAMIC_DRAW
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfEntities * sizeof(PositionData), nullptr, GL_DYNAMIC_DRAW);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, orbitBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfEntities * sizeof(OrbitData), nullptr, GL_DYNAMIC_DRAW);
        }

        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

        size_t idx = 0;
        
        for (auto entity : view) {
            auto& pos = view.get<PositionComponent>(entity);
            auto& orbit = view.get<CircularOrbitComponent>(entity);
            auto& bbox = view.get<BoundingBoxComponent>(entity);
            auto& trans = view.get<TransformComponent>(entity);

            positions[idx] = { pos.position, trans.scale , pos.chunk , 0.0f};
            orbits[idx] = {
                glm::vec4(
                    static_cast<float>(orbit.radius),
                    static_cast<float>(orbit.inclination),
                    static_cast<float>(orbit.omega),
                    static_cast<float>(orbit.period)
                ),
                static_cast<float>(fmod(seconds, orbit.period))
            };
            ++idx;
        }
        
        // Map and memcpy to update buffer data
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
        void* ptrPos = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
        if (ptrPos) {
            std::memcpy(ptrPos, positions.data(), numberOfEntities * sizeof(PositionData));
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, orbitBuffer);
        void* ptrOrbit = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
        if (ptrOrbit) {
            std::memcpy(ptrOrbit, orbits.data(), numberOfEntities * sizeof(OrbitData));
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }

        if (isNotBound){
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, orbitBuffer);
            isNotBound = false;
        }
        dispatchComputeShader(numberOfEntities);
        updateBuffer(numberOfEntities, registry , returnRenderData);
    }

private:
    GLuint shaderProgram;
    unsigned int positionBuffer, orbitBuffer;
    std::chrono::high_resolution_clock::time_point last_Sample_Time = std::chrono::high_resolution_clock::now();
    Stopwatch<std::chrono::milliseconds> sw;
    bool isNotBound = true;

    std::vector<PositionData> positions;
    std::vector<OrbitData> orbits;

    size_t prevNumOfEntries = 0;

    void compileAndAttachShader(const std::string& shaderCode) {
        GLuint shader = glCreateShader(GL_COMPUTE_SHADER);

        const char* shaderCodeCStr = shaderCode.c_str();
        glShaderSource(shader, 1, &shaderCodeCStr, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        glAttachShader(shaderProgram, shader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(shader);
    }

    void createBuffer(GLuint& buffer, const std::vector<float>& data, GLenum usage) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(float), data.data(), usage);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
    }


    void updateBuffer(size_t numEntity , entt::registry& registry , std::vector<RenderTransferDataTemp>& returnRenderData) {

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, numEntity * sizeof(PositionData), positions.data());
        
        //---------------------------------------------------------------------------------------------------------------
        std::map<int, int> modelIdToVectorPosition;
        int index = 0;
        //---------------------------------------------------------------------------------------------------------------

        auto view = registry.view<PositionComponent, CircularOrbitComponent, BoundingBoxComponent, TransformComponent>();
        size_t idx = 0;
        for (auto entity : view) {
            auto& posComp = view.get<PositionComponent>(entity);
            auto& boxComp = view.get<BoundingBoxComponent>(entity);
            auto& transformComp = view.get<TransformComponent>(entity);

            posComp.updatePos(positions[idx].position);
            boxComp.update(positions[idx].position, positions[idx].scale);

            // Check for chunk changes and update ChunkManager accordingly
            if (posComp.chunk != positions[idx].chunk) {
                if (posComp.chunk == glm::ivec3(999, 999, 999)) {
                    ChunkManager::addEntityToChunk(positions[idx].chunk, entity);
                }
                else {
                    ChunkManager::removeEntityFromChunk(posComp.chunk, entity);
                    ChunkManager::addEntityToChunk(positions[idx].chunk, entity);
                }
            }

            if (positions[idx].doRender == 1.0f) {
                //---------------------------------------------------------------------------------------------------------------
                auto it = modelIdToVectorPosition.find(transformComp.modelId);
                if (it != modelIdToVectorPosition.end()) {
                    //Model is already loaded
                    returnRenderData.at(it->second).instanceInformation.push_back(RenderTransferData(transformComp.updatePosition(positions[idx].position), boxComp.Color));
                }
                else {
                    //Model has not been added to the vector so needs to be added  
                    modelIdToVectorPosition[transformComp.modelId] = index;
                    index++;
                    RenderTransferDataTemp newModelEntry;
                    newModelEntry.modelId = transformComp.modelId;
                    std::vector<RenderTransferData> tempVector;
                    tempVector.push_back(RenderTransferData(transformComp.updatePosition(positions[idx].position), boxComp.Color));
                    newModelEntry.instanceInformation = tempVector;
                    returnRenderData.push_back(newModelEntry);

                }
                //--------------------------------------------------------------------------------------------------------------
            }
            posComp.chunk = positions[idx].chunk;

            //returnRenderData.push_back(RenderTransferData(transformComp.updatePosition(positions[idx].position), boxComp.Color));
            ++idx;
        }
    }
    void dispatchComputeShader(size_t numEntities) {
        glUseProgram(shaderProgram);

        GLuint chunkSize = glGetUniformLocation(shaderProgram, "chunkSize");
        GLuint cameraChunkPosition = glGetUniformLocation(shaderProgram,"currentCameraChunk");
        glm::ivec3 currentCameraChunk = ChunkManager::getCurrentCameraChunk();
        glUniform3i(cameraChunkPosition, currentCameraChunk.x, currentCameraChunk.y, currentCameraChunk.z);

        glUniform1i(chunkSize, ChunkManager::getChunkSize());
        GLuint numGroupsX = static_cast<GLuint>((numEntities + 255) / 256);
        glDispatchCompute(numGroupsX, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
    }
};

#endif
