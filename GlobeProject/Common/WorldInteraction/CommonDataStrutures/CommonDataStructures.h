#pragma once
#include "./../../Vendor/glm/glm.hpp"
#include "./../../OpenGLUtilities/LineRenderer/LineRenderer.h"
#include <vector>

// BoundingBox structure definition
struct BoundingBox {
    glm::vec3 RBB;
    glm::vec3 RBT;
    glm::vec3 RFT;
    glm::vec3 RFB;
    glm::vec3 LBB;
    glm::vec3 LBT;
    glm::vec3 LFT;
    glm::vec3 LFB;

    bool isBoundingBoxSet;
    glm::vec3 boundingBoxColor;
    std::vector<float> renderLines;

    BoundingBox() : isBoundingBoxSet(false) {
        boundingBoxColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }


    void renderBoundingBox(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
        if (isBoundingBoxSet) {
            LineRenderer render(false);
            render.appendLines(renderLines,false);
            render.setColor(boundingBoxColor);
            render.render(model, view, projection);
        }
    }

    void setColor(glm::vec3 color) {
        boundingBoxColor = color;
    }
};

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 end;
    std::vector<float> lineVertices;
};