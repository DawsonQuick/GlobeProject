#pragma once
#ifndef TERRAINCHUNKORCHESTRATOR_H
#define TERRAINCHUNKORCHESTRATOR_H

#include "ChunkInstance/ChunkInstance.h"
#include "./../../../Common/ChunkManager/ChunkManager.h"
#include "GuiInterface/TerrainChunkGuiInterface.h"
#include <memory>
class TerrainChunkOrchestrator {
public:
	TerrainChunkOrchestrator();
	~TerrainChunkOrchestrator();

	void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

private:
   
    std::vector<std::unique_ptr<ChunkInstance>> instances;
    bool isInstanceSet = false;
	unsigned int m_ShaderProgram;
    std::vector<ComputeTerrainGenInfo_OUT> outData;
    const char* vertexShaderSource = R"(
        #version 410 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTex;
        out vec2 TexCoord;
        out vec3 pos;
        void main()
        {
            gl_Position = vec4(aPos, 1.0);
            pos = aPos;
            TexCoord = aTex;

        }
    )";

    const char* tesselationEvalShaderSource = R"(
        #version 410 core
        layout(quads, fractional_odd_spacing, ccw) in;
        uniform sampler2D heightMap;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        in vec2 TextureCoord[];
        out vec2 FragTexCoord; // Add this line
        out vec3 terrainNormal;
        out vec3 terrainGradient;
        out float Height;
        out vec4 normal;
        out vec4 p;
        
        //Prototype
        void unpackData(vec4 packedData, out vec3 normal, out vec3 gradient);

        void main()
        {
    
    
            float u = gl_TessCoord.x;
            float v = gl_TessCoord.y;

            vec2 t00 = TextureCoord[0];
            vec2 t01 = TextureCoord[1];
            vec2 t10 = TextureCoord[2];
            vec2 t11 = TextureCoord[3];

            vec2 t0 = (t01 - t00) * u + t00;
            vec2 t1 = (t11 - t10) * u + t10;
            vec2 texCoord = (t1 - t0) * v + t0;
            
            vec4 packedData = texture(heightMap, texCoord).rgba;
            Height = packedData.a * (50*30) ;

            unpackData(packedData,terrainNormal,terrainGradient);

            FragTexCoord = (t1 - t0) * v + t0;
    
            vec4 p00 = gl_in[0].gl_Position;
            vec4 p01 = gl_in[1].gl_Position;
            vec4 p10 = gl_in[2].gl_Position;
            vec4 p11 = gl_in[3].gl_Position;

            vec4 uVec = p01 - p00;
            vec4 vVec = p10 - p00;
            normal = normalize(vec4(cross(vVec.xyz, uVec.xyz), 0));

            vec4 p0 = (p01 - p00) * u + p00;
            vec4 p1 = (p11 - p10) * u + p10;
            p = (p1 - p0) * v + p0 + normal * Height;

            p.y -= (500 * 30); // Shifting down in Y direction

            gl_Position = projection * view * model * p;
        }

        void unpackData(vec4 packedData, out vec3 terrainNormal, out vec3 gradient) {

            // Extract normal and gradient from RGB channels
            vec3 packedNormalGradient = packedData.rgb;

            // Decode normal and gradient
            terrainNormal = (vec3(
                floor(packedNormalGradient.r / 255.0),
                floor(packedNormalGradient.g / 255.0),
                floor(packedNormalGradient.b / 255.0)
            ) * 2.0) - 1.0; // Convert from [0, 1] to [-1, 1]

            gradient = (vec3(
                mod(packedNormalGradient.r, 255.0),
                mod(packedNormalGradient.g, 255.0),
                mod(packedNormalGradient.b, 255.0)
            ) * 2.0) - 1.0; // Convert from [0, 1] to [-1, 1]
        }
    )";

    const char* tesselationControlShaderSource = R"(
        #version 410 core
        layout(vertices=4) out;
        uniform mat4 model;
        uniform mat4 view;
        uniform int minTessLevel;
        uniform int maxTessLevel;
        uniform float minTessDistance;
        uniform float maxTessDistance;

        in vec2 TexCoord[];
        out
        vec2 TextureCoord[];
        void main()
        {
            gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
            TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

            if (gl_InvocationID == 0)
            {
                int MIN_TESS_LEVEL = minTessLevel;
                int MAX_TESS_LEVEL = maxTessLevel;
                float MIN_DISTANCE = minTessDistance;
                float MAX_DISTANCE = maxTessDistance;

                vec4 eyeSpacePos00 = view * model * gl_in[0].gl_Position;
                vec4 eyeSpacePos01 = view * model * gl_in[1].gl_Position;
                vec4 eyeSpacePos10 = view * model * gl_in[2].gl_Position;
                vec4 eyeSpacePos11 = view * model * gl_in[3].gl_Position;

                // "distance" from camera scaled between 0 and 1
                float distance00 = clamp((abs(eyeSpacePos00.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
                float distance01 = clamp((abs(eyeSpacePos01.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
                float distance10 = clamp((abs(eyeSpacePos10.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
                float distance11 = clamp((abs(eyeSpacePos11.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);

                float tessLevel0 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00));
                float tessLevel1 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01));
                float tessLevel2 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11));
                float tessLevel3 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10));

                gl_TessLevelOuter[0] = tessLevel0;
                gl_TessLevelOuter[1] = tessLevel1;
                gl_TessLevelOuter[2] = tessLevel2;
                gl_TessLevelOuter[3] = tessLevel3;

                gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
                gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
            }
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 410 core
        in float Height;
        in vec2 FragTexCoord; // Add this line
        in vec4 normal;
        in vec4 p;

        in vec3 terrainNormal;
        in vec3 terrainGradient;

        out vec4 FragColor;
        uniform vec3 lightPos;
        uniform float steepnessValue;

        in vec4 EyeSpacePosition; // Add this line
        void main() 
        {
            vec3 normalizedGradient = normalize(terrainGradient);

            float angleX = acos(dot(normalizedGradient, vec3(1.0, 0.0, 0.0)));
            float normalizedAngleX = angleX / (3.14159 / 2.0);

            float angleY = acos(dot(normalizedGradient, vec3(0.0, 1.0, 0.0)));
            float normalizedAngleY = angleY / (3.14159 / 2.0);

            float angleZ = acos(dot(normalizedGradient, vec3(0.0, 0.0, 1.0)));
            float normalizedAngleZ = angleZ / (3.14159 / 2.0);

            float steepness = steepnessValue;

            vec3 color;
            //vec3 mixResult = mix(color2,color1,normalizedAngle - 0.2);

            if((normalizedAngleX > steepness && normalizedAngleY > steepness) || (normalizedAngleX > steepness && normalizedAngleZ > steepness) || (normalizedAngleY > steepness && normalizedAngleZ > steepness) ){
                color = vec3(0.0,0.669,0.063);
            }
            else{
                color = vec3(0.32,0.32,0.32);
            }

            // ambient
            float ambientStrength = 0.3;
            vec3 ambient = ambientStrength * vec3(0.7,0.7,0.7);

            // diffuse 
            vec3 norm = normalize(terrainNormal);
            vec3 lightDir = normalize(lightPos - p.xyz);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * vec3(0.7,0.7,0.7);

            vec3 result = (ambient + diffuse) * color;
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
    GLuint createShaderProgram() {
        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        GLuint tessEvalShader = compileShader(GL_TESS_EVALUATION_SHADER, tesselationEvalShaderSource);
        GLuint tessControlShader = compileShader(GL_TESS_CONTROL_SHADER, tesselationControlShaderSource);

        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glAttachShader(shaderProgram, tessEvalShader);
        glAttachShader(shaderProgram, tessControlShader);
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
        glDeleteShader(tessEvalShader);
        glDeleteShader(tessControlShader);

        return shaderProgram;
    }

};

#endif