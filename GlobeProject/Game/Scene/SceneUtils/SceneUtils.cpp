#include "SceneUtils.h"

void SceneUtils::exportScene(const char* path) {
	Writer sceneWriter(path);

	sceneWriter.write(Scene::getInstance().getActiveCamera()->getCameraInfo(), Scene::getInstance().getActiveCamera()->getCameraInfo().typeId());


}

void SceneUtils::importScene(const char* path){
	Reader sceneReader(path);

	while (!sceneReader.reachedEndOfFile()) {
		CodecPacket inPacket = sceneReader.getNextPacket();

		std::shared_ptr<void> obj = Registry::getInstance().deserialize(inPacket.header.type, inPacket.body.data);

		if (inPacket.header.type == CameraInfo::classId()) {
			auto newCameraInfo = std::static_pointer_cast<CameraInfo>(obj);
			std::unique_ptr<Camera> newCamera = std::make_unique<Camera>(Scene::getInstance().getCurrentSceneWindow(), newCameraInfo->type);
			newCamera->getCameraInfo() = *newCameraInfo;

			Scene::getInstance().addCamera("New camera", std::move(newCamera));
			break;
		}


	}
}