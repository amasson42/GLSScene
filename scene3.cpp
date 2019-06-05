
#include "GLScene.hpp"

float degreeToRadians(float deg) {
    return deg * M_PI / 180.0;
}

std::shared_ptr<GLS::Node> bigCubeNode = nullptr;
std::shared_ptr<GLS::Node> lightPivotNode = nullptr;
std::shared_ptr<GLS::Node> cubesPivotNode = nullptr;
std::shared_ptr<GLS::InstancedMesh> instancedMeshFloater = nullptr;

void updateScene3(double et, double dt) {
    (void)dt;
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
            float length = (float)i * 0.2 * (float)i + et * 0.5;
            GLS::Transform t = instancedMeshFloater->instanceTransformAt(i);
            t.position().y = cos(length) * 0.2;
            instancedMeshFloater->setInstanceTransformAt(i, t);
        }
        instancedMeshFloater->resetTransformsBufferValues();
    }
}

float randFloat() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void loadScene3(GLS::Scene& scene, const std::vector<std::string>& args) {
    (void)args;

    auto texturedMaterial = std::make_shared<GLS::Material>();
    texturedMaterial->specular = glm::vec3(0.1);
    texturedMaterial->shininess = 64;
    try {
        if (args.size() >= 1)
            texturedMaterial->texture_diffuse = std::make_shared<GLS::Texture>(args[0]);
        if (args.size() >= 2)
            texturedMaterial->texture_normal = std::make_shared<GLS::Texture>(args[1]);
    } catch (std::exception& e) {
        std::cout << "error " << e.what() << std::endl;
    }

    auto planeNode = std::make_shared<GLS::Node>();
    planeNode->transform().setEulerAngles(glm::vec3(-M_PI_2, 0, 0));
    auto planeMesh = GLS::Mesh::plane(10, 10);
    planeNode->addRenderable(planeMesh);
    planeNode->setName("plane");
    scene.rootNode().addChildNode(planeNode);
    
    auto cubeNode1 = std::make_shared<GLS::Node>();
    auto cubeMesh1 = GLS::Mesh::cube(1, 1, 1);
    cubeMesh1->setMaterial(texturedMaterial);
    cubeNode1->addRenderable(cubeMesh1);
    cubeNode1->setName("cube1");
    cubeNode1->transform().setPosition(glm::vec3(-1, 1.75, 1));
    scene.rootNode().addChildNode(cubeNode1);
    bigCubeNode = cubeNode1;

    cubesPivotNode = std::make_shared<GLS::Node>();
    cubesPivotNode->transform().setPosition(bigCubeNode->transform().position());
    scene.rootNode().addChildNode(cubesPivotNode);
    auto cubesNode = std::make_shared<GLS::Node>();
    auto smallCubeMesh = GLS::Mesh::cube(0.05, 0.05, 0.05);
    auto smallCubeMaterial = std::make_shared<GLS::Material>();
    smallCubeMaterial->diffuse = glm::vec3(0.8, 0.2, 0.3);
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
    scene.rootNode().addChildNode(sphereNode);

    lightPivotNode = std::make_shared<GLS::Node>();
    lightPivotNode->transform().setEulerAngles(glm::vec3(degreeToRadians(-50), degreeToRadians(50), 0));
    lightPivotNode->setName("light_pivot");
    scene.rootNode().addChildNode(lightPivotNode);

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
    pointlight->type = GLS::light_point;
    pointlight->color = glm::vec3(0.3);
    pointlightNode->setLight(pointlight);

    std::shared_ptr<GLS::Node> cameraNode = std::make_shared<GLS::Node>();
    {
        std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
        camera->aspect = (scene.getAspect());
        cameraNode->setCamera(camera);
    }
    cameraNode->transform().moveBy(0, 2, 5);
    scene.setCameraNode(*cameraNode);
    scene.rootNode().addChildNode(cameraNode);
    cameraNode->addChildNode(pointlightNode);
    pointlightNode->transform().moveBy(1, 0, 0);

    auto chunkNode = std::make_shared<GLS::Node>();
    auto chunkMesh = std::make_shared<GLS::VoxelChunk>();
    chunkNode->addRenderable(chunkMesh);
    chunkMesh->setMaterial(texturedMaterial);
    scene.rootNode().addChildNode(chunkNode);
    chunkNode->addRenderable(GLS::Mesh::cube(0.2, 0.2, 0.2));
    auto endChunkNode = std::make_shared<GLS::Node>();
    endChunkNode->transform().setPosition(glm::vec3(static_cast<float>(GLS::VoxelChunk::chunkSize)));
    chunkNode->addChildNode(endChunkNode);
    auto endChunkMesh = GLS::Mesh::cube(0.2, 0.2, 0.2);
    auto endChunkMaterial = std::make_shared<GLS::Material>();
    endChunkMaterial->diffuse = glm::vec3(1, 0, 0);
    endChunkMesh->setMaterial(endChunkMaterial);
    endChunkNode->addRenderable(endChunkMesh);

    {
        std::cout << "testing..." << std::endl;
        for (int ix = 0; ix < GLS::VoxelChunk::chunkSize; ix++)
            for (int iy = 0; iy < GLS::VoxelChunk::chunkSize; iy++)
                for (int iz = 0; iz < GLS::VoxelChunk::chunkSize; iz++) {
                    std::tuple<int, int, int> blockCoordinates = GLS::VoxelChunk::coordinatesOfBlock(GLS::VoxelChunk::indexOfBlock(ix, iy, iz));
                    if (std::get<0>(blockCoordinates) != ix)
                        std::cout << "error block x" << std::endl;
                    if (std::get<1>(blockCoordinates) != iy)
                        std::cout << "error block y" << std::endl;
                    if (std::get<2>(blockCoordinates) != iz)
                        std::cout << "error block z" << std::endl;
                    if (((ix - 8) * (ix - 8) + (iy - 8) * (iy - 8) + (iz - 8) * (iz - 8) < 70)
                        )
                    {
                        chunkMesh->blockIds()[GLS::VoxelChunk::indexOfBlock(ix, iy, iz)] = 1;
                        auto smallSphereNode = std::make_shared<GLS::Node>();
                        smallSphereNode->addRenderable(sphereMesh);
                        smallSphereNode->transform().setPosition(glm::vec3(ix + 0.5, iy + 0.5, iz + 0.5));
                        smallSphereNode->transform().scaleBy(glm::vec3(0.5));
                        chunkNode->addChildNode(smallSphereNode);
                    }
                }
    }

    std::array<std::shared_ptr<GLS::VoxelChunk>, 6> adjChunks;
    for (int a = 0; a < 6; a++) {
        auto adjChunkNode = std::make_shared<GLS::Node>();
        auto adjChunkMesh = std::make_shared<GLS::VoxelChunk>();
        adjChunks[a] = adjChunkMesh;
        adjChunkNode->addRenderable(adjChunkMesh);
        adjChunkMesh->setMaterial(endChunkMaterial);
        scene.rootNode().addChildNode(adjChunkNode);
        float offset = GLS::VoxelChunk::chunkSize * 1.0;
        glm::vec3 offsetPos(0);
        switch (a) {
            case 0: offsetPos.x = offset; break;
            case 1: offsetPos.x = -offset; break;
            case 2: offsetPos.y = offset; break;
            case 3: offsetPos.y = -offset; break;
            case 4: offsetPos.z = offset; break;
            case 5: offsetPos.z = -offset; break;
        }
        adjChunkNode->transform().setPosition(offsetPos);
        for (int ix = 0; ix < GLS::VoxelChunk::chunkSize; ix++)
            for (int iy = 0; iy < GLS::VoxelChunk::chunkSize; iy++)
                for (int iz = 0; iz < GLS::VoxelChunk::chunkSize; iz++) {
                    if (((ix - 8) * (ix - 8) + (iy - 8) * (iy - 8) + (iz - 8) * (iz - 8) < 70)
                        || (ix + iy + iz) % 10 <= 3
                        )
                    {
                        adjChunkMesh->blockIds()[GLS::VoxelChunk::indexOfBlock(ix, iy, iz)] = 1;
                        auto smallSphereNode = std::make_shared<GLS::Node>();
                        smallSphereNode->addRenderable(cubeMesh1);
                        smallSphereNode->transform().setPosition(glm::vec3(ix + 0.5, iy + 0.5, iz + 0.5));
                        smallSphereNode->transform().scaleBy(glm::vec3(0.5));
                        adjChunkNode->addChildNode(smallSphereNode);
                    }
                }
    }

    adjChunks[0]->blockAt(0, 8, 8);// +x
    adjChunks[1]->blockAt(GLS::VoxelChunk::chunkSize - 1, 8, 8);// -x
    adjChunks[2]->blockAt(8, 0, 8);// +y
    adjChunks[3]->blockAt(8, GLS::VoxelChunk::chunkSize - 1, 8);// -y
    adjChunks[4]->blockAt(8, 8, 0);// +z
    adjChunks[5]->blockAt(8, 8, GLS::VoxelChunk::chunkSize - 1);// -z

    chunkMesh->calculBlockAdjacence(adjChunks);
    adjChunks[0]->calculBlockAdjacence({nullptr, chunkMesh, nullptr, nullptr, nullptr, nullptr});
    adjChunks[1]->calculBlockAdjacence({chunkMesh, nullptr, nullptr, nullptr, nullptr, nullptr});
    adjChunks[2]->calculBlockAdjacence({nullptr, nullptr, nullptr, chunkMesh, nullptr, nullptr});
    adjChunks[3]->calculBlockAdjacence({nullptr, nullptr, chunkMesh, nullptr, nullptr, nullptr});
    adjChunks[4]->calculBlockAdjacence({nullptr, nullptr, nullptr, nullptr, nullptr, chunkMesh});
    adjChunks[5]->calculBlockAdjacence({nullptr, nullptr, nullptr, nullptr, chunkMesh, nullptr});

    for (int i = 0; i < 6; i++)
        adjChunks[i]->generateBuffers();

    chunkMesh->resetIdsBufferValues();
    chunkMesh->generateBuffers();
}
