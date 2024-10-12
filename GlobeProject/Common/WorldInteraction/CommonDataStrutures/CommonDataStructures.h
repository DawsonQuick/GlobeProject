#pragma once
#include "./../../Vendor/glm/glm.hpp"
#include "./../../OpenGLUtilities/LineRenderer/LineRenderer.h"
#include <vector>

struct BoundingBox {
    // Corner points for full bounding box rendering
    glm::vec3 RBB, RBT, RFT, RFB; // Right back, top, front, bottom
    glm::vec3 LBB, LBT, LFT, LFB; // Left back, top, front, bottom

    bool isBoundingBoxSet;
    glm::vec3 color;
    std::vector<float> renderLines;

    // AABB for fast calculations
    glm::vec3 min;
    glm::vec3 max;

    BoundingBox() : isBoundingBoxSet(false) {
        color = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    // Initialize using an AABB, calculating corner points for rendering
    void setFromAABB(const glm::vec3& aabbMin, const glm::vec3& aabbMax) {
        min = aabbMin;
        max = aabbMax;

        // Set the 8 corners of the box
        RBB = glm::vec3(max.x, min.y, min.z);
        RBT = glm::vec3(max.x, max.y, min.z);
        RFT = glm::vec3(max.x, max.y, max.z);
        RFB = glm::vec3(max.x, min.y, max.z);
        LBB = glm::vec3(min.x, min.y, min.z);
        LBT = glm::vec3(min.x, max.y, min.z);
        LFT = glm::vec3(min.x, max.y, max.z);
        LFB = glm::vec3(min.x, min.y, max.z);

        // Generate render lines for visualization
        generateRenderLines();
        isBoundingBoxSet = true;
    }

    // Expands the AABB to enclose another AABB
    void expand(const BoundingBox& other) {
        min = glm::min(min, other.min);
        max = glm::max(max, other.max);
        RBB = glm::vec3(max.x, min.y, min.z);
        RBT = glm::vec3(max.x, max.y, min.z);
        RFT = glm::vec3(max.x, max.y, max.z);
        RFB = glm::vec3(max.x, min.y, max.z);
        LBB = glm::vec3(min.x, min.y, min.z);
        LBT = glm::vec3(min.x, max.y, min.z);
        LFT = glm::vec3(min.x, max.y, max.z);
        LFB = glm::vec3(min.x, min.y, max.z);
        generateRenderLines();
    }

    int maxExtent() const {
        glm::vec3 extent = max - min;
        if (extent.x > extent.y && extent.x > extent.z) {
            return 0;  // Split along x-axis
        }
        else if (extent.y > extent.z) {
            return 1;  // Split along y-axis
        }
        else {
            return 2;  // Split along z-axis
        }
    }

    // Set the bounding box directly using corner points
    void setFromCorners(glm::vec3 rbb, glm::vec3 rbt, glm::vec3 rft, glm::vec3 rfb,
        glm::vec3 lbb, glm::vec3 lbt, glm::vec3 lft, glm::vec3 lfb) {
        RBB = rbb; RBT = rbt; RFT = rft; RFB = rfb;
        LBB = lbb; LBT = lbt; LFT = lft; LFB = lfb;

        // Calculate the min and max for the AABB
        min = glm::min(LBB, glm::min(LFT, RFB)); // Use any three corner points
        max = glm::max(RBB, glm::max(RFT, LBT)); // for proper min/max calculation

        generateRenderLines();
        isBoundingBoxSet = true;
    }

    // Create render lines for the 8 corners of the box
    void generateRenderLines() {
        renderLines.clear();
        std::vector<float> edges = {
            // Bottom square (LBB, LBF, RBB, RBF)
            LBB.x, LBB.y, LBB.z, color.r, color.g, color.b,  RBB.x, RBB.y, RBB.z, color.r, color.g, color.b,
            RBB.x, RBB.y, RBB.z, color.r, color.g, color.b,  RFB.x, RFB.y, RFB.z, color.r, color.g, color.b,
            RFB.x, RFB.y, RFB.z, color.r, color.g, color.b,  LFB.x, LFB.y, LFB.z, color.r, color.g, color.b,
            LFB.x, LFB.y, LFB.z, color.r, color.g, color.b,  LBB.x, LBB.y, LBB.z, color.r, color.g, color.b,

            // Top square (LTB, LTF, RTB, RFT)
            LBT.x, LBT.y, LBT.z, color.r, color.g, color.b,  RBT.x, RBT.y, RBT.z, color.r, color.g, color.b,
            RBT.x, RBT.y, RBT.z, color.r, color.g, color.b,  RFT.x, RFT.y, RFT.z, color.r, color.g, color.b,
            RFT.x, RFT.y, RFT.z, color.r, color.g, color.b,  LFT.x, LFT.y, LFT.z, color.r, color.g, color.b,
            LFT.x, LFT.y, LFT.z, color.r, color.g, color.b,  LBT.x, LBT.y, LBT.z, color.r, color.g, color.b,

            // Vertical edges
            LBB.x, LBB.y, LBB.z, color.r, color.g, color.b,  LBT.x, LBT.y, LBT.z, color.r, color.g, color.b,
            RBB.x, RBB.y, RBB.z, color.r, color.g, color.b,  RBT.x, RBT.y, RBT.z, color.r, color.g, color.b,
            RFB.x, RFB.y, RFB.z, color.r, color.g, color.b,  RFT.x, RFT.y, RFT.z, color.r, color.g, color.b,
            LFB.x, LFB.y, LFB.z, color.r, color.g, color.b,  LFT.x, LFT.y, LFT.z, color.r, color.g, color.b
        };
        renderLines = edges;
    }

    void addBoundingBox(std::vector<float>& boundBoxRenderInfo) {
            boundBoxRenderInfo.insert(boundBoxRenderInfo.end(), renderLines.begin(), renderLines.end());
    }

    void setColor(glm::vec3 tempColor) {
        color = tempColor;
        generateRenderLines();
    }
};

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 end;
    std::vector<float> lineVertices;
};