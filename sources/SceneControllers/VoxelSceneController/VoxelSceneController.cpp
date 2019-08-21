
#include "AppEnv.hpp"

namespace glm {
	bool operator<(const glm::ivec2& lhs, const glm::ivec2& rhs) {
		if (lhs.x < rhs.x)
			return true;
		else
			return lhs.y < rhs.y;
	}

	std::ostream& operator<<(std::ostream& out, const glm::ivec2& iv) {
		out << "[" << iv.x << "; " << iv.y << "]";
		return out;
	}
}


VoxelSceneController::VoxelSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window) {
    removeBlockCD = 1.0;
    i = 0;
}

VoxelSceneController::~VoxelSceneController() {

}

void VoxelSceneController::update() {
    ISceneController::update();
}

void VoxelSceneController::makeScene() {
}
