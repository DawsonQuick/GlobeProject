#pragma once
#include "./../../../Common/Vendor/glm/glm.hpp"
#include "./../../WorldInteraction/WorldInteraction.h"
#include "./../../../Common/IO/IO.h"
#include "./../../../Common/Serialization/SerializationUtils.h"



enum CameraType {
	ORBITAL,
	FREECAM,
	MAXNUMBER //Have the last entry in the enum be a identifier for the number of enum types
};


#pragma optimize("", off)
struct CameraInfo {

	TYPE_NAME(CameraInfo);

	glm::vec3 m_CameraPos = glm::vec3(100, 0, 100);
	glm::vec3 m_CameraFront = glm::vec3(0, 0, -1);
	glm::vec3 m_CameraUp = glm::vec3(0, 1, 0);
	glm::vec3 m_CameraRight;
	glm::mat4 view;
	glm::mat4 projection;
	CameraType type;
	bool isRaySet;
	Ray ray;

	float m_Yaw;
	float m_Pitch;

	CameraInfo() {}


	void serialize(std::vector<std::byte>& buffer) {

		serializePrimitive(buffer, m_CameraPos.x);
		serializePrimitive(buffer, m_CameraPos.y);
		serializePrimitive(buffer, m_CameraPos.z);

		serializePrimitive(buffer, m_CameraFront.x);
		serializePrimitive(buffer, m_CameraFront.y);
		serializePrimitive(buffer, m_CameraFront.z);

		serializePrimitive(buffer, m_CameraUp.x);
		serializePrimitive(buffer, m_CameraUp.y);
		serializePrimitive(buffer, m_CameraUp.z);


		serializePrimitive(buffer, m_CameraRight.x);
		serializePrimitive(buffer, m_CameraRight.y);
		serializePrimitive(buffer, m_CameraRight.z);

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				serializePrimitive(buffer, view[i][j]);
			}
		}

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				serializePrimitive(buffer, projection[i][j]);
			}
		}

		serializePrimitive(buffer, type);
		serializePrimitive(buffer, m_Yaw);
		serializePrimitive(buffer, m_Pitch);

	}

	static CameraInfo deserialize(const std::vector<std::byte>& buffer) {

		CameraInfo info;
		size_t offset = 0;
		deserializePrimitive(buffer, offset, info.m_CameraPos.x);
		deserializePrimitive(buffer, offset, info.m_CameraPos.y);
		deserializePrimitive(buffer, offset, info.m_CameraPos.z);

		deserializePrimitive(buffer, offset, info.m_CameraFront.x);
		deserializePrimitive(buffer, offset, info.m_CameraFront.y);
		deserializePrimitive(buffer, offset, info.m_CameraFront.z);

		deserializePrimitive(buffer, offset, info.m_CameraUp.x);
		deserializePrimitive(buffer, offset, info.m_CameraUp.y);
		deserializePrimitive(buffer, offset, info.m_CameraUp.z);


		deserializePrimitive(buffer, offset, info.m_CameraRight.x);
		deserializePrimitive(buffer, offset, info.m_CameraRight.y);
		deserializePrimitive(buffer, offset, info.m_CameraRight.z);

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				deserializePrimitive(buffer, offset, info.view[i][j]);
			}
		}

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				deserializePrimitive(buffer, offset, info.projection[i][j]);
			}
		}

		deserializePrimitive(buffer, offset, info.type);
		deserializePrimitive(buffer, offset, info.m_Yaw);
		deserializePrimitive(buffer, offset, info.m_Pitch);

		return info;

	}

	// Automatic registration
	static AutoRegister<CameraInfo> _autoRegister;
};

inline AutoRegister<CameraInfo> CameraInfo::_autoRegister;

struct ICameraControls {
	virtual ~ICameraControls() = default;

	
	//Assign KeyBinds
	virtual void bindKeys() = 0;
	//Remove KeyBinds
	virtual void unbindKeys() = 0;

	//Render ImGui controls for variable controls
	virtual void renderGuiControls() = 0;
};

#pragma optimize("", on)