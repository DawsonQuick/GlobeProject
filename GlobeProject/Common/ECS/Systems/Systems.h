#pragma once
#ifndef SYSTEM_H
#define SYSTEM_H
#include <entt/entt.hpp>
#include <iostream>
#include <chrono>
#include <execution>
#include "./../Components/Components.h"
#include "./../../Utilities/Stopwatch/Stopwatch.h"
#include "./../../OpenGLUtilities/UniveralStructs.h"
#include "./ECSRegistry/Registry.h"
#include "TestComputeShader.h"
#include "./../../../Game/Managers/DataTransferManager/DataTransferManager.h"
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
    float scale;
    ivec3 chunk;
    float doRender;
};

layout (local_size_x = 256) in;

// Buffer bindings
layout (std430, binding = 0) buffer PositionBuffer {
    PositionData positions[];
};

layout (std430, binding = 1) buffer OrbitBuffer {
    OrbitData orbits[];
};

uniform int chunkSize;
uniform ivec3 currentCameraChunk;

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

    float dist = distance(vec3(positions[idx].chunk) , vec3(currentCameraChunk));

    if(dist > 100){
        positions[idx].doRender = 0.0;
    }else{
        positions[idx].doRender = 1.0;
    }

}


)";
	inline ComputeShader* computeShader = nullptr;
    inline std::vector<RenderTransferDataTemp> boundingBoxRenderInfo;
	inline void initializeComputeShader() {
		computeShader = new ComputeShader(computeShaderSource);
	}

    inline std::vector<RenderTransferDataTemp>& updateOrbitPositions() {
        boundingBoxRenderInfo.resize(0);
		computeShader->performOperations(registry, boundingBoxRenderInfo);
        return boundingBoxRenderInfo;
	}

}
#endif
