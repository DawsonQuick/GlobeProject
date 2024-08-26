#pragma once
#ifndef SPHEREGENERATION_H
#define SPHEREGENERATION_H
#include <vector>
#include <cmath>
#include <random>   // For random number generation

#include "./../Utilities/PerlinNoise/PerlinNoise.h"

const float PI = 3.14159265358979323846f;


// Function to normalize a vector
inline void normalize(float& x, float& y, float& z) {
    float length = std::sqrt(x * x + y * y + z * z);
    if (length > 0.0f) {
        x /= length;
        y /= length;
        z /= length;
    }
}

// Smoothing function to spherify the cube vertices
inline void spherify(float& x, float& y, float& z) {
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;

    x *= std::sqrt(1.0f - (y2 + z2) * 0.5f + (y2 * z2) / 3.0f);
    y *= std::sqrt(1.0f - (z2 + x2) * 0.5f + (z2 * x2) / 3.0f);
    z *= std::sqrt(1.0f - (x2 + y2) * 0.5f + (x2 * y2) / 3.0f);
}

inline void generateSphere(float radius, int sectorCount, int stackCount, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            vertices.push_back(s);
            vertices.push_back(t);
        }
    }

    // generate CCW index list of sphere triangles
    int k1, k2;
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

inline void generateSpherifiedCubeSphere(float radius, int resolution, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    PerlinNoise perlinNoise(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Create a random number generator and distribution
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0, 0.1);

    // Multi-octave noise parameters
    int numOctaves = 12;
    float persistence = 0.75f;

    float x, y, z, nx, ny, nz;
    for (int face = 0; face < 6; ++face) {
        for (int i = 0; i < resolution; ++i) {
            for (int j = 0; j < resolution; ++j) {
                // Map the (i, j) grid onto a [-1, 1] plane
                float a = 2.0f * i / (resolution - 1) - 1.0f;
                float b = 2.0f * j / (resolution - 1) - 1.0f;

                // Depending on the face, we define the x, y, z coordinates
                switch (face) {
                case 0: x = 1; y = a; z = b; break;  // +X face
                case 1: x = -1; y = a; z = b; break; // -X face
                case 2: x = a; y = 1; z = b; break;  // +Y face
                case 3: x = a; y = -1; z = b; break; // -Y face
                case 4: x = a; y = b; z = 1; break;  // +Z face
                case 5: x = a; y = b; z = -1; break; // -Z face
                }

                // Apply spherification to improve vertex distribution
                spherify(x, y, z);

                // Multi-octave Perlin noise for smooth position variation
                float noiseValue = 0.0f;
                float amplitude = 1.5f;
                float frequency = 1.0f;

                for (int octave = 0; octave < numOctaves; ++octave) {
                    float noise = perlinNoise.noise(x * frequency, y * frequency, z * frequency);
                    noiseValue += noise * amplitude;
                    amplitude *= persistence;
                    frequency *= 1.5f;
                }


                // Normalize the vector to project onto a sphere
                normalize(x, y, z);

                // Scale to the desired radius
                x *= radius;
                y *= radius;
                z *= radius;

                // Calculate the current distance from the center (after spherification)
                float currentDistance = std::sqrt(x * x + y * y + z * z);

                // Apply Perlin noise to adjust the distance from the center
                float perturbedDistance = currentDistance + noiseValue * 1000.0f; // Adjust 0.1f to control perturbation strength

                // Normalize the vertex direction
                x /= currentDistance;
                y /= currentDistance;
                z /= currentDistance;

                // Scale to the perturbed distance
                x *= perturbedDistance;
                y *= perturbedDistance;
                z *= perturbedDistance;

                // Vertex position
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);



                // Vertex normal (already normalized)
                nx = x / radius;
                ny = y / radius;
                nz = z / radius;
                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);

                // Texture coordinates (for illustration purposes, simple mapping)
                float s = (a + 1.0f) * 0.5f;
                float t = (b + 1.0f) * 0.5f;
                vertices.push_back(s);
                vertices.push_back(t);
            }
        }
    }

    // Generate indices for the cube faces
    for (int face = 0; face < 6; ++face) {
        int start = face * resolution * resolution;
        for (int i = 0; i < resolution - 1; ++i) {
            for (int j = 0; j < resolution - 1; ++j) {
                int k1 = start + i * resolution + j;
                int k2 = k1 + resolution;

                // Two triangles per quad
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

#endif