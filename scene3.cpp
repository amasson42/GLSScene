
#include "GLScene.hpp"

float degreeToRadians(float deg) {
    return deg * M_PI / 180.0;
}

std::shared_ptr<GLS::Node> rotateNode = nullptr;

void updateScene3(double et, double dt) {
    (void)dt;
    if (rotateNode != nullptr) {
        rotateNode->transform().setEulerAngles(glm::vec3(0, et * 0.2, 0));
    }
}

float randFloat() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

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
    rotateNode = cubeNode1;

    auto cubesNode = std::make_shared<GLS::Node>();
    auto smallCubeMesh = GLS::Mesh::cube(0.05, 0.05, 0.05);
    auto cubesMesh = std::make_shared<GLS::InstancedMesh>(*smallCubeMesh, 1000);
    for (size_t i = 0; i < cubesMesh->instancesCount(); i++) {
        float angle = randFloat() * M_PI * 2;
        float distance = randFloat() * 0.4 + 1.6;

        glm::vec3 pos;
        pos.x = cos(angle) * distance;
        pos.y = randFloat() * 0.3 - 0.8;
        pos.z = sin(angle) * distance;
        GLS::Transform t;
        t.setPosition(pos);
        t.setRotation(glm::quat(randFloat(), randFloat(), randFloat(), randFloat()));
        t.setScale(glm::vec3(randFloat() * 0.2 + 0.7));
        cubesMesh->setInstanceTransformAt(i, t);
    }
    cubesMesh->setMaterial(std::make_shared<GLS::Material>());
    cubesNode->addRenderable(cubesMesh);
    cubeNode1->addChildNode(cubesNode);

    auto sphereNode = std::make_shared<GLS::Node>();
    auto sphereMesh = GLS::Mesh::sphere(0.5);
    auto sphereMaterial = std::make_shared<GLS::Material>();
    sphereMaterial->shininess = 64;
    sphereMesh->setMaterial(sphereMaterial);
    sphereNode->addRenderable(sphereMesh);
    sphereNode->setName("sphere");
    sphereNode->transform().setPosition(glm::vec3(1, 0.75, 1));
    scene.rootNode().addChildNode(sphereNode);

    auto spotlightNode = std::make_shared<GLS::Node>();
    auto spotlight = std::make_shared<GLS::Light>();
    spotlight->type = (GLS::light_point);
    spotlight->attenuation = glm::vec3(1, 0.05, 0.01);
    spotlightNode->setLight(spotlight);
    spotlightNode->transform().setPosition(glm::vec3(3, 4, 3));
    spotlightNode->transform().setEulerAngles(glm::vec3(degreeToRadians(-50), degreeToRadians(50), 0));
    spotlightNode->setName("spotlight");
    scene.rootNode().addChildNode(spotlightNode);
    spotlightNode->addRenderable(GLS::Mesh::cube(0.2, 0.2, 0.6));

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
