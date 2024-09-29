#pragma once
#ifndef INSTANCEDRENDERINGORCHESTRATOR_H
#define INSTANCEDRENDERINGORCHESTRATOR_H

#include <iostream>
#include <map>
#include <memory>
#include "./../RenderUtils/RenderUtils.h"
#include "./../../../../Common/Logger/Logger.h"
#include "./../../../../Common/ECS/ECSWrapper.h"

class InstancedRenderingOrchestrator {
public:
	InstancedRenderingOrchestrator();
	~InstancedRenderingOrchestrator();

	void updateAndRender(std::vector<RenderTransferDataTemp>& instancedModelData, glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos, glm::vec3 lightColor);

private:

	std::map<int, std::unique_ptr<RenderUtils>> modelToRenderer;
	bool hasRendererBeenInititialized;
    GLuint m_shaderProgram;

    // Shader source code
    const char* vertexShaderSource = R"(
    #version 460 core
    layout(location = 0) in vec3 inPosition;
    layout(location = 1) in vec3 inNormal;
    layout(location = 2) in vec2 inTexCoord;
    layout(location = 3) in mat4 instanceMatrix;
    layout(location = 7) in vec3 inColor;

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
    uniform vec3 lightColor;

    void main()
    {
    // ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    vec3 norm = normalize(nNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.8;
    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

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