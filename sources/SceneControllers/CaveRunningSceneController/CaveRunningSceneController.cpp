
#include "AppEnv.hpp"

CaveRunningSceneController::CaveRunningSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window) {

}

CaveRunningSceneController::~CaveRunningSceneController() {

}

void CaveRunningSceneController::makeScene() {
    if (_window.expired())
        return;
    AppEnv *env = _window.lock()->getAppEnvPtr();
    (void)env;
    GLS::Scene& scene(*_scene);

    std::shared_ptr<GLS::Node> cameraNode = std::make_shared<GLS::Node>();
    std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
    camera->aspect = scene.getAspect();
    camera->farZ = 25.0;
    cameraNode->setCamera(camera);
    cameraNode->transform().moveBy(0, 0, 7);
    scene.setCameraNode(cameraNode);
    scene.rootNode()->addChildNode(cameraNode);

    std::shared_ptr<GLS::Light> light = std::make_shared<GLS::Light>();
    light->type = GLS::light_point;
    cameraNode->setLight(light);

    _game = std::make_shared<CaveRunningGame>(scene);
}

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

static time_t getLastmodifiedTimeOfFile(char const *filename) {
    struct stat result;
    if (stat(filename, &result) == 0) {
        auto mod_time = result.st_mtime;
        return mod_time;
    } else
        throw std::runtime_error("File does not exist");
}

void CaveRunningSceneController::update() {
    ISceneController::update();

    const char *kernelName = "assets/caveRunningGeneratorSources/default.cl";
    auto modifiedTime = getLastmodifiedTimeOfFile(kernelName);
    static time_t lastModifiedTime = modifiedTime;

    if (modifiedTime != lastModifiedTime) {
        lastModifiedTime = modifiedTime;
        _game->createRoomNodes(glm::ivec2(0, 0));
    }
}

void CaveRunningSceneController::keyCallBack(int key, int scancode, int action, int mods) {
    std::cout << "key-" << key << " scancode-" << scancode << " action-" << action << " mods-" << mods << std::endl;
    switch (key) {
    case '1':
        _game->createNewGame();
        break;
    case '2':
        _game->displayCaveScheme();
        break;
    case '3':
        _game->createRoomNodes(glm::ivec2(0, 0));
        break;
    }
    
}

void CaveRunningSceneController::scrollCallBack(double x, double y) {
    (void)x;
    (void)y;
}

void CaveRunningSceneController::resizeWindowCallBack(glm::vec2 newSize) {
    (void)newSize;
}

void CaveRunningSceneController::mouseButtonCallBack(int button, int action, int modifiers) {
    (void)button;
    (void)action;
    (void)modifiers;
}

void CaveRunningSceneController::closeCallback() {
    
}