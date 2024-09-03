#pragma optimize("", off)
#ifndef PLANARTERRAIN_H
#define PLANARTERRAIN_H
#include <vector>
#include <cmath>
#include <random>   // For random number generation
#include "./../../../Common/Utilities/PerlinNoise/PerlinNoise.h"
#include "./../../../Common/Vendor/glm/glm.hpp"
#include<chrono>

struct PlanarTerrainGenerationSettings {

    int seed;
    int chunkSize;
    int resolution;

    int numOctaves;
    float initialAmplitude;
    float amplitudeStepSize;
    float initalFrequency;
    float frequencyStepSize;
    float scale;


    //Default constructor
    PlanarTerrainGenerationSettings()
        : seed(0), chunkSize(0), resolution(0), numOctaves(0), initialAmplitude(0), amplitudeStepSize(0),
        initalFrequency(0), frequencyStepSize(0), scale(0)
    {

    }

    PlanarTerrainGenerationSettings(int seed, int chunkSize, int resolution, int numOctaves, float initAmp, float ampStepSize, float initFreq, float freqStepSize, float scale)
        : seed(seed) , chunkSize(chunkSize), resolution(resolution) , numOctaves(numOctaves) , initialAmplitude(initAmp), amplitudeStepSize(ampStepSize),
          initalFrequency(initFreq) , frequencyStepSize(freqStepSize) , scale(scale)
    {

    }




};

inline void generatePlanarTerrain(const PlanarTerrainGenerationSettings& settings, std::vector<float>& vertices, std::vector<unsigned int>& indices) {

    PerlinNoise perlin(settings.seed);

    // Clear the vectors
    vertices.clear();
    indices.clear();

    // Calculate the step size based on resolution
    float stepSize = static_cast<float>(settings.chunkSize) / (settings.resolution - 1);

    // Generate vertices
    for (int y = -settings.resolution / 2; y < settings.resolution / 2; ++y) {
        for (int x = -settings.resolution / 2; x < settings.resolution / 2; ++x) {
            float posX = x * stepSize;
            float posY = y * stepSize;

            // Multi-octave Perlin noise for smooth position variation
            float noiseValue = 0.0f;
            float amplitude = settings.initialAmplitude;
            float frequency = settings.initalFrequency;  // Start with a very low frequency for a large hill
            float scale = settings.scale;

            // Calculate noise value
            for (int octave = 0; octave < settings.numOctaves; ++octave) {
                float noise = perlin.noise2D(x * frequency, y * frequency);
                noiseValue += noise * amplitude;
                // Decrease amplitude and increase frequency for more detail
                amplitude *= settings.amplitudeStepSize;
                frequency *= settings.frequencyStepSize;
            }

            // Calculate the final frequency used
            frequency /= std::pow(2.0f, settings.numOctaves);

            // Calculate heights of neighboring vertices for normal calculation
            float heightL = perlin.noise2D((x - 1) * frequency, y * frequency) * scale;
            float heightR = perlin.noise2D((x + 1) * frequency, y * frequency) * scale;
            float heightD = perlin.noise2D(x * frequency, (y - 1) * frequency) * scale;
            float heightU = perlin.noise2D(x * frequency, (y + 1) * frequency) * scale;

            // Calculate the normal using the cross product of the slopes
            glm::vec3 normal;
            normal.x = heightL - heightR;
            normal.y = 2.0f; // Smoothing factor for the Y axis
            normal.z = heightD - heightU;
            normal = glm::normalize(normal);

            // Add vertex position (x, height, y)
            vertices.push_back(posX);
            vertices.push_back(noiseValue * scale);
            vertices.push_back(posY);

            // Add normal (nx, ny, nz)
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }

    // Generate indices for the grid
    for (int y = 0; y < settings.resolution - 1; ++y) {
        for (int x = 0; x < settings.resolution - 1; ++x) {
            int topLeft = y * settings.resolution + x;
            int topRight = topLeft + 1;
            int bottomLeft = (y + 1) * settings.resolution + x;
            int bottomRight = bottomLeft + 1;

            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

#endif

#pragma optimize("", on)