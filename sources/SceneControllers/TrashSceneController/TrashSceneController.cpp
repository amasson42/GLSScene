
#include "AppEnv.hpp"

// class ExplodingMesh:: public GLS::IAnimatable {
// 	private:
// 	float _lifeTime;
// 	std::shared_ptr<GLS::Node> _node; // contains multiples children node containing meshes

// 	public:

static GLS::Vertex _midVertex(GLS::Vertex v1, GLS::Vertex v2) {
    return GLS::Vertex((v1.position + v2.position) / glm::vec3(2),
                        (v1.normal + v2.normal) / glm::vec3(2),
                        (v1.tangent + v2.tangent) / glm::vec3(2),
                        (v1.bitangent + v2.bitangent) / glm::vec3(2),
                        (v1.uv + v2.uv) / glm::vec2(2));
}

ExplodingMesh::ExplodingMesh(const std::shared_ptr<GLS::Mesh>& mesh) {
    _node = std::make_shared<GLS::Node>();

    std::shared_ptr<GLS::Material> mat = mesh->getMaterial();

    for (int i = 0; i <= static_cast<int>(mesh->indicesRef().size()) - 3; i += 3) {
        int id_0 = mesh->indicesRef()[i + 0];
        int id_1 = mesh->indicesRef()[i + 1];
        int id_2 = mesh->indicesRef()[i + 2];
        GLS::Vertex v1 = mesh->verticesRef()[id_0];
        v1.position -= glm::vec3(0.5);
        GLS::Vertex v2 = mesh->verticesRef()[id_1];
        v2.position -= glm::vec3(0.5);
        GLS::Vertex v3 = mesh->verticesRef()[id_2];
        v3.position -= glm::vec3(0.5);
        GLS::Vertex v1_2 = _midVertex(v1, v2);
        GLS::Vertex v2_3 = _midVertex(v2, v3);
        GLS::Vertex v3_1 = _midVertex(v3, v1);

        {
            std::shared_ptr<GLS::Node> fragNode = std::make_shared<GLS::Node>();
            std::shared_ptr<GLS::Mesh> fragMesh = std::make_shared<GLS::Mesh>();
            fragMesh->setMaterial(mat);

            fragMesh->verticesRef().push_back(v1);
            fragMesh->verticesRef().push_back(v1_2);
            fragMesh->verticesRef().push_back(v3_1);
            fragMesh->indicesRef().push_back(0);
            fragMesh->indicesRef().push_back(1);
            fragMesh->indicesRef().push_back(2);

            fragMesh->generateBuffers();
            fragNode->addRenderable(fragMesh);
            _node->addChildNode(fragNode);
        }
        {
            std::shared_ptr<GLS::Node> fragNode = std::make_shared<GLS::Node>();
            std::shared_ptr<GLS::Mesh> fragMesh = std::make_shared<GLS::Mesh>();
            fragMesh->setMaterial(mat);

            fragMesh->verticesRef().push_back(v1_2);
            fragMesh->verticesRef().push_back(v2);
            fragMesh->verticesRef().push_back(v2_3);
            fragMesh->indicesRef().push_back(0);
            fragMesh->indicesRef().push_back(1);
            fragMesh->indicesRef().push_back(2);

            fragMesh->generateBuffers();
            fragNode->addRenderable(fragMesh);
            _node->addChildNode(fragNode);
        }
        {
            std::shared_ptr<GLS::Node> fragNode = std::make_shared<GLS::Node>();
            std::shared_ptr<GLS::Mesh> fragMesh = std::make_shared<GLS::Mesh>();
            fragMesh->setMaterial(mat);

            fragMesh->verticesRef().push_back(v3_1);
            fragMesh->verticesRef().push_back(v2_3);
            fragMesh->verticesRef().push_back(v3);
            fragMesh->indicesRef().push_back(0);
            fragMesh->indicesRef().push_back(1);
            fragMesh->indicesRef().push_back(2);

            fragMesh->generateBuffers();
            fragNode->addRenderable(fragMesh);
            _node->addChildNode(fragNode);
        }
        {
            std::shared_ptr<GLS::Node> fragNode = std::make_shared<GLS::Node>();
            std::shared_ptr<GLS::Mesh> fragMesh = std::make_shared<GLS::Mesh>();
            fragMesh->setMaterial(mat);

            fragMesh->verticesRef().push_back(v1_2);
            fragMesh->verticesRef().push_back(v2_3);
            fragMesh->verticesRef().push_back(v3_1);
            fragMesh->indicesRef().push_back(0);
            fragMesh->indicesRef().push_back(1);
            fragMesh->indicesRef().push_back(2);

            fragMesh->generateBuffers();
            fragNode->addRenderable(fragMesh);
            _node->addChildNode(fragNode);
        }

    }
}

ExplodingMesh::~ExplodingMesh() {
    _node->removeFromParent();
}

std::shared_ptr<GLS::Node> ExplodingMesh::node() const {
	return _node;
}

void ExplodingMesh::initAnimation() {
    _lifeTime = 0;
}

