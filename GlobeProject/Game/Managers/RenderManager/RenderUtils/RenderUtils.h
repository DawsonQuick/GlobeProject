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
    RenderUtils();
    ~RenderUtils();

    void generateAndBindBuffers();
    void render(glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos);
    void updateInstanceData(std::vector<RenderTransferData>& updatedData);

private:
    std::vector<glm::mat4> m_instanceMatrices;

    size_t m_NumOfInstances;

    GLuint m_vao, m_vbo, m_ebo;
    GLuint m_instanceVBO;
    GLuint m_shaderProgram;
    
    // Cube vertices
    std::vector<glm::vec3> cubeVertices = {
    {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},
    { 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},
    { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},
    {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},

    // Back face
    {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},
    { 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},
    { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},
    {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},

    // Left face
    {-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f},
    {-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f},
    {-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f},
    {-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f},

    // Right face
    { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},
    { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},
    { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f},
    { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f},

    // Top face
    {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f},
    { 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f},
    { 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f},
    {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f},

    // Bottom face
    {-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f},
    { 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f},
    { 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f},
    {-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f},
    };

    // Cube indices
    std::vector<GLuint> cubeIndices = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };
    


    // Shader source code
    const char* vertexShaderSource = R"(
    #version 460 core
    layout(location = 0) in vec3 inPosition;
    layout(location = 1) in vec3 inNormal;
    layout(location = 2) in mat4 instanceMatrix;
    layout(location = 6) in vec3 inColor;

    uniform mat4 view;
    uniform mat4 projection;

    out vec3 fragColor; // Pass color to fragment shader
    out vec3 nNormal;
    out vec3 FragPos;
    out vec3 ViewPos; // Pass the view position to fragment shader

    void main()
    {
    gl_Position = projection * view *  instanceMatrix * vec4(inPosition, 1.0);
    fragColor = inColor;

    FragPos = vec3(instanceMatrix * vec4(inPosition, 1.0));
    
    // Transform normal to world space
    nNormal = normalize(mat3(transpose(inverse(instanceMatrix))) * inNormal);

    // Pass the view position (camera position) in world space
    ViewPos = vec3(view[3]);
    }
    )";

    const char* fragmentShaderSource = R"(
    #version 460 core

    out vec4 FragColor;

    in vec3 fragColor; // Received from vertex shader
    in vec3 nNormal;
    in vec3 FragPos;
    in vec3 ViewPos; // Received from vertex shader

    uniform vec3 lightPos;

    void main()
    {
    // ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * vec3(0.8,0.8,0.8);

    // diffuse 
    vec3 norm = normalize(nNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.8,0.8,0.8);

    // specular
    float specularStrength = 0.8;
    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);

    vec3 result = (ambient + diffuse + specular) * fragColor;
    FragColor = vec4(result, 1.0);

    }
    )";

    // Utility function to compile shaders
    GLuint compileShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "Shader compilation failed: " << infoLog << std::endl;
        }

        return shader;
    }

    // Utility function to create shader program
    GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        GLint success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "Shader linking failed: " << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }

};


#endif