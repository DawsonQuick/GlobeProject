#include "ModelManager.h"

Model ModelManager::loadModel(const std::string path) {
	Model newModel;
	AssimpLoader::loadModel(newModel.vertices, newModel.indices, path);
	calculateBoundingBox(newModel.vertices);
	return newModel;
}

