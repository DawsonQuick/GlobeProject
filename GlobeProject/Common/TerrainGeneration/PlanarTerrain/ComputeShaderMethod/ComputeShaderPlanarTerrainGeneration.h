#pragma once
#ifndef COMPUTESHADERPLANARTERRAINGENERATION_H
#define COMPUTESHADERPLANARTERRAINGENERATION_H
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <sstream>
#include "./../../../Logger/Logger.h"
#include "./../../../Vendor/glm/glm.hpp"

//Information to feed into the ComputeShader to use for operations
struct ComputeTerrainGenInfo_IN {
	int PERMUTATION_SIZE;
	int xOffset;
	int yOffset;
	
	int chunkSize;
	int numOctaves;
	float initialAmplitude;
	float amplitudeStepSize;
	float initalFrequency;
	float frequencyStepSize;
	float scale;

	std::vector<int> p;

	ComputeTerrainGenInfo_IN(int seed, int PERMUTATION_SIZE, int xOff, int yOff, int chunkSize, int numOctaves
	, float initialAmp, float ampStep, float initialFreq, float freqStep, float scale):
		PERMUTATION_SIZE(PERMUTATION_SIZE), xOffset(xOff), yOffset(yOff), chunkSize(chunkSize), numOctaves(numOctaves),
		initialAmplitude(initialAmp), amplitudeStepSize(ampStep), initalFrequency(initialFreq), frequencyStepSize(freqStep),
		scale(scale)
	{ 
		// Generate permutation table
		std::vector<int> permutation(PERMUTATION_SIZE);
		for (int i = 0; i < PERMUTATION_SIZE; ++i) {
			permutation[i] = i;
		}

		// Shuffle permutation table with the given seed
		std::mt19937 gen(seed);
		std::shuffle(permutation.begin(), permutation.end(), gen);

		// Duplicate permutation table
		p.resize(PERMUTATION_SIZE * 2);
		for (int i = 0; i < PERMUTATION_SIZE; ++i) {
			p[i] = permutation[i];
			p[PERMUTATION_SIZE + i] = permutation[i];
		}
	}

};

//Package of data to read out from the Compute shader
struct ComputeTerrainGenInfo_OUT {
	std::vector<glm::vec4> outData;      // Vector to store height data



	int width, height;

	void free() {
		outData.clear();
		outData.shrink_to_fit();
		std::vector<glm::vec4>().swap(outData);
	}
};

class ComputeShaderPlanarTerrainGeneration {
public:
	ComputeShaderPlanarTerrainGeneration();
	~ComputeShaderPlanarTerrainGeneration();

	void performOperations(ComputeTerrainGenInfo_IN& settings , ComputeTerrainGenInfo_OUT& outData);

private:
	unsigned int shaderProgram;
	unsigned int ssboComputeInfo;
	unsigned int ssboOutput;

	int previousChunkSize = -1; // Initialize with an invalid size
	int previousPSize = -1;

	void compileAndAttachShader(const std::string& shaderCode);
	void unpackData(ComputeTerrainGenInfo_OUT& outData, int numElements);
	void dispatchComputeShader(size_t chunkSize);

	const char* computeShaderSource = R"(
	#version 430

	layout (local_size_x = 10, local_size_y = 10) in;

	layout(std430, binding = 0) buffer ComputeTerrainGenInfo {
		int PERMUTATION_SIZE;
		int xOffset;
		int yOffset;
		int chunkSize;
		int numOctaves;
		float initialAmplitude;
		float amplitudeStepSize;
		float initalFrequency;
		float frequencyStepSize;
		float scale;
		int p[]; // Use an array for the permutation table
	};

	layout(std430, binding = 1) buffer Output {
		vec4 outData[];
	};

	// Function prototypes
	float fade(float t);
	float lerp(float t, float a, float b);
	float grad(int hash, float x, float y);
	float noise2D(float x, float y);
	vec4 encodeData(vec3 normal, vec3 gradient, float height);

	void main() {
		uint xIdx = gl_GlobalInvocationID.x;
		uint yIdx = gl_GlobalInvocationID.y;
		uint idx = yIdx * chunkSize + xIdx;

		float x = (float(xIdx)) + float(xOffset * chunkSize);
		float y = (float(yIdx)) + float(yOffset * chunkSize);

		// Initialize amplitude and frequency
		float amplitude = initialAmplitude;
		float frequency = initalFrequency;
		float noiseValue = 0.0;
		vec3 gradient = vec3(0.0);
		vec3 normal = vec3(0.0);

		// Accumulate noise contributions from multiple octaves
		for (int i = 0; i < numOctaves; ++i) {
			noiseValue += amplitude * noise2D(x * frequency, y * frequency);
			gradient += vec3(noise2D(x * frequency + 0.01, y * frequency) - noise2D(x * frequency - 0.01, y * frequency),
							 noise2D(x * frequency, y * frequency + 0.01) - noise2D(x * frequency, y * frequency - 0.01),
							 0.0);
			amplitude *= amplitudeStepSize;
			frequency *= frequencyStepSize;
		}

		// Normalize gradient and calculate normal
		if (length(gradient) > 0.0) {
			gradient = normalize(gradient);
			normal = cross(vec3(0.0, 0.0, 1.0), gradient);
		}
		outData[idx] = encodeData(normal, gradient, noiseValue);
	}
	
	// Fade function
	float fade(float t) {
		return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
	}

	// Linear interpolation
	float lerp(float t, float a, float b) {
		return a + t * (b - a);
	}

	// Gradient function
	float grad(int hash, float x, float y) {
		int h = hash & 15;
		float u = (h < 8) ? x : y;
		float v = (h < 4) ? y : ((h == 12 || h == 14) ? x : y);
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

	// 2D Perlin noise function
	float noise2D(float x, float y) {
		// Wrap coordinates around the chunk size
		x = mod(x, float(chunkSize));
		y = mod(y, float(chunkSize));

		// Compute unit square coordinates
		int X = int(floor(x)) & 255;
		int Y = int(floor(y)) & 255;

		// Compute the relative coordinates
		x -= floor(x);
		y -= floor(y);

		// Compute fade curves
		float u = fade(x);
		float v = fade(y);

		// Hash coordinates
		int A = p[X] + Y;
		int AA = p[A];
		int AB = p[A + 1];
		int B = p[X + 1] + Y;
		int BA = p[B];
		int BB = p[B + 1];

		// Interpolate between corners
		return lerp(v, lerp(u, grad(p[AA], x, y), grad(p[BA], x - 1.0, y)),
					lerp(u, grad(p[AB], x, y - 1.0), grad(p[BB], x - 1.0, y - 1.0)));
	}

	vec4 encodeData(vec3 normal, vec3 gradient, float height) {
		// Normalize normal and gradient vectors to [0, 1]
		vec3 normalEncoded = (normal + 1.0) * 0.5; // Assuming normal is in the range [-1, 1]
		vec3 gradientEncoded = (gradient + 1.0) * 0.5; // Assuming gradient is in the range [-1, 1]

		// Pack normal and gradient into RGB channels
		vec3 packedNormalGradient = vec3(
			normalEncoded.x * 255.0 + gradientEncoded.x,
			normalEncoded.y * 255.0 + gradientEncoded.y,
			normalEncoded.z * 255.0 + gradientEncoded.z
		);

		// Store height in alpha channel
		vec4 packedData = vec4(packedNormalGradient, height);

		return packedData;
	}

	)";
};

#endif