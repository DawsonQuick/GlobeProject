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
	LockFreeMap<DataTransferTypes,std::shared_ptr<DoubleBuffer<RenderTransferDataTemp>>> doubleBufferMap;
	float simulationFrameRate;
	bool pauseSimulationState;
	bool terminateSignal;

	DataTransferManager() {
		//std::shared_ptr <DoubleBuffer<float>> tmp = std::make_shared<DoubleBuffer<float>>(1);
		//doubleBufferMap.write(DataTransferTypes::TEST, tmp);

		std::shared_ptr <DoubleBuffer<RenderTransferDataTemp>> tmpTransform = std::make_shared<DoubleBuffer<RenderTransferDataTemp>>(1);
		doubleBufferMap.write(DataTransferTypes::TEST, tmpTransform);

		pauseSimulationState = false;
		terminateSignal = false;
	}

	DataTransferManager(const DataTransferManager&) = delete;
	DataTransferManager& operator=(const DataTransferManager&) = delete;

public:
	static DataTransferManager& getInstance();
	std::shared_ptr<DoubleBuffer<RenderTransferDataTemp>> getDoubleBuffer(DataTransferTypes key);
	float& getSimulationFrameRate();
	bool& pauseSimulation();
	bool& terminationSignal();
};


