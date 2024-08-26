#pragma once
#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <utility>
#include <cmath>
#include "./../../Vendor/glm/glm.hpp"
#include "./../../Vendor/glm/gtc/matrix_transform.hpp"
#include "./../../Utilities/OrbitalCalculation/SphericalOrbit/SphericalOrbitCalculator.h"
#include "./../../Vendor/imgui/imgui.h"

struct IRenderable {
    virtual void guiRender() = 0; // Pure virtual function
};

struct PositionComponent : public IRenderable {
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

    void guiRender() override {
        ImGui::Text("Entity Postion: x:%.3f , y:%.3f , z:%.3f", position.x, position.y, position.z);
    }

};

struct VelocityComponent {
    glm::vec3 velocity;
    VelocityComponent() = default;
    VelocityComponent(const VelocityComponent&) = default;
    VelocityComponent(const glm::vec3& newVelocity)
        :velocity(newVelocity) {}
};

struct TransformComponent : public IRenderable {
    glm::mat4 transform;
    float scale;
    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::mat4& newTransform)
        :transform(newTransform) {
        scale = 300;
        transform = glm::scale(transform, glm::vec3(scale));
    }


     const glm::mat4& updatePosition(const glm::vec3 &position) {
        transform[3][0] = position.x;
        transform[3][1] = position.y;
        transform[3][2] = position.z;
        
        return transform;
    }

     void guiRender() override {
         if (ImGui::TreeNode("Entity Transform Matrix")) {
             for (int row = 0; row < 4; ++row) {
                 ImGui::Text("Row %d: %.3f, %.3f, %.3f, %.3f",
                     row + 1,
                     transform[row][0], transform[row][1], transform[row][2], transform[row][3]);
             }
             if (ImGui::DragFloat("Scale", &scale, 1.0f)) {
                 transform[0][0] = scale;
                 transform[1][1] = scale;
                 transform[2][2] = scale;
             }


             ImGui::TreePop();
         }
     }
};

struct BoundingBoxComponent : public IRenderable {
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


    void guiRender() override {
        if (ImGui::TreeNode("Entity Bounding Box Info")) {
            // Convert glm::vec3 to ImVec4 (ImGui's color type)
            ImVec4 colorVec4(Color.r, Color.g, Color.b, 1.0f);

            // Use ImGui::ColorEdit3 to display a color picker
            if (ImGui::ColorEdit3("Color", (float*)&colorVec4)) {
                // Update glm::vec3 with the new color values
                Color.r = colorVec4.x;
                Color.g = colorVec4.y;
                Color.b = colorVec4.z;
            }
            ImGui::TreePop();
        }
    }

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
struct CircularOrbitComponent : public IRenderable {
    double radius;
    float tempRadius = static_cast<float>(radius);
    double inclination;
    float tempInclination = static_cast<float>(inclination);
    double omega;
    float tempOmega = static_cast<float>(omega);
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

    void guiRender() override {
        if (ImGui::TreeNode("Entity Orbital Information")) {
            if (ImGui::DragFloat("Radius", &tempRadius, 1.0f)) {
                radius = static_cast<double>(tempRadius);
                // This block will be executed if the slider changes the value of myFloat
                period = (orbitalPeriod(6.67430e-11, 5.972e24, radius) * 100000);
            }
            if(ImGui::DragFloat("Inclination", &tempInclination,0.01,0.0, 6.283)) {
                inclination = static_cast<double>(tempInclination);
            }
            if (ImGui::DragFloat("Omega", &tempOmega, 0.01, 0.0, 6.283)) {
                omega = static_cast<double>(tempOmega);
            }
            ImGui::Text("Period: %.3f", period);
            ImGui::TreePop();
        }
    }
};

#endif