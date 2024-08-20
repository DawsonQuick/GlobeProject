#pragma once
#include <GL/glew.h>
#include <vector>
#include "./../../Common/Utilities/OrbitalCalculation/SphericalOrbit/SphericalOrbitCalculator.h"
#include "./../../Common/OpenGLUtilities/LineRenderer/LineRenderer.h"
#include "./../../Common/Vendor/glm/glm.hpp"
#include "./../../Common/Vendor/glm/gtc/matrix_transform.hpp"
#include "./../../Common/Vendor/glm/gtc/type_ptr.hpp"
class TrajectoryManager
{
public:
	TrajectoryManager();
	~TrajectoryManager();

	void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void generate();

private:
    LineRenderer renderer;
};

