#pragma once
#ifndef MODELMANAGER_H
#define MODELMANAGER_H
#include <iostream>
#include <vector>

#include "./../AssimpLoader/AssimpLoader.h"
#include "./../../OpenGLUtilities/LineRenderer/LineRenderer.h"
#include "./../BoundingBox/BoundingBoxGeneration.h"
struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	BoundingBox boundingBox;
};

class ModelManager {
public:
	ModelManager() {

	}
	~ModelManager() {

	}

	Model loadModel(const std::string modelFilePath);
};

#endif