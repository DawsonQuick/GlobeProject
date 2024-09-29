#pragma once
#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <utility>
#include <cmath>
#include <vector>
#include "./../../Vendor/glm/glm.hpp"
#include "./../../Vendor/glm/gtc/matrix_transform.hpp"
#include "./../../../Common/Utilities/OrbitalCalculation/SphericalOrbit/SphericalOrbitCalculator.h"
#include "./../../Vendor/imgui/imgui.h"



struct IComponent {
public:
    virtual ~IComponent() = default;

    // A virtual method that all components must implement.
    // Could be useful for serialization or debugging.
   // virtual void serialize() const = 0;

    // Optional: A method to identify the type (helpful for reflection-like behavior)
    //virtual const char* getType() const = 0;
};

struct IRenderable : public IComponent {
    virtual void guiRender() = 0; // Pure virtual function
};

struct PositionComponent : public IRenderable, IComponent {
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

struct VelocityComponent : public IComponent {
    glm::vec3 velocity;
    VelocityComponent() = default;
    VelocityComponent(const VelocityComponent&) = default;
    VelocityComponent(const glm::vec3& newVelocity)
        :velocity(newVelocity) {}
};

struct TransformComponent : public IRenderable , IComponent {
    glm::mat4 transform;
    float scale;
    int modelId;

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::mat4& newTransform, int& modelId)
        :transform(newTransform), modelId(modelId) {
        scale = 1;
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

struct BoundingBoxComponent : public IRenderable , IComponent {
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

    void update(const glm::vec3& position, float scale) {
        RBB = position + initialRBB * scale;
        RBT = position + initialRBT * scale;
        RFT = position + initialRFT * scale;
        RFB = position + initialRFB * scale;
        LBB = position + initialLBB * scale;
        LBT = position + initialLBT * scale;
        LFT = position + initialLFT * scale;
        LFB = position + initialLFB * scale;
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

    void printVec3(glm::vec3 vec) {
        std::cout << vec.x << " " << vec.y << " " << vec.z << std::endl;
    }

    BoundingBoxComponent(const BoundingBoxComponent& copy , glm::vec3 color) {
        RBB = copy.RBB;
        RBT = copy.RBT;
        RFT = copy.RFT;
        RFB = copy.RFB;
        LBB = copy.LBB;
        LBT = copy.LBT;
        LFT = copy.LFT;
        LFB = copy.LFB;
        initialRBB = copy.RBB;
        initialRBT = copy.RBT;
        initialRFT = copy.RFT;
        initialRFB = copy.RFB;
        initialLBB = copy.LBB;
        initialLBT = copy.LBT;
        initialLFT = copy.LFT;
        initialLFB = copy.LFB;
        dataGenerated = false;
        Color = color;

    };
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

struct CircularOrbitComponent : public IRenderable , IComponent {
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
                period = (orbitalPeriod(6.67430e-11, 5.972e24, radius) * 10000);
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