#pragma once
#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <utility>
#include <cmath>
#include "./../../Vendor/glm/glm.hpp"
#include "./../../Vendor/glm/gtc/matrix_transform.hpp"
#include "./../../Utilities/OrbitalCalculation/SphericalOrbit/SphericalOrbitCalculator.h"
struct PositionComponent {
    glm::vec3 position;
    glm::ivec3 chunk;

    PositionComponent() = default;
    PositionComponent(const PositionComponent&) = default;
    PositionComponent(const glm::vec3& newPosition)
        :position(newPosition) , chunk(glm::ivec3(999,999,999)) {}

    void updatePos(const glm::vec3 &newPos) {
        position = newPos;
    }

    void updateChunk(const glm::ivec3 &newChunk) {
        chunk = newChunk;
    }
};

struct VelocityComponent {
    glm::vec3 velocity;

    VelocityComponent() = default;
    VelocityComponent(const VelocityComponent&) = default;
    VelocityComponent(const glm::vec3& newVelocity)
        :velocity(newVelocity) {}
};

struct TransformComponent {
    glm::mat4 transform;

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::mat4& newTransform)
        :transform(newTransform) {
        transform = glm::scale(transform, glm::vec3(100));
    }


     const glm::mat4& updatePosition(const glm::vec3 &position) {
        transform[3][0] = position.x;
        transform[3][1] = position.y;
        transform[3][2] = position.z;
        
        return transform;
    }

     const glm::mat4& updateScale(const glm::vec3& scale) {
        transform = glm::scale(transform, scale);
        return transform;
    }
};

struct BoundingBoxComponent {
    glm::vec3 RBB;
    glm::vec3 RBT;
    glm::vec3 RFT;
    glm::vec3 RFB;
    glm::vec3 LBB;
    glm::vec3 LBT;
    glm::vec3 LFT;
    glm::vec3 LFB;

    glm::vec3 Color;

    std::vector<float> renderData;

    void update(const glm::vec3& tmpRBB,
        const glm::vec3& tmpRBT,
        const glm::vec3& tmpRFT,
        const glm::vec3& tmpRFB,
        const glm::vec3& tmpLBB,
        const glm::vec3& tmpLBT,
        const glm::vec3& tmpLFT,
        const glm::vec3& tmpLFB) {
        RBB = tmpRBB;
        RBT = tmpRBT;
        RFT = tmpRFT;
        RFB = tmpRFB;
        LBB = tmpLBB;
        LBT = tmpLBT;
        LFT = tmpLFT;
        LFB = tmpLFB;
    }

    void setColor(const glm::vec3& color) {
        Color = color;
    }

    const std::vector<float>& getRenderData() {
        return renderData;
    }

    void generateRenderData(float(&arr)[144]) {
        renderData.assign(arr, arr + sizeof(arr) / sizeof(arr[0]));
    }
    BoundingBoxComponent() = default;
    BoundingBoxComponent(const BoundingBoxComponent&) = default;
    BoundingBoxComponent(const glm::vec3& rbb, const glm::vec3& rbt, const glm::vec3& rft, const glm::vec3& rfb,
                         const glm::vec3& lbb, const glm::vec3& lbt, const glm::vec3& lft, const glm::vec3& lfb,
                         const glm::vec3& color)
        : RBB(rbb), RBT(rbt), RFT(rft), RFB(rfb), LBB(lbb), LBT(lbt), LFT(lft), LFB(lfb), Color(color),
          initialRBB(rbb), initialRBT(rbt), initialRFT(rft), initialRFB(rfb),
          initialLBB(lbb), initialLBT(lbt), initialLFT(lft), initialLFB(lfb),
          dataGenerated(false) {}

private:
    glm::vec3 initialRBB, initialRBT, initialRFT, initialRFB;
    glm::vec3 initialLBB, initialLBT, initialLFT, initialLFB;
    bool dataGenerated;
};

struct MeshComponent {
	std::vector<float> verticies;
	std::vector<unsigned int> indicies;

	MeshComponent() = default;
	MeshComponent(const MeshComponent&) = default;
	MeshComponent(const std::vector<float> verticies, const std::vector<unsigned int> indicies)
		:verticies(verticies), indicies(indicies) {}
};

struct LineComponent {
	std::vector<float> verticies;

	LineComponent() = default;
	LineComponent(const LineComponent&) = default;
	LineComponent(const std::vector<float> verticies)
		:verticies(verticies){}
};

struct RayComponent {
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 end;

	RayComponent() = default;
	RayComponent(const RayComponent&) = default;
	RayComponent(const glm::vec3 origin , const glm::vec3 direction, const glm::vec3 end)
		:origin(origin), direction(direction) , end(end) {}
};

struct CircularOrbitComponent {
    double radius;
    double inclination;
    double omega;
    double period;

    glm::vec3 getCurrentOrbitPosition() const {
        std::tuple<double, double, double> tmp = getPositionInOrbitAtCurrentTime(radius, inclination, omega, period);
        return glm::vec3(std::get<0>(tmp), std::get<1>(tmp), std::get<2>(tmp));
    }

    CircularOrbitComponent() = default;
    CircularOrbitComponent(const CircularOrbitComponent&) = default;
    CircularOrbitComponent(double radius, double inclination, double omega)
        : radius(radius), inclination(inclination), omega(omega) {
        period = (orbitalPeriod(6.67430e-11, 5.972e24, radius) * 100000);
    }
};

#endif