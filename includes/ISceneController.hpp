
#pragma once

#include "GLSWindow.hpp"

class ISceneController {

    protected:
    std::weak_ptr<GLSWindow> _window;
    std::shared_ptr<GLS::Scene> _scene;

    public:

    bool mustUpdate;
	float cameraMoveSpeed;

    ISceneController(std::shared_ptr<GLSWindow> window);
    virtual ~ISceneController();

    std::shared_ptr<GLS::Scene> scene();

    virtual void makeScene() = 0;
    virtual void update();
    virtual void resizeWindowCallBack(glm::vec2 newSize);
    virtual void keyCallBack(int key, int scancode, int action, int mods);

	void setCameraMouseControl(bool enabled);

	private:
		glm::vec2 _lastMousePosition;
		bool _cameraMouseControlEnabled;

};

# include "CustomSceneControllers/Human.hpp"
# include "CustomSceneControllers/ParticleSystem.hpp"
# include "CustomSceneControllers/Shadow.hpp"
# include "CustomSceneControllers/Trash.hpp"
# include "CustomSceneControllers/Voxel.hpp"
# include "CustomSceneControllers/VoxelProcedural/VoxelProcedural.hpp"
# include "CustomSceneControllers/Gros.hpp"
