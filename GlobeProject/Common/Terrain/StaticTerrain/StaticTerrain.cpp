#include "StaticTerrain.h"


void renderBVHNode(std::vector<float>& boundBoxRenderInfo, BVHNode* bvhNode, glm::mat4 trans) {
    if (!bvhNode) return;  // Base case: if the node is null, stop recursion

    if (bvhNode->isLeaf()) {
        glm::vec3 color = bvhNode->boundingBox.color;

        glm::vec3 LBB = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.LBB, 1.0));
        glm::vec3 RBB = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.RBB, 1.0));
        glm::vec3 RFB = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.RFB, 1.0));
        glm::vec3 LFB = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.LFB, 1.0));
        glm::vec3 LBT = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.LBT, 1.0));
        glm::vec3 RFT = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.RFT, 1.0));
        glm::vec3 LFT = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.LFT, 1.0));
        glm::vec3 RBT = glm::vec3(trans * glm::vec4(bvhNode->boundingBox.RBT, 1.0));

        std::vector<float> edges = {
            // Bottom square (LBB, LBF, RBB, RBF)
            LBB.x, LBB.y, LBB.z, color.r, color.g, color.b,  RBB.x, RBB.y , RBB.z, color.r, color.g, color.b,
            RBB.x, RBB.y, RBB.z, color.r, color.g, color.b,  RFB.x, RFB.y , RFB.z, color.r, color.g, color.b,
            RFB.x, RFB.y, RFB.z, color.r, color.g, color.b,  LFB.x, LFB.y , LFB.z, color.r, color.g, color.b,
            LFB.x, LFB.y, LFB.z, color.r, color.g, color.b,  LBB.x, LBB.y , LBB.z, color.r, color.g, color.b,

            // Top square (LTB, LTF, RTB, RFT)
            LBT.x, LBT.y, LBT.z, color.r, color.g, color.b,  RBT.x, RBT.y , RBT.z, color.r, color.g, color.b,
            RBT.x, RBT.y, RBT.z, color.r, color.g, color.b,  RFT.x, RFT.y , RFT.z, color.r, color.g, color.b,
            RFT.x, RFT.y, RFT.z, color.r, color.g, color.b,  LFT.x, LFT.y , LFT.z, color.r, color.g, color.b,
            LFT.x, LFT.y, LFT.z, color.r, color.g, color.b,  LBT.x, LBT.y , LBT.z, color.r, color.g, color.b,

            // Vertical edges
            LBB.x, LBB.y, LBB.z, color.r, color.g, color.b,  LBT.x, LBT.y , LBT.z, color.r, color.g, color.b,
            RBB.x, RBB.y, RBB.z, color.r, color.g, color.b,  RBT.x, RBT.y , RBT.z, color.r, color.g, color.b,
            RFB.x, RFB.y, RFB.z, color.r, color.g, color.b,  RFT.x, RFT.y , RFT.z, color.r, color.g, color.b,
            LFB.x, LFB.y, LFB.z, color.r, color.g, color.b,  LFT.x, LFT.y , LFT.z, color.r, color.g, color.b
        };

        boundBoxRenderInfo.insert(boundBoxRenderInfo.end(), edges.begin(), edges.end());
    }
    // Recursively render bounding boxes for the left and right children
    if (bvhNode->leftChild) {
        renderBVHNode(boundBoxRenderInfo, bvhNode->leftChild, trans);
    }
    if (bvhNode->rightChild) {
        renderBVHNode(boundBoxRenderInfo, bvhNode->rightChild, trans);
    }
}


StaticTerrain::~StaticTerrain() {

}
void StaticTerrain::generate() {
    int id = ModelManager::loadModel("./Resources/Models/CrytekSponza/sponza.obj");
	bvhNode = ModelManager::getModel(id)->bvhNode;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, ModelManager::getModel(id)->vertices.size() * sizeof(Vertex), &ModelManager::getModel(id)->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ModelManager::getModel(id)->indices.size() * sizeof(unsigned int), &ModelManager::getModel(id)->indices[0], GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_indicesSize = ModelManager::getModel(id)->indices.size();
    //renderBVHNode(m_BoundingBoxPoints, bvhNode, glm::mat4(50.0f));
    std::cout << "Loaded with " << m_indicesSize << " indices." << std::endl;
}

void StaticTerrain::render(LineRenderer &lineRenderer, glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, m_indicesSize, GL_UNSIGNED_INT, 0);

    //lineRenderer.appendLines(m_BoundingBoxPoints, false);
    //lineRenderer.render(model, view, projection);
}