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
    float yaw = 0.0f;   // Rotation around the Y-axis
    float pitch = 0.0f; // Rotation around the X-axis
    float roll = 0.0f;  // Rotation around the Z-axis
    int modelId;
    glm::vec3 color;

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::mat4& newTransform, int& modelId, glm::vec3 color)
        :transform(newTransform), modelId(modelId), color(color) {
        scale = 1;
        transform = glm::scale(transform, glm::vec3(scale));
    }

    int getModelId() {
        return modelId;
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
             // Rotation controls
             if (ImGui::DragFloat("Yaw (Y-axis)", &yaw, 0.1f, -180.0f, 180.0f)) {
                 // Reset the rotation to the new yaw value
                 transform = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f)) * scale;

                 transform[3][3] = 1.0f;
             }
             if (ImGui::DragFloat("Pitch (X-axis)", &pitch, 0.1f, -180.0f, 180.0f)) {
                 // Reset the rotation to the new pitch value
                 transform = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)) * scale;

                 transform[3][3] = 1.0f;
             }
             if (ImGui::DragFloat("Roll (Z-axis)", &roll, 0.1f, -180.0f, 180.0f)) {
                 // Reset the rotation to the new roll value
                 transform = glm::rotate(glm::mat4(1.0f), glm::radians(roll), glm::vec3(0.0f, 0.0f, 1.0f)) * scale;

                 transform[3][3] = 1.0f;
             }


             ImGui::TreePop();
         }
     }

     // Helper function to update the rotation matrix based on Euler angles
     
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