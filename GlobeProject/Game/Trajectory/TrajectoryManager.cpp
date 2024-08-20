#include "TrajectoryManager.h"



TrajectoryManager::TrajectoryManager() : renderer(false) {

}

TrajectoryManager::~TrajectoryManager() {

}
void TrajectoryManager::generate() {
    std::vector<float> trajectories;
    std::vector<float> tmp = generateSphericalOrbitTrajectory(7000.0, (3.1415/4), (3.1415 / 4));
    trajectories.insert(trajectories.end(), tmp.begin(), tmp.end());

    tmp = generateSphericalOrbitTrajectory(9000.0, (3.1415) , ((3.1415 * 2) / 4));
    trajectories.insert(trajectories.end(), tmp.begin(), tmp.end());

    tmp = generateSphericalOrbitTrajectory(9000.0, ((3.1415 * 2)/4), (3.1415));
    trajectories.insert(trajectories.end(), tmp.begin(), tmp.end());

    tmp = generateSphericalOrbitTrajectory(8000.0, ((3.1415 * 5) / 6) , ((3.1415 * 3) / 2));
    trajectories.insert(trajectories.end(), tmp.begin(), tmp.end());

    tmp = generateSphericalOrbitTrajectory(7500.0, ((3.1415 * 3) / 2) , ((3.1415 * 5) / 6));
    trajectories.insert(trajectories.end(), tmp.begin(), tmp.end());

    renderer.appendLines(trajectories);

}

void TrajectoryManager::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
    renderer.render(model, view, projection);
}