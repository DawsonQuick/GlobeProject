#pragma once
#ifndef ASSIMPLOADER_H
#define ASSIMPLOADER_H
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "./../../../../Common/Vendor/imgui/imgui.h"
#include "./../../../../Common/Vendor/glm/glm.hpp"
#include "./../../../../Common/Vendor/glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class AssimpLoader
{
private: 
   static Assimp::Importer importer;
public:
    AssimpLoader() {

    }
    static void loadModel(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const std::string filePath);
};
#endif
