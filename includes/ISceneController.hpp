
#pragma once

#include "GLSWindow.hpp"

class ISceneController {

    protected:
    std::weak_ptr<GLSWindow> _window;
    std::shared_ptr<GLS::Scene> _scene;
	bool _cameraMouseControlEnabled;
	glm::vec2 _lastMousePosition;
	glm::vec2 _cameraEulerAngles;

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
    virtual void scrollCallBack(double x, double y);
    virtual void mouseButtonCallBack(int button, int action, int modifiers);
	virtual void closeCallback();

	void setCameraMouseControl(bool enabled);

};

# include "CustomSceneControllers/Human.hpp"
# include "CustomSceneControllers/ParticleSystem.hpp"
# include "CustomSceneControllers/Shadow.hpp"
# include "CustomSceneControllers/Trash.hpp"
# include "CustomSceneControllers/VoxelProcedural/VoxelProceduralSceneController.hpp"
# include "CustomSceneControllers/Gros.hpp"
# include "CustomSceneControllers/CaveRunning/CaveRunningSceneController.hpp"
