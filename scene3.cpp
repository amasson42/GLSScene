
#include "GLScene.hpp"

void loadScene3(GLS::Scene& scene) {

    auto planeNode = std::make_shared<GLS::Node>();
    planeNode->transform().setEulerAngles(glm::vec3(-M_PI_2, 0, 0));
    auto planeMesh = GLS::Mesh::plane(10, 10);
    planeNode->addRenderable(planeMesh);
    planeNode->setName("plane");
    scene.rootNode().addChildNode(planeNode);
    
    auto cubeNode1 = std::make_shared<GLS::Node>();
    auto cubeMesh1 = GLS::Mesh::cube(1, 1, 1);
    cubeNode1->addRenderable(cubeMesh1);
    cubeNode1->setName("cube1");
    cubeNode1->transform().setPosition(glm::vec3(-1, 0.75, 1));
    scene.rootNode().addChildNode(cubeNode1);

    auto sphereNode = std::make_shared<GLS::Node>();
    auto sphereMesh = GLS::Mesh::sphere(0.5);
    sphereNode->addRenderable(sphereMesh);
    sphereNode->setName("sphere");
    sphereNode->transform().setPosition(glm::vec3(1, 0.75, 1));
    scene.rootNode().addChildNode(sphereNode);

    auto pointLightNode = std::make_shared<GLS::Node>();
    auto pointLight = std::make_shared<GLS::Light>();
    pointLight->type = (GLS::light_point);
    pointLight->position = (glm::vec3(0, 5, 0));
    pointLightNode->setLight(pointLight);
    pointLightNode->setName("pointlight");
    scene.rootNode().addChildNode(pointLightNode);

    auto spotlightNode = std::make_shared<GLS::Node>();
    auto spotlight = std::make_shared<GLS::Light>();
    spotlight->type = (GLS::light_spot);
    spotlight->position = (glm::vec3(3, 4, 3));
    spotlight->direction = (glm::normalize(glm::vec3(-1, 0, 1)));
    spotlight->angle = (3);
    spotlightNode->setLight(spotlight);
    spotlightNode->setName("spotlight");
    // scene.rootNode().addChildNode(spotlightNode);

    auto ambiantLightNode = std::make_shared<GLS::Node>();
    auto ambiantlight = std::make_shared<GLS::Light>();
    ambiantlight->type = (GLS::light_ambiant);
    ambiantLightNode->setLight(ambiantlight);
    ambiantLightNode->setName("ambiant");
    // scene.rootNode().addChildNode(ambiantLightNode);

    std::shared_ptr<GLS::Node> cameraNode = std::make_shared<GLS::Node>();
    {
        std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
        camera->aspect = (scene.getAspect());
        camera->farZ = (25.0);
        cameraNode->setCamera(camera);
    }
    cameraNode->transform().moveBy(0, 2, 5);
    scene.setCameraNode(*cameraNode);
    scene.rootNode().addChildNode(cameraNode);

}
