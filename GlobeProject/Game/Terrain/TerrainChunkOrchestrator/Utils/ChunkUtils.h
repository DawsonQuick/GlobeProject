#pragma once
#ifndef CHUNKUTILS_H
#define CHUNKUTILS_H
#include <vector>
#include <GL/glew.h>
#include "./../../../../Common/Vendor/stb_image/stb_image.h"
#include "./../../../../Common/Logger/Logger.h"
#include "./../../../../Common/Vendor/glm/glm.hpp"

// Convert glm::ivec3 to 565 format
inline uint16_t rgbTo565(const glm::ivec3& color) {
    return ((color.r >> 3) << 11) | ((color.g >> 2) << 5) | (color.b >> 3);
}

// Get the index of the color in the block
inline uint16_t getColorIndex(const glm::ivec3& color, const glm::ivec3& color0, const glm::ivec3& color1) {
    // Convert colors to 565 format
    uint16_t c0 = rgbTo565(color0);
    uint16_t c1 = rgbTo565(color1);
    uint16_t c = rgbTo565(color);

    uint16_t r0 = (c0 >> 11) & 0x1F;
    uint16_t g0 = (c0 >> 5) & 0x3F;
    uint16_t b0 = c0 & 0x1F;
    uint16_t r1 = (c1 >> 11) & 0x1F;
    uint16_t g1 = (c1 >> 5) & 0x3F;
    uint16_t b1 = c1 & 0x1F;
    uint16_t r = (c >> 11) & 0x1F;
    uint16_t g = (c >> 5) & 0x3F;
    uint16_t b = c & 0x1F;

    // Compute dot products
    int32_t dot0 = (r - r0) * (r - r0) + (g - g0) * (g - g0) + (b - b0) * (b - b0);
    int32_t dot1 = (r - r1) * (r - r1) + (g - g1) * (g - g1) + (b - b1) * (b - b1);

    return dot0 < dot1 ? 0 : 1;
}

// Compress a 4x4 block of glm::ivec3 colors to DXT1 format
inline void compressDXT1Block(const std::vector<glm::ivec3>& block, uint8_t* outData) {
    glm::ivec3 minColor = block[0];
    glm::ivec3 maxColor = block[0];

    for (const auto& color : block) {
        if (color.r < minColor.r) minColor.r = color.r;
        if (color.g < minColor.g) minColor.g = color.g;
        if (color.b < minColor.b) minColor.b = color.b;

        if (color.r > maxColor.r) maxColor.r = color.r;
        if (color.g > maxColor.g) maxColor.g = color.g;
        if (color.b > maxColor.b) maxColor.b = color.b;
    }

    uint16_t min565 = rgbTo565(minColor);
    uint16_t max565 = rgbTo565(maxColor);

    outData[0] = min565 & 0xFF;
    outData[1] = (min565 >> 8) & 0xFF;
    outData[2] = max565 & 0xFF;
    outData[3] = (max565 >> 8) & 0xFF;

    uint8_t indices[16] = { 0 };
    for (size_t i = 0; i < 16; ++i) {
        indices[i] = getColorIndex(block[i], minColor, maxColor);
    }

    outData[4] = (indices[0] << 0) | (indices[1] << 2) | (indices[2] << 4) | (indices[3] << 6);
    outData[5] = (indices[4] << 0) | (indices[5] << 2) | (indices[6] << 4) | (indices[7] << 6);
    outData[6] = (indices[8] << 0) | (indices[9] << 2) | (indices[10] << 4) | (indices[11] << 6);
    outData[7] = (indices[12] << 0) | (indices[13] << 2) | (indices[14] << 4) | (indices[15] << 6);
}










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

/*
inline void bindMapTexture(unsigned int& gradientMapBuffer, const std::vector<glm::vec4>& data, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, gradientMapBuffer); // Bind the gradient map texture object
    if (!data.empty()) {
        // Set the texture data and generate mipmaps
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data.data());
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
*/

inline void bindHeightTexture(unsigned int& heightMapBuffer, const std::vector<float>& data, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, heightMapBuffer); // Bind the gradient map texture object
    if (!data.empty()) {
        // Set the texture data and generate mipmaps
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, data.data());
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

inline void bindRGBTexture(unsigned int& mapBuffer, const std::vector<glm::ivec4>& data, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, mapBuffer);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Compress data
    std::vector<uint8_t> compressedData((width / 4) * (height / 4) * 8);
    size_t offset = 0;

    for (int y = 0; y < height; y += 4) {
        for (int x = 0; x < width; x += 4) {
            std::vector<glm::ivec3> block(16);
            for (int j = 0; j < 4; ++j) {
                for (int i = 0; i < 4; ++i) {
                    block[j * 4 + i] = data[(y + j) * width + (x + i)];
                }
            }
            compressDXT1Block(block, &compressedData[offset]);
            offset += 8;
        }
    }
    // Upload compressed texture data
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, width, height, 0, compressedData.size(), compressedData.data());

    // Optionally generate mipmaps if needed
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}



#endif