#define EXPLODING_TIME 0.25
void ExplodingMesh::animate(GLS::timefloat deltaTime) {
    _lifeTime += deltaTime;
    glm::vec3 pos = glm::vec3(_node->getWorldTransformMatrix() * glm::vec4(0, 0, 0, 1));
    for (int i = 0; i < _node->childNodes().size(); i++) {
        GLS::Transform& transform(_node->childNodeAt(i)->transform());
        GLS::Mesh* triangleMesh = dynamic_cast<GLS::Mesh*>(_node->childNodeAt(i)->renderables()[0].get());
        glm::vec3 cen =
            (triangleMesh->verticesRef()[0].position
            + triangleMesh->verticesRef()[1].position
            + triangleMesh->verticesRef()[2].position) / 3.0f;
        transform.rotateEulerAnglesBy(
            linearNoise(cen.x * 9.1242, pos.y * 0.2498, pos.z * 0.3851) * deltaTime * 3.2,
            linearNoise(pos.x * 0.8426, cen.y * 4.7643, pos.z * 0.5874) * deltaTime * 4.1 + ((i % 4 - 2) * 4.5 * deltaTime),
            linearNoise(pos.x * 0.9731, pos.y * 0.4981, cen.z * 8.6415) * deltaTime * 7.2
        );
    }
    float t = _lifeTime / EXPLODING_TIME;
    float powed = -1.3 * t * t + 0.3 * t + 1;
    _node->transform().setScale(glm::vec3(powed));
}

bool ExplodingMesh::alive() const {
    return _lifeTime < EXPLODING_TIME;
}

TrashSceneController::TrashSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window) {
    cameraMoveSpeed *= 4;
}

TrashSceneController::~TrashSceneController() {
    
}

void TrashSceneController::makeScene() {
    if (_window.expired())
        return;
    initNoise(12);
    GLS::Scene& scene(*_scene);
    AppEnv *env = _window.lock()->getAppEnvPtr();

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

    std::shared_ptr<GLS::Mesh> sphereMesh = GLS::Mesh::sphere(1.0);
    std::shared_ptr<GLS::Node> sphereNode = std::make_shared<GLS::Node>();
    {
        auto sphereMat = std::make_shared<GLS::Material>();
        sphereMat->diffuse = glm::vec3(0.5, 0.1, 0.2);
        sphereMesh->setMaterial(sphereMat);
        sphereMesh->generateBuffers();
        sphereNode->addRenderable(sphereMesh);
    }
    sphereNode->setName("sphere");
    sphereNode->transform().setPosition(glm::vec3(2, 1, 0));
    planeNode->addChildNode(sphereNode);

    try {
        std::shared_ptr<GLS::Node> nsNode = std::make_shared<GLS::Node>();
        std::string nsPath = "../models/nanosuit/nanosuit.obj";
        if (env->getArgument("-model") != nullptr)
            nsPath = *env->getArgument("-model");
        nsNode->loadFromFile(nsPath);
        scene.rootNode()->addChildNode(nsNode);
    } catch (std::exception& e) {
        std::cout << "can't load object with exception: " << e.what() << std::endl;
    }

    try {
        std::shared_ptr<GLS::Node> ftNode = std::make_shared<GLS::Node>();
        ftNode->loadFromFile("../models/42.obj");
        ftNode->setName("ft");
        scene.rootNode()->addChildNode(ftNode);
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
    spotlightNode->setName("spotlight");
    scene.rootNode()->addChildNode(spotlightNode);

    std::shared_ptr<GLS::Node> pointLightNode = std::make_shared<GLS::Node>();
    std::shared_ptr<GLS::Light> pointlight = std::make_shared<GLS::Light>();
    pointlight->type = (GLS::light_point);
    pointLightNode->transform().setPosition(glm::vec3(0, 15, -7));
    pointLightNode->setLight(pointlight);
    pointLightNode->setName("pointight");
    scene.rootNode()->addChildNode(pointLightNode);

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
    cubeNode->setName("cube");
    scene.rootNode()->addChildNode(cubeNode);

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
    // skyboxFaces.push_back("assets/textures/skybox/lac_skybox/right.jpg");
    // skyboxFaces.push_back("assets/textures/skybox/lac_skybox/left.jpg");
    // skyboxFaces.push_back("assets/textures/skybox/lac_skybox/top.jpg");
    // skyboxFaces.push_back("assets/textures/skybox/lac_skybox/bottom.jpg");
    // skyboxFaces.push_back("assets/textures/skybox/lac_skybox/front.jpg");
    // skyboxFaces.push_back("assets/textures/skybox/lac_skybox/back.jpg");
    skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_ft.jpg");
    skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_bk.jpg");
    skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_up.jpg");
    skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_dn.jpg");
    skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_rt.jpg");
    skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_lf.jpg");
    try {
        std::shared_ptr<GLS::Skybox> skybox = std::make_shared<GLS::Skybox>(skyboxFaces);
        scene.setSkybox(skybox);
    } catch (std::exception& e) {
        std::cout << "can't load skybox textures with exception: " << e.what() << std::endl;
    }

    std::shared_ptr<ExplodingMesh> expl = std::make_shared<ExplodingMesh>(cubeMesh);
    scene.rootNode()->addChildNode(expl->node());
    scene.addAnimatable(expl);

}

void TrashSceneController::update() {
    ISceneController::update();
    if (!mustUpdate)
        return;
}
