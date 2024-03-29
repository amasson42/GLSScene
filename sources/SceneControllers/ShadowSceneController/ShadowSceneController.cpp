
#include "AppEnv.hpp"

float degreeToRadians(float deg) {
    return deg * M_PI / 180.0;
}

ShadowSceneController::ShadowSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window) {

}

ShadowSceneController::~ShadowSceneController() {
    
}

void ShadowSceneController::update() {
    ISceneController::update();
    if (!mustUpdate)
        return;
    if (_window.expired())
        return;
    float et = _window.lock()->elapsedTime();
    float dt = _window.lock()->deltaTime();

    if (bigCubeNode != nullptr) {
        bigCubeNode->transform().setEulerAngles(glm::vec3(sin(et * 0.3) * 0.5, et * 0.2, 0));
    }
    if (lightPivotNode != nullptr) {
        float angle = ((sin(et) + 1.0) / 2.0) * degreeToRadians(-50);
        lightPivotNode->transform().setEulerAngles(glm::vec3(angle, degreeToRadians(50), 0));
    }
    if (cubesPivotNode != nullptr) {
        float rotateSpeed = 0.2;
        cubesPivotNode->transform().setEulerAngles(glm::vec3(sin(et * 0.02) * 0.3, et * rotateSpeed, cos(et * 0.07) * 0.3));
        cubesPivotNode->transform().moveBy(glm::vec3(0, 0.2 * dt * sin(M_PI + et * 0.3), 0));
    }
    if (instancedMeshFloater != nullptr) {
        for (size_t i = 0; i < instancedMeshFloater->instancesCount(); i++) {
            float length = (float)i * 0.2 + et * 0.5;
            GLS::Transform t = instancedMeshFloater->instanceTransformAt(i);
            t.position().y = cos(length) * 0.2;
            instancedMeshFloater->setInstanceTransformAt(i, t);
        }
        instancedMeshFloater->updateTransformsBufferValues();
    }
}

float randFloat() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void ShadowSceneController::keyCallBack(int key, int scancode, int action, int mods) {
    (void)scancode;(void)mods;
    if (key == 84) { // keycode = T
        if (action) {
            cubesPivotNode->removeFromParent();
        } else {
            _scene->rootNode()->addChildNode(cubesPivotNode);
        }
    }
}

