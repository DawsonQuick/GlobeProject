#include "DataTransferManager.h"

DataTransferManager& DataTransferManager::getInstance() {
	static DataTransferManager instance;
	return instance;
}

std::shared_ptr<DoubleBuffer<RenderTransferData>> DataTransferManager::getDoubleBuffer(DataTransferTypes key) {
	return doubleBufferMap.get(key);
}

float& DataTransferManager::getSimulationFrameRate() {
	return simulationFrameRate;
}

bool& DataTransferManager::pauseSimulation() {
	return pauseSimulationState;
}

bool& DataTransferManager::terminationSignal() {
	return terminateSignal;
}

