#pragma once
#ifndef GLOBE_H
#define GLOBE_H
#include <iostream>
#include <GL/glew.h>
#include "./../../../Common/Utilities/Stopwatch/Stopwatch.h"
#include "./../../../Common/SphereGeneration/SphereGeneration.h"
#include "./../../../Common/Vendor/glm/glm.hpp"
#include "./../../../Common/Vendor/glm/gtc/matrix_transform.hpp"
#include "./../../../Common/Vendor/glm/gtc/type_ptr.hpp"
#include "./../../../Common/ModelManager/ModelLoading/BoundingBox/BoundingBoxGeneration.h"
#include "./../../../Common/ChunkManager/ChunkManager.h"
#include "./../../../Common/Terrain/TerrainUtils/TerrainUtils.h"
#include "./../../../Common/Logger/Logger.h"

struct GlobeRenderProperties {
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    glm::vec3 objectColor;

    GlobeRenderProperties() {
        lightPos = glm::vec3(10000000.2f, 1.0f, 2.0f);
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        objectColor = glm::vec3(0.8f, 0.8f, 0.8f);
    }
};

class Globe
{
public:
	Globe();
	~Globe();
	void generate();
	void render(LineRenderer& render, glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos);
    BoundingBox& getBoundingBoxInfo();
    GlobeRenderProperties& getGlobeRenderProperties();
private:
	unsigned int VBO, VAO, EBO;
    unsigned int shaderProgram;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    int indiceSize;

    Stopwatch<std::chrono::milliseconds> stopWatch;
    GlobeRenderProperties globeProperties;
    BoundingBox m_boundingBox;
    unsigned int compileShader(unsigned int type, const char* source);
    unsigned int createShaderProgram();

    BVHNode* bvhNode;
    
    const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 ViewPos; // Pass the view position to fragment shader

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
    // Pass the view position (camera position) in world space
    ViewPos = vec3(view[3]);
}
)";

    const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 ViewPos; // Received from vertex shader

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

};
#endif