void ShadowSceneController::makeScene() {
    if (_window.expired())
        return;
    GLS::Scene& scene(*_scene);
    AppEnv *env = _window.lock()->getAppEnvPtr();

    auto texturedMaterial = std::make_shared<GLS::Material>();
    texturedMaterial->specular = glm::vec3(0.1);
    texturedMaterial->shininess = 64;
    try {
        std::shared_ptr<std::string> diffuseName = env->getArgument("-diffuse");
        if (diffuseName != nullptr)
            texturedMaterial->texture_diffuse = std::make_shared<GLS::Texture>(*diffuseName);
        std::shared_ptr<std::string> normalName = env->getArgument("-normal");
        if (normalName)
            texturedMaterial->texture_normal = std::make_shared<GLS::Texture>(*normalName);
    } catch (std::exception& e) {
        std::cout << "error " << e.what() << std::endl;
    }

    auto planeNode = std::make_shared<GLS::Node>();
    planeNode->transform().setEulerAngles(glm::vec3(-M_PI_2, 0, 0));
    auto planeMesh = GLS::Mesh::plane(10, 10);
    planeNode->addRenderable(planeMesh);
    planeNode->setName("plane");
    planeNode->transform().moveBy(0, -0.01, 0);
    scene.rootNode()->addChildNode(planeNode);
    
    auto cubeNode1 = std::make_shared<GLS::Node>();
    auto cubeMesh1 = GLS::Mesh::cube(1, 1, 1);
    cubeMesh1->setMaterial(texturedMaterial);
    cubeNode1->addRenderable(cubeMesh1);
    cubeNode1->setName("cube1");
    cubeNode1->transform().setPosition(glm::vec3(-1, 1.75, 1));
    scene.rootNode()->addChildNode(cubeNode1);
    bigCubeNode = cubeNode1;

    cubesPivotNode = std::make_shared<GLS::Node>();
    cubesPivotNode->transform().setPosition(bigCubeNode->transform().position());
    scene.rootNode()->addChildNode(cubesPivotNode);
    auto cubesNode = std::make_shared<GLS::Node>();
    auto smallCubeMesh = GLS::Mesh::cube(0.05, 0.05, 0.05);
    auto smallCubeMaterial = std::make_shared<GLS::Material>();
    smallCubeMaterial->diffuse = glm::vec3(0.8, 0.2, 0.3);
    smallCubeMesh->setMaterial(smallCubeMaterial);
    auto cubesMesh = std::make_shared<GLS::InstancedMesh>(*smallCubeMesh, 10000);
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
    instancedMeshFloater = cubesMesh;
    cubesNode->addRenderable(cubesMesh);
    cubesPivotNode->addChildNode(cubesNode);
    cubesMesh->generateBuffers();

    auto sphereNode = std::make_shared<GLS::Node>();
    auto sphereMesh = GLS::Mesh::sphere(0.5);
    sphereMesh->setMaterial(texturedMaterial);
    sphereNode->addRenderable(sphereMesh);
    sphereNode->setName("sphere");
    sphereNode->transform().setPosition(glm::vec3(1, 0.75, 1));
    scene.rootNode()->addChildNode(sphereNode);

    lightPivotNode = std::make_shared<GLS::Node>();
    lightPivotNode->transform().setEulerAngles(glm::vec3(degreeToRadians(-50), degreeToRadians(50), 0));
    lightPivotNode->setName("light_pivot");
    scene.rootNode()->addChildNode(lightPivotNode);

    auto spotlightNode = std::make_shared<GLS::Node>();
    auto spotlight = std::make_shared<GLS::Light>();
    spotlight->cast_shadow = true;
    spotlight->type = (GLS::light_spot);
    spotlightNode->setLight(spotlight);
    spotlightNode->transform().setPosition(glm::vec3(0, 0, 4));
    spotlightNode->setName("spotlight");
    lightPivotNode->addChildNode(spotlightNode);
    auto spotlightMesh = GLS::Mesh::cube(0.1, 0.1, 0.15);
    spotlightMesh->setOutline(0.1, spotlight->color);
    spotlightNode->addRenderable(spotlightMesh);

    auto pointlightNode = std::make_shared<GLS::Node>();
    auto pointlight = std::make_shared<GLS::Light>();
    pointlight->type = GLS::light_spot;
    pointlight->color = glm::vec3(1);
    pointlight->cast_shadow = false;
    pointlightNode->setLight(pointlight);

    std::shared_ptr<GLS::Node> cameraNode = std::make_shared<GLS::Node>();
    {
        std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
        camera->aspect = (scene.getAspect());
        cameraNode->setCamera(camera);
    }
    cameraNode->transform().moveBy(0, 2, 5);
    scene.setCameraNode(cameraNode);
    scene.rootNode()->addChildNode(cameraNode);
    cameraNode->addChildNode(pointlightNode);
    pointlightNode->transform().moveBy(1, 0, 0);
    pointlightNode->transform().rotateEulerAnglesBy(0, 0.2, 0);

    std::shared_ptr<GLS::Node> particleNode = std::make_shared<GLS::Node>();

    GLS::ParticleSystemProperties psProp;
    psProp.count = 1000000;
    psProp.kernelSource = file_to_string("assets/particleSystemSources/gravityCenterPoint.cl");
    
    try {
        std::shared_ptr<GLS::ParticleSystem> ps = std::make_shared<GLS::ParticleSystem>(psProp);

        cl_float3 clf3;
        clf3.s[0] = 0.5;
        clf3.s[1] = 0.5;
        clf3.s[2] = 0.5;
        ps->getAnimationKernel()->setArgument(3, clf3);
        ps->generateBuffers();
        ps->initAnimation();
        particleNode->addRenderable(ps);
        particleNode->addAnimatable(ps);
        scene.rootNode()->addChildNode(particleNode);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    // particleSystem = ps;
}
