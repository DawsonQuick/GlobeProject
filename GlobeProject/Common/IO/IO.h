#pragma once
#pragma optimize("", off)
#include "./Input/Input.h"

//Outerfacing class so there only needs to be a single include for all the I/O functionality

namespace IO {

	inline void setActiveCamera(Camera* activeCamera) {
		IO::activeCamera = activeCamera;
		IO::isActiveCameraSet = true;
	}

}
#pragma optimize("", on)