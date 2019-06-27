
#include "sceneTest.hpp"

void loadSceneTrash(const AppEnv& env) {
    GLS::Scene& scene(*env.scene);

    std::shared_ptr<GLS::Node> triangleNode = std::make_shared<GLS::Node>();
    triangleNode->setName("triangles");
    {
        std::shared_ptr<GLS::InstancedMesh> trianglesMesh = std::make_shared<GLS::InstancedMesh>();
        trianglesMesh->verticesRef().push_back(GLS::Vertex(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec2(0, 0)));
        trianglesMesh->verticesRef().push_back(GLS::Vertex(glm::vec3(1, 0, 0), glm::vec3(0, 0, -1), glm::vec2(1, 0)));
        trianglesMesh->verticesRef().push_back(GLS::Vertex(glm::vec3(0.5, 1, 0), glm::vec3(0, 0, -1), glm::vec2(0.5, 1)));
        trianglesMesh->indicesRef().push_back(0);
        trianglesMesh->indicesRef().push_back(1);
        trianglesMesh->indicesRef().push_back(2);
        std::shared_ptr<GLS::Material> triangleMat = std::make_shared<GLS::Material>();
        triangleMat->diffuse = glm::vec3(1.0, 0, 0);
        trianglesMesh->setMaterial(triangleMat);
        trianglesMesh->calculNormals();
        trianglesMesh->setInstancesCount(1000);
        
        for (size_t i = 0; i < trianglesMesh->instancesCount(); i++) {
            float x = ((i % 100) / 10) * 0.2f;
            float y = ((i % 100) % 10) * 0.2f;
            float z = ((float)i / 100) * 0.5f - 5.0f;
            GLS::Transform t;
            t.setPosition(glm::vec3(x, y, z));
            trianglesMesh->setInstanceTransformAt(i, t);
        }
        trianglesMesh->generateBuffers();
        triangleNode->addRenderable(trianglesMesh);
        triangleNode->transform().moveBy(0, 0, -10);
    }
    scene.rootNode()->addChildNode(triangleNode);

    std::shared_ptr<GLS::Node> planeNode = std::make_shared<GLS::Node>();
    planeNode->transform().rotateEulerAnglesBy(-M_PI / 2, 0, 0);
    planeNode->addRenderable(GLS::Mesh::plane(1.0, 1.0));
    scene.rootNode()->addChildNode(planeNode);
    planeNode->setName("plane");

    // std::shared_ptr<GLS::Node> planesNode = std::make_shared<GLS::Node>();
    // auto planesMesh = std::make_shared<GLS::InstancedMesh>(*GLS::Mesh::plane(2.0, 0.3), 50);
    // planesMesh->generateBuffers();
    // planesNode->addRenderable(planesMesh);
    // planeNode->addChildNode(planesNode);
    // planesNode->setName("planes");

    std::shared_ptr<GLS::Node> sphereNode = std::make_shared<GLS::Node>();
    {
        std::shared_ptr<GLS::Mesh> sphereMesh = GLS::Mesh::sphere(1.0);
        auto sphereMat = std::make_shared<GLS::Material>();
        sphereMat->diffuse = glm::vec3(0.5, 0.1, 0.2);
        sphereMesh->setMaterial(sphereMat);
        sphereMesh->generateBuffers();
        sphereNode->addRenderable(sphereMesh);
    }
    planeNode->addChildNode(sphereNode);
    sphereNode->setName("sphere");
    sphereNode->transform().setPosition(glm::vec3(2, 1, 0));

    try {
        std::shared_ptr<GLS::Node> nsNode = std::make_shared<GLS::Node>();
        std::string nsPath = "../models/nanosuit/nanosuit.obj";
        if (env.getArgument("-model") != nullptr)
            nsPath = *env.getArgument("-model");
        nsNode->loadFromFile(nsPath);
        scene.rootNode()->addChildNode(nsNode);
    } catch (std::exception& e) {
        std::cout << "can't load object with exception: " << e.what() << std::endl;
    }

    try {
        std::shared_ptr<GLS::Node> ftNode = std::make_shared<GLS::Node>();
        ftNode->loadFromFile("../models/42.obj");
        scene.rootNode()->addChildNode(ftNode);
        ftNode->setName("ft");
    } catch (std::exception& e) {
        std::cout << "can't create 42 node with exception: " << e.what() << std::endl;
    }

    std::shared_ptr<GLS::Node> spotlightNode = std::make_shared<GLS::Node>();
    std::shared_ptr<GLS::Light> spotlight = std::make_shared<GLS::Light>();
    spotlight->type = GLS::light_spot;
    spotlightNode->transform().setPosition(glm::vec3(0, 4, 0));
    spotlightNode->transform().setEulerAngles(0.5, -0.4, 0);
    spotlight->angle = 1.5;
    spotlightNode->setLight(spotlight);
    scene.rootNode()->addChildNode(spotlightNode);
    spotlightNode->setName("spotlight");

    std::shared_ptr<GLS::Node> pointLightNode = std::make_shared<GLS::Node>();
    std::shared_ptr<GLS::Light> pointlight = std::make_shared<GLS::Light>();
    pointlight->type = (GLS::light_point);
    pointLightNode->transform().setPosition(glm::vec3(0, 15, -7));
    pointLightNode->setLight(pointlight);
    scene.rootNode()->addChildNode(pointLightNode);
    pointLightNode->setName("pointight");

    std::shared_ptr<GLS::Node> cubeNode = std::make_shared<GLS::Node>();
    std::shared_ptr<GLS::Mesh> cubeMesh = GLS::Mesh::cube(1.5, 1.5, 1.5);
    try {
        auto cubeMat = std::make_shared<GLS::Material>();
        cubeMesh->setOutline(0.1, glm::vec3(1));
        cubeMesh->setMaterial(cubeMat);
        std::shared_ptr<GLS::Texture> diffuse(new GLS::Texture("../textures/pavement/albedo.png"));
        cubeMat->texture_diffuse = diffuse;
        std::shared_ptr<GLS::Texture> normal(new GLS::Texture("../textures/pavement/normal.png"));
        cubeMat->texture_normal = normal;
        std::shared_ptr<GLS::Texture> occlusion(new GLS::Texture("../textures/pavement/occlusion.png"));
        cubeMat->texture_occlusion = occlusion;
        std::shared_ptr<GLS::Texture> roughness(new GLS::Texture("../textures/pavement/roughness.png"));
        cubeMat->texture_roughness = roughness;
    } catch (std::exception& e) {
        std::cerr << "error with cube: " << e.what() << std::endl;
    }
    cubeNode->setName("cube");
    cubeNode->addRenderable(cubeMesh);
    cubeNode->transform().setPosition(glm::vec3(4, 1, 0));
    scene.rootNode()->addChildNode(cubeNode);
    cubeNode->setName("cube");

    std::shared_ptr<GLS::Node> cameraNode = std::make_shared<GLS::Node>();
    {
        std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
        camera->aspect = (scene.getAspect());
        camera->farZ = (300.0);
        cameraNode->setCamera(camera);
    }
    cameraNode->transform().moveBy(0, 3, 6);
    scene.setCameraNode(cameraNode);
    scene.rootNode()->addChildNode(cameraNode);

    std::vector<std::string> skyboxFaces;
    // skyboxFaces.push_back("../textures/lac_skybox/right.jpg");
    // skyboxFaces.push_back("../textures/lac_skybox/left.jpg");
    // skyboxFaces.push_back("../textures/lac_skybox/top.jpg");
    // skyboxFaces.push_back("../textures/lac_skybox/bottom.jpg");
    // skyboxFaces.push_back("../textures/lac_skybox/front.jpg");
    // skyboxFaces.push_back("../textures/lac_skybox/back.jpg");
    skyboxFaces.push_back("../textures/elbrus/elbrus_ft.jpg");
    skyboxFaces.push_back("../textures/elbrus/elbrus_bk.jpg");
    skyboxFaces.push_back("../textures/elbrus/elbrus_up.jpg");
    skyboxFaces.push_back("../textures/elbrus/elbrus_dn.jpg");
    skyboxFaces.push_back("../textures/elbrus/elbrus_rt.jpg");
    skyboxFaces.push_back("../textures/elbrus/elbrus_lf.jpg");
    try {
        std::shared_ptr<GLS::Skybox> skybox = std::make_shared<GLS::Skybox>(skyboxFaces);
        scene.setSkybox(skybox);
    } catch (std::exception& e) {
        std::cout << "can't load skybox textures with exception: " << e.what() << std::endl;
    }

}
