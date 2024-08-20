#pragma once
#include <thread>
#include <chrono>
#include <random>
#include "./../../../Common/Vendor/imgui/imgui.h"
#include "./../DataTransferManager/DataTransferManager.h"
#include "./../../../Common/Utilities/OrbitalCalculation/SphericalOrbit/SphericalOrbitCalculator.h"
#include "./../../../Common/ECS/Entity.h"
#include "./../../../Common/Stopwatch/Stopwatch.h"
class SimulationManager
{
public:
	SimulationManager();
	~SimulationManager();

	void startSimulationThread();
private:
	std::chrono::high_resolution_clock::time_point prev_time;
	DataTransferManager& dtMrg;
	Stopwatch<std::chrono::milliseconds> stopWatch;

};

