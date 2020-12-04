
#include "AppEnv.hpp"

GrosSceneController::GrosSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window) {
}

GrosSceneController::~GrosSceneController() {
    
}

void GrosSceneController::update() {
    ISceneController::update();
	// float deltaTime = _window.lock()->deltaTime();

}

void GrosSceneController::makeScene() {
    using namespace GLS;

    // camera
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    camera->aspect = scene()->getAspect();

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    cameraNode->setName("camera");
    cameraNode->setCamera(camera);
    cameraNode->transform().moveBy(0, 2, 5);
    scene()->rootNode()->addChildNode(cameraNode);
    scene()->setCameraNode(cameraNode);

    std::shared_ptr<Light> pointLight = std::make_shared<Light>();
    pointLight->type = light_point;
    cameraNode->setLight(pointLight);


    // mes nodes
    std::shared_ptr<Node> myNode = std::make_shared<Node>();
    myNode->setName("myNode");
    scene()->rootNode()->addChildNode(myNode);

    std::shared_ptr<Mesh> myMesh = Mesh::sphere(1.2);
    myNode->addRenderable(myMesh);

}
