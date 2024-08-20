#pragma once
#ifndef LINERENDERER_H
#define LINERENDERER_H
#include <vector>
#include <iostream>
#include "./../../Vendor/glm/glm.hpp"
#include "./../../Vendor/glm/gtc/matrix_transform.hpp"
#include "./../../Vendor/glm/gtc/type_ptr.hpp"
#include "./../../Stopwatch/Stopwatch.h"
class LineRenderer {
public:
    LineRenderer(bool isColorsEmbdedded);
    ~LineRenderer();

    void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void appendLines(std::vector<float>& newLines);
    void setColor(glm::vec3 newColor);
private:
    void initSSBO(size_t initialSize);
    void updateSSBO(const std::vector<float>& newLines);

    unsigned int VAO, VBO, shaderProgram;

    unsigned int ssbo;
    float* persistentMappedBuffer;
    size_t maxBufferSize;
    size_t currentBufferSize;


    std::vector<float> trajectoryData;
    glm::vec3 m_currentColor;
    bool isColorsEmbdedded;
    Stopwatch<std::chrono::milliseconds> stopWatch;

    bool m_cachedUniforms;
    unsigned int m_Model, m_View, m_Projection , m_Colors;


    unsigned int compileShader(unsigned int type, const char* source);
    unsigned int createShaderProgram();

    void bindBuffers();

    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main()
        {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec3 color;
        void main()
        {
            FragColor = vec4(color.x, color.y, color.z, 1.0); // white color for the trajectory
        }
    )";

    const char* vertexShaderSourceWithColors = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aCol;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        out vec3 mColor;
        void main()
        {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            mColor = aCol;
        }
    )";

    const char* fragmentShaderSourceWithColors = R"(
        #version 330 core
        out vec4 FragColor;
        in vec3 mColor;
        void main()
        {
            FragColor = vec4(mColor.x, mColor.y, mColor.z, 1.0); // white color for the trajectory
        }
    )";


    const char* vertexShaderSourceSSBO = R"(
        #version 450 core

        layout(std430, binding = 0) buffer LineData {
            float positions[];
        };

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec4 vertexColor;

        void main() {
            // Each vertex has 3 components (x, y, z), so the vertex ID maps to positions in the buffer
            int idx = gl_VertexID * 3;

            vec4 position = vec4(positions[idx], positions[idx + 1], positions[idx + 2], 1.0);
            gl_Position = projection * view * model * position;

            // Optional: Assign color based on some logic (if colors are not embedded)
            vertexColor = vec4(1.0, 1.0, 1.0, 1.0); // Default to white color
        }
    )";

    const char* fragmentShaderSourceSSBO = R"(
        #version 450 core

        in vec4 vertexColor;
        out vec4 fragColor;

        void main() {
            fragColor = vertexColor;
        }
    )";

    const char* vertexShaderSourceWithColorsSSBO = R"(
                #version 450 core

        layout(std430, binding = 0) buffer LineData {
            float data[];
        };

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec4 vertexColor;

        void main() {
            // Each vertex has 6 components: 3 for position, 3 for color
            int idx = gl_VertexID * 6;

            vec4 position = vec4(data[idx], data[idx + 1], data[idx + 2], 1.0);
            vertexColor = vec4(data[idx + 3], data[idx + 4], data[idx + 5], 1.0);

            gl_Position = projection * view * model * position;
        }
    )";

    const char* fragmentShaderSourceWithColorsSSBO = R"(
        #version 450 core

        in vec4 vertexColor;
        out vec4 fragColor;

        void main() {
            fragColor = vertexColor;
        }
    )";



};
#endif