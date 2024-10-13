#pragma once
#include <string>
#include <map>
#include <vector>
#include <GL/glew.h>
#include <iostream>

#include "./StaticTerrain/StaticTerrain.h"
#include "./InstancedTerrain/InstancedTerrain.h"

#include "./../../Common/Vendor/glm/glm.hpp"
#include "./../../Common/OpenGLUtilities/LineRenderer/LineRenderer.h"

/*
* Terrain will be broken into two different sections
*	
*	Static geometry, which will be all in one buffer and be renderered at the same time
*		-Note, this type will be used for geometry that is unique (ie. only a single instance)
*			-Reasoning for this, there is no need to add another opengl call for each unique geometry. Just throw it all into one buffer
* 
*	Intanced geometry, which will be for any gemoetry that is repeated within a scene.
*		-There can be multiple instanced of Instanced Geometry.
*		  Which means there will need to be multiple opengl calls for each set up instanced gemoetry
* 
* 
*	Both types of geometry , should used the same shader to simplify the logic and to reduce the draw calls.
*	Unique information will be tranferred in the unique buffers of the geometry
*/
class TerrainManager {
public:
	TerrainManager();
	~TerrainManager();
	void render(LineRenderer& render, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void setStaticTerrain(StaticTerrain staticTerrain);

private:
	StaticTerrain m_StaticTerrain;
	std::vector<InstancedTerrain> instancedTerrain;

    unsigned int shaderProgram;

    unsigned int createShaderProgram();
    unsigned int compileShader(unsigned int type, const char* source);


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