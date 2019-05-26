
#include "GLScene.hpp"

float degreeToRadians(float deg) {
    return deg * M_PI / 180.0;
}

std::shared_ptr<GLS::Node> rotateNode = nullptr;
std::shared_ptr<GLS::Node> pivotNode = nullptr;

void updateScene3(double et, double dt) {
    (void)dt;
    if (rotateNode != nullptr) {
        rotateNode->transform().setEulerAngles(glm::vec3(sin(et * 0.3) * 0.5, et * 0.2, 0));
    }
    if (pivotNode != nullptr) {
        float angle = ((sin(et) + 1.0) / 2.0) * degreeToRadians(-50);
        pivotNode->transform().setEulerAngles(glm::vec3(angle, degreeToRadians(50), 0));
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
    auto cubeMaterial1 = std::make_shared<GLS::Material>();
    cubeMaterial1->texture_diffuse = std::make_shared<GLS::Texture>("../textures/brickwall.jpg");
    cubeMaterial1->texture_normal = std::make_shared<GLS::Texture>("../textures/brickwall_normal.jpg");
    cubeMaterial1->specular = glm::vec3(0.1);
    cubeMaterial1->shininess = 64;
    cubeMesh1->setMaterial(cubeMaterial1);
    cubeNode1->addRenderable(cubeMesh1);
    cubeNode1->setName("cube1");
    cubeNode1->transform().setPosition(glm::vec3(-1, 1.75, 1));
    scene.rootNode().addChildNode(cubeNode1);
    rotateNode = cubeNode1;

    auto cubesNode = std::make_shared<GLS::Node>();
    auto smallCubeMesh = GLS::Mesh::cube(0.05, 0.05, 0.05);
    auto smallCubeMaterial = std::make_shared<GLS::Material>();
    smallCubeMaterial->diffuse = glm::vec3(0.8, 0.2, 0.3);
    planeMesh->setMaterial(smallCubeMaterial);
    smallCubeMesh->setMaterial(smallCubeMaterial);
    auto cubesMesh = std::make_shared<GLS::InstancedMesh>(*smallCubeMesh, 1000);
    cubesMesh->setMaterial(smallCubeMaterial);
    for (size_t i = 0; i < cubesMesh->instancesCount(); i++) {
        float angle = randFloat() * M_PI * 2;
        float distance = randFloat() * 0.4 + 1.6;

        glm::vec3 pos;
        pos.x = cos(angle) * distance;
        pos.y = randFloat() * 0.3 + 0.4;
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
    sphereMaterial->texture_diffuse = std::make_shared<GLS::Texture>("../textures/brickwall.jpg");
    sphereMaterial->texture_normal = std::make_shared<GLS::Texture>("../textures/brickwall_normal.jpg");
    sphereMaterial->specular = glm::vec3(0.2);
    sphereMaterial->shininess = 64;
    sphereMesh->setMaterial(sphereMaterial);
    sphereNode->addRenderable(sphereMesh);
    sphereNode->setName("sphere");
    sphereNode->transform().setPosition(glm::vec3(1, 0.75, 1));
    scene.rootNode().addChildNode(sphereNode);

    auto lightPivotNode = std::make_shared<GLS::Node>();
    lightPivotNode->transform().setEulerAngles(glm::vec3(degreeToRadians(-50), degreeToRadians(50), 0));
    lightPivotNode->setName("light_pivot");
    scene.rootNode().addChildNode(lightPivotNode);
    pivotNode = lightPivotNode;

    auto spotlightNode = std::make_shared<GLS::Node>();
    auto spotlight = std::make_shared<GLS::Light>();
    spotlight->cast_shadow = true;
    spotlight->type = (GLS::light_spot);
    spotlightNode->setLight(spotlight);
    spotlightNode->transform().setPosition(glm::vec3(0, 0, 4));
    spotlightNode->setName("spotlight");
    lightPivotNode->addChildNode(spotlightNode);

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
