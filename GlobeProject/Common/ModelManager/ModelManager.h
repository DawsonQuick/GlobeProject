#pragma once
#ifndef MODELMANAGER_H
#define MODELMANAGER_H
#include <iostream>
#include <vector>
#include <map>
#include <functional>

#include "./ModelLoading/AssimpLoader/AssimpLoader.h"
#include "./../OpenGLUtilities/LineRenderer/LineRenderer.h"
#include "./ModelLoading/BoundingBox/BoundingBoxGeneration.h"
#include "./../ECS/Entity.h"
struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	BoundingBoxComponent boundingBox;
};
namespace {
	inline int stringToID(const std::string& str) {
		std::hash<std::string> hashFunction;
		size_t hashValue = hashFunction(str);

		// Convert to a positive integer ID by taking the modulus with the maximum int value
		return static_cast<int>(hashValue % std::numeric_limits<int>::max());
	}
}

namespace ModelManager {

	inline std::map<int, std::shared_ptr<Model>> idToModelMap;
	inline std::mutex modelMapMutex;

	inline int loadModel(const std::string path) {
		std::lock_guard<std::mutex> lock(modelMapMutex);
		int id = stringToID(path);
		auto it = idToModelMap.find(id);
		if (it == idToModelMap.end()) {
			std::shared_ptr<Model> newModel = std::make_shared<Model>();
			AssimpLoader::loadModel(newModel->vertices, newModel->indices, path);
			newModel->boundingBox = calculateBoundingBox(newModel->vertices);
			idToModelMap[id] = newModel;
		}	
			return id;
	}

	/*
	* Overloaded function, assuming that vertices has 8 floats per vertex (3 position, 3 normal, 2 texture corrds) to directly set a model with a tag
	*/
	inline int loadModel(std::vector<float> vertices, std::vector<unsigned int> indices, const std::string name) {
		std::lock_guard<std::mutex> lock(modelMapMutex);
		int id = stringToID(name);
		std::shared_ptr<Model> newModel = std::make_shared<Model>();

		std::vector<Vertex> tempVertexList;
		for (int i = 0; i < vertices.size(); i += 8) {
			Vertex tempVertex;
			tempVertex.position = glm::vec3(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
			tempVertex.normal = glm::vec3(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
			tempVertex.texCoords = glm::vec2(vertices[i + 6], vertices[i + 7]);
			tempVertexList.push_back(tempVertex);
		}
		newModel->vertices = tempVertexList;
		newModel->indices = indices;
		newModel->boundingBox = calculateBoundingBox(tempVertexList);

		idToModelMap[id] = std::move(newModel);
		return id;
	}

	inline std::shared_ptr<Model> getModel(const int id) {
		std::lock_guard<std::mutex> lock(modelMapMutex);
		auto it = idToModelMap.find(id);
		if (it != idToModelMap.end()) {
			//std::cout << "Found model, returning it" << std::endl;
			return it->second;
		}
		else {
			//std::cerr << "No model found!" << std::endl;
			return nullptr; // Or consider throwing an exception
		}
	
	}


	//Remove the model from the internal data store for models
	inline bool removeModel(const int id) {
		std::lock_guard<std::mutex> lock(modelMapMutex);
		auto it = idToModelMap.find(id);
		if (it != idToModelMap.end()) {
			idToModelMap.erase(id);
		}
		else {
			return false;
		}


		return true;
	}

}
#endif