#pragma once
#ifndef GROUND_H
#define GROUND_H
#include <iostream>
#include <GL/glew.h>
#include "./../../../Common/Stopwatch/Stopwatch.h"
#include "./../../../Common/TerrainGeneration/PlanarTerrain/PlanarTerrain.h"
#include "./../../../Common/Vendor/glm/glm.hpp"
#include "./../../../Common/Vendor/glm/gtc/matrix_transform.hpp"
#include "./../../../Common/Vendor/glm/gtc/type_ptr.hpp"
#include "./../../../Common/ModelManager/ModelLoading/BoundingBox/BoundingBoxGeneration.h"
#include "./../../../Common/ChunkManager/ChunkManager.h"
#include "./../../../Common/Logger/Logger.h"

struct PlanarTerrainRenderProperties {
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    glm::vec3 objectColor;

    PlanarTerrainRenderProperties() {
        lightPos = glm::vec3(10000000.2f, 1.0f, 2.0f);
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        objectColor = glm::vec3(0.8f, 0.8f, 0.8f);
    }
};
#pragma optimize("", off) 
class PlanarTerrain
{
public:
    PlanarTerrain();
    ~PlanarTerrain();
    void generate();
    void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos);
    BoundingBox& getBoundingBoxInfo();
    PlanarTerrainRenderProperties& getPlanarTerrainRenderProperties();
private:
    unsigned int VBO, VAO, EBO;
    unsigned int shaderProgram;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    Stopwatch<std::chrono::milliseconds> stopWatch;
    PlanarTerrainRenderProperties planarTerrainProperties;
    BoundingBox m_boundingBox;
    unsigned int compileShader(unsigned int type, const char* source);
    unsigned int createShaderProgram();


    const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 nNormal;
out vec3 FragPos;

void main()
{
    // Transform the position to world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Calculate the normal in world space
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    nNormal = normalize(normalMatrix * aNorm);
    
    // Apply projection and view to position
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

    const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 nNormal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // Ambient lighting
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(nNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Combine results and set fragment color
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

};
#endif
#pragma optimize("", on) 