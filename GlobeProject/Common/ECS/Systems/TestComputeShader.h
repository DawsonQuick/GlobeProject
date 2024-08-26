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
    float padding2;      // 4 bytes padding to align to 16 bytes
};
struct BoundingBoxData {
    glm::vec3 RBB;
    float padding1;
    glm::vec3 RBT;
    float padding2;
    glm::vec3 RFT;
    float padding3;
    glm::vec3 RFB;
    float padding4;
    glm::vec3 LBB;
    float padding5;
    glm::vec3 LBT;
    float padding6;
    glm::vec3 LFT;
    float padding7;
    glm::vec3 LFB;
    float padding8;
    glm::vec3 Color;
    float padding9;
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
        glGenBuffers(1, &boundingBoxBuffer);
    }

    ~ComputeShader() {
        glDeleteProgram(shaderProgram);

        glDeleteBuffers(1, &positionBuffer);
        glDeleteBuffers(1, &orbitBuffer);
        glDeleteBuffers(1, &boundingBoxBuffer);
    }

    void performOperations(entt::registry& registry, std::vector<RenderTransferData>& returnRenderData) {
        auto view = registry.view<PositionComponent, CircularOrbitComponent, BoundingBoxComponent, TransformComponent>();
        size_t numberOfEntities = view.size_hint();

        if (prevNumOfEntries != numberOfEntities) {
            positions.reserve(numberOfEntities);
            positions.resize(numberOfEntities);
            orbits.reserve(numberOfEntities);
            orbits.resize(numberOfEntities);
            boundingBoxs.reserve(numberOfEntities);
            boundingBoxs.resize(numberOfEntities);
            prevNumOfEntries = numberOfEntities;


            // Initialize buffers with GL_DYNAMIC_DRAW
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfEntities * sizeof(PositionData), nullptr, GL_DYNAMIC_DRAW);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, orbitBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfEntities * sizeof(OrbitData), nullptr, GL_DYNAMIC_DRAW);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, boundingBoxBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfEntities * sizeof(BoundingBoxData), nullptr, GL_DYNAMIC_DRAW);
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
            boundingBoxs[idx].Color = bbox.Color;
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

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, boundingBoxBuffer);
        void* ptrBB = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
        if (ptrBB) {
            std::memcpy(ptrBB, boundingBoxs.data(), numberOfEntities * sizeof(BoundingBoxData));
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }

        if (isNotBound){
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, orbitBuffer);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, boundingBoxBuffer);
            isNotBound = false;
        }
        dispatchComputeShader(numberOfEntities);
        updateBuffer(numberOfEntities, registry , returnRenderData);
    }

private:
    GLuint shaderProgram;
    unsigned int positionBuffer, orbitBuffer, boundingBoxBuffer;
    std::chrono::high_resolution_clock::time_point last_Sample_Time = std::chrono::high_resolution_clock::now();
    Stopwatch<std::chrono::milliseconds> sw;
    bool isNotBound = true;

    std::vector<PositionData> positions;
    std::vector<OrbitData> orbits;
    std::vector<BoundingBoxData> boundingBoxs;

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


    void updateBuffer(size_t numEntity , entt::registry& registry , std::vector<RenderTransferData>& returnRenderData) {

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, numEntity * sizeof(PositionData), positions.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, boundingBoxBuffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, numEntity * sizeof(BoundingBoxData), boundingBoxs.data());
        
        auto view = registry.view<PositionComponent, CircularOrbitComponent, BoundingBoxComponent, TransformComponent>();
        size_t idx = 0;
        for (auto entity : view) {
            auto& posComp = view.get<PositionComponent>(entity);
            auto& boxComp = view.get<BoundingBoxComponent>(entity);
            auto& transformComp = view.get<TransformComponent>(entity);

            posComp.updatePos(positions[idx].position);
            boxComp.update(boundingBoxs[idx].RBB, boundingBoxs[idx].RBT, boundingBoxs[idx].RFT, boundingBoxs[idx].RFB, boundingBoxs[idx].LBB, boundingBoxs[idx].LBT, boundingBoxs[idx].LFT, boundingBoxs[idx].LFB);

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

            posComp.chunk = positions[idx].chunk;

            returnRenderData.push_back(RenderTransferData(transformComp.updatePosition(positions[idx].position), boundingBoxs[idx].Color));
            ++idx;
        }
    }
    void dispatchComputeShader(size_t numEntities) {
        glUseProgram(shaderProgram);

        GLuint chunkSize = glGetUniformLocation(shaderProgram, "chunkSize");
        glUniform1i(chunkSize, ChunkManager::getChunkSize());
        GLuint numGroupsX = static_cast<GLuint>((numEntities + 255) / 256);
        glDispatchCompute(numGroupsX, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
    }
};

#endif
