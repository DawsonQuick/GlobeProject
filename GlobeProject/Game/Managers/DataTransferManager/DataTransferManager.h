#pragma once
#include "./../../../Common/Utilities/LockFreeMap/LockFreeMap.h"
#include "./../../../Common/Utilities/DoubleBuffer/DoubleBuffer.h"
#include "./../../../Common/Vendor/glm/glm.hpp"
#include "./../../../Common/OpenGLUtilities/UniveralStructs.h"

enum DataTransferTypes {
	TEST,
};

class DataTransferManager
{
private:
	//LockFreeMap<DataTransferTypes,std::shared_ptr<DoubleBuffer<float>>> doubleBufferMap;
	LockFreeMap<DataTransferTypes,std::shared_ptr<DoubleBuffer<RenderTransferData>>> doubleBufferMap;
	float simulationFrameRate;
	bool pauseSimulationState;

	DataTransferManager() {
		//std::shared_ptr <DoubleBuffer<float>> tmp = std::make_shared<DoubleBuffer<float>>(1);
		//doubleBufferMap.write(DataTransferTypes::TEST, tmp);

		std::shared_ptr <DoubleBuffer<RenderTransferData>> tmpTransform = std::make_shared<DoubleBuffer<RenderTransferData>>(1);
		doubleBufferMap.write(DataTransferTypes::TEST, tmpTransform);

		pauseSimulationState = false;
	}

	DataTransferManager(const DataTransferManager&) = delete;
	DataTransferManager& operator=(const DataTransferManager&) = delete;

public:
	static DataTransferManager& getInstance();
	std::shared_ptr<DoubleBuffer<RenderTransferData>> getDoubleBuffer(DataTransferTypes key);
	float& getSimulationFrameRate();
	bool& pauseSimulation();
};


