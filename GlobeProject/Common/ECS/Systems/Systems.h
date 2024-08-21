#pragma once
#ifndef SYSTEM_H
#define SYSTEM_H
#include <entt/entt.hpp>
#include <iostream>
#include <chrono>
#include <execution>
#include "./../Components/Components.h"
#include "./../../Stopwatch/Stopwatch.h"
#include "./../../OpenGLUtilities/UniveralStructs.h"
#include "./ECSRegistry/Registry.h"
#include "TestComputeShader.h"
namespace ECS {

	inline const char* computeShaderSource = R"(
#version 430

const float M_PI = 3.14159265358979323846;

// Function to calculate position in orbit at time t in 3D
vec3 positionInOrbit(float r, float T, float t, float i, float omega) {
    float cosOmega = cos(omega);
    float sinOmega = sin(omega);
    float cosI = cos(i);
    float sinI = sin(i);
    float angle = 2.0 * M_PI * t / T;

    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    float x = r * (cosOmega * cosAngle - sinOmega * sinAngle * cosI);
    float y = r * (sinOmega * cosAngle + cosOmega * sinAngle * cosI);
    float z = r * (sinAngle * sinI);

    return vec3(x, y, z);
}

float customRound(float value) {
    return floor(value + 0.5);
}

// Structure definition outside of the buffer block
struct OrbitData {
    vec4 orbitParams; // Radius, Inclination, Omega, Period
    float currentTime;
    float padding[3];
};

struct PositionData {
    vec3 position;
    float padding;
    ivec3 chunk;
    float padding2;
};

struct BoundingBoxData {
    vec3 RBB;
    vec3 RBT;
    vec3 RFT;
    vec3 RFB;
    vec3 LBB;
    vec3 LBT;
    vec3 LFT;
    vec3 LFB;

    vec3 Color;
    float padding;
};

layout (local_size_x = 256) in;

// Buffer bindings
layout (std430, binding = 0) buffer PositionBuffer {
    PositionData positions[];
};

layout (std430, binding = 1) buffer OrbitBuffer {
    OrbitData orbits[];
};

layout (std430, binding = 2) buffer BoundingBoxBuffer {
    BoundingBoxData bb[];
};

uniform int chunkSize;

void main() {
    uint idx = gl_GlobalInvocationID.x;

    vec4 orbit = orbits[idx].orbitParams;
    float currentTime = orbits[idx].currentTime;
    float radius = orbit.x;
    float inclination = orbit.y;
    float omega = orbit.z;
    float period = orbit.w;
    vec3 newPosition = positionInOrbit(radius, period, currentTime, inclination, omega);

    positions[idx].position = newPosition;
    positions[idx].chunk = ivec3(customRound(newPosition.x/chunkSize),customRound(newPosition.y/chunkSize),customRound(newPosition.z/chunkSize));
    BoundingBoxData bbox = bb[idx];

    bbox.RBB = newPosition + vec3(100.0, -100.0, -100.0);
    bbox.RBT = newPosition + vec3(100.0, 100.0, -100.0);
    bbox.RFT = newPosition + vec3(100.0, 100.0, 100.0);
    bbox.RFB = newPosition + vec3(100.0, -100.0, 100.0);
    bbox.LBB = newPosition + vec3(-100.0, -100.0, -100.0);
    bbox.LBT = newPosition + vec3(-100.0, 100.0, -100.0);
    bbox.LFT = newPosition + vec3(-100.0, 100.0, 100.0);
    bbox.LFB = newPosition + vec3(-100.0, -100.0, 100.0);

    vec3 color = bb[idx].Color;

    bbox.Color = color;
    bb[idx] = bbox;
}


)";
	inline ComputeShader* computeShader = nullptr;
    inline std::vector<RenderTransferData> boundingBoxRenderInfo;
	inline void initializeComputeShader() {
		computeShader = new ComputeShader(computeShaderSource);
	}

    inline std::vector<RenderTransferData>& updateOrbitPositions() {
        boundingBoxRenderInfo.resize(0);
		computeShader->performOperations(registry, boundingBoxRenderInfo);
        return boundingBoxRenderInfo;
	}

	inline std::vector<float> updateBoundingBoxes() {
		auto view = registry.view<BoundingBoxComponent>();
		std::vector<float> boundingBoxRenderInfo;
		for (auto [entity, bbox] : view.each()) {
			const std::vector<float>& tmp = bbox.getRenderData();
			boundingBoxRenderInfo.insert(boundingBoxRenderInfo.end(), tmp.begin(), tmp.end());
			
		}
		
		return boundingBoxRenderInfo;
	}


}
#endif
