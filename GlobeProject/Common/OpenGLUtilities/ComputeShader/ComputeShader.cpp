#pragma once
#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H
#include <GL/glew.h>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <stdexcept>

class ComputeShader {
public:
    ComputeShader() {
        shaderProgram = glCreateProgram();
    }

    ComputeShader(const std::string& shaderCode) {
        shaderProgram = glCreateProgram();
        compileAndAttachShader(shaderCode);
    }

    ~ComputeShader() {
        glDeleteProgram(shaderProgram);
    }

    void performOperations(std::vector<float>& data, float xOffset, float yOffset) {
        if (data.size() % 4 != 0) {
            throw std::runtime_error("Data size must be a multiple of 4.");
        }

        GLuint buffer;
        createBuffer(buffer, data, GL_DYNAMIC_DRAW);
        dispatchComputeShader(buffer, data.size() / 4, xOffset, yOffset);
        updateBuffer(buffer, data);
        glDeleteBuffers(1, &buffer);
    }

private:
    GLuint shaderProgram;

    void compileAndAttachShader(const std::string& shaderCode) {
        GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
        std::cout << "Attempting to assign shader program: " << shaderCode.c_str() << std::endl;

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

    void updateBuffer(GLuint buffer, std::vector<float>& data) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(float), data.data());
    }

    void dispatchComputeShader(GLuint buffer, size_t numVertices, float xOffset, float yOffset) {
        glUseProgram(shaderProgram);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);


        // Pass offsets to the shader
        GLuint xOffsetLoc = glGetUniformLocation(shaderProgram, "xOffset");
        GLuint yOffsetLoc = glGetUniformLocation(shaderProgram, "yOffset");

        if (xOffsetLoc == -1 || yOffsetLoc == -1) {
            std::cerr << "ERROR::SHADER::UNIFORM::NOT_FOUND" << std::endl;
        }

        glUniform1f(xOffsetLoc, xOffset);
        glUniform1f(yOffsetLoc, yOffset);

        GLuint numGroupsX = static_cast<GLuint>(ceil(numVertices / 256.0));
        glDispatchCompute(numGroupsX, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
    }
};

#endif
