#pragma once
#ifndef CHUNKUTILS_H
#define CHUNKUTILS_H
#include <vector>
#include <GL/glew.h>
#include "./../../../../Common/Vendor/stb_image/stb_image.h"
#include "./../../../../Common/Logger/Logger.h"
#include "./../../../../Common/Vendor/glm/glm.hpp"

inline void generateVertices(unsigned int m_VOABuffer ,int width, int height, unsigned int rez , int xOffset, int yOffset) {

    std::vector<float> vertices;
    float xOffsetPos = (xOffset * width);
    float yOffsetPos = (yOffset * height);
    for (unsigned i = 0; i <= rez - 1; i++) {
        for (unsigned j = 0; j <= rez - 1; j++) {
            float xPos = -width / 2.0f + width * i / (float)rez + xOffsetPos;
            float yPos = -height / 2.0f + height * j / (float)rez + yOffsetPos;

            vertices.push_back(xPos); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(yPos); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            // Adjust next vertex positions
            vertices.push_back(xPos + width / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(yPos); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            vertices.push_back(xPos); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(yPos + height / (float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v

            vertices.push_back(xPos + width / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(yPos + height / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v
        }
    }
    unsigned int terrainVBO;
    glBindVertexArray(m_VOABuffer);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

inline void bindMapTexture(unsigned int& gradientMapBuffer, const std::vector<glm::vec4>& data, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, gradientMapBuffer); // Bind the gradient map texture object
    if (!data.empty()) {
        // Set the texture data and generate mipmaps
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data.data());
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        LOG_MESSAGE(LogLevel::INFO, "Success: GradientMap loaded into a GLTextureBuffer");
    }
    else {
        LOG_MESSAGE(LogLevel::WARN, "Warning: GradientMap data is empty, texture not updated.");
    }

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture when done
}

#endif