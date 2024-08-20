#pragma once
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
#include <thread>
#include "./../../Terrain/Globe/Globe.h"
#include "./../../Trajectory/TrajectoryManager.h"
#include "./../../MainImGuiComponents/MainImGuiRender.h"
#include "./../../../Common/Vendor/imgui/imgui.h"
#include "./../../../Common/Vendor/imgui/imgui_impl_glfw_gl3.h"


#include "./../../../Common/Vendor/glm/glm.hpp"
#include "./../../../Common/Vendor/glm/gtc/matrix_transform.hpp"
#include "./../../../Common/Vendor/glm/gtc/type_ptr.hpp"


#include "./../../../Common/Camera/Camera.h"
#include "./../../../Common/OpenGLUtilities/LineRenderer/LineRenderer.h"
#include "./../DataTransferManager/DataTransferManager.h"
#include "./RenderUtils/RenderUtils.h"
class RenderManager
{

public:
	RenderManager();
	~RenderManager();

	int startRenderThread();
private:
	DataTransferManager& dtMrg;

};

