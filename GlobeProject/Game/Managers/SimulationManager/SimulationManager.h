#pragma once
#include <thread>
#include <chrono>
#include <random>
#include "./../../../Common/Vendor/imgui/imgui.h"
#include "./../DataTransferManager/DataTransferManager.h"
#include "./../../../Common/Utilities/OrbitalCalculation/SphericalOrbit/SphericalOrbitCalculator.h"
#include "./../../Scene/Scene.h"
#include "./../../../Common/Utilities/Stopwatch/Stopwatch.h"
#include "./../../../Common/ModelManager/ModelManager.h"
#include "./../../../Common/SphereGeneration/SphereGeneration.h"
#include "./../../../Common/Logger/Logger.h"
#include "./../../../Common/Vendor/stb_image/stb_image_write.h"
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

