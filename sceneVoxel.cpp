
#include "GLScene.hpp"

std::shared_ptr<GLS::Node> mainChunkNode = nullptr;
std::shared_ptr<GLS::VoxelChunk> mainChunk = nullptr;
std::array<std::shared_ptr<GLS::VoxelChunk>, 6> neighbourgsChunks = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

void updateSceneVoxel(double et, double dt) {
    static double removeBlockCD = 1.0;
    static int i = 0;

    if (mainChunkNode != nullptr) {
        mainChunkNode->transform().setEulerAngles(glm::vec3(0, sin(et * 0.05), 0));
    }
    if (mainChunk != nullptr) {
        removeBlockCD -= dt;
        if (removeBlockCD <= 0) {
            while (i < GLS::VoxelChunk::chunkBlockCount && mainChunk->blockIds()[i] == 0)
                i++;
            if (i < GLS::VoxelChunk::chunkBlockCount) {
                mainChunk->blockIds()[i] = 0;
                i++;
            }
            mainChunk->calculBlockAdjacence(neighbourgsChunks);
            mainChunk->updateIdsBuffer();
            for (int c = 0; c < 6; c++) {
                switch (c) {
                    case 0: neighbourgsChunks[c]->calculBlockAdjacence({nullptr, mainChunk, nullptr, nullptr, nullptr, nullptr}); break;
                    case 1: neighbourgsChunks[c]->calculBlockAdjacence({mainChunk, nullptr, nullptr, nullptr, nullptr, nullptr}); break;
                    case 2: neighbourgsChunks[c]->calculBlockAdjacence({nullptr, nullptr, nullptr, mainChunk, nullptr, nullptr}); break;
                    case 3: neighbourgsChunks[c]->calculBlockAdjacence({nullptr, nullptr, mainChunk, nullptr, nullptr, nullptr}); break;
                    case 4: neighbourgsChunks[c]->calculBlockAdjacence({nullptr, nullptr, nullptr, nullptr, nullptr, mainChunk}); break;
                    case 5: neighbourgsChunks[c]->calculBlockAdjacence({nullptr, nullptr, nullptr, nullptr, mainChunk, nullptr}); break;
                }
                neighbourgsChunks[c]->updateIdsBuffer();
            }
            removeBlockCD += 0.05;
        }
    }
}

void loadSceneVoxel(GLS::Scene& scene, const std::vector<std::string>& args) {

    auto texturedMaterial = std::make_shared<GLS::Material>();
    texturedMaterial->specular = glm::vec3(0.1);
    texturedMaterial->shininess = 64;
    try {
        if (args.size() >= 1) {
            texturedMaterial->texture_diffuse = std::make_shared<GLS::Texture>(args[0]);
            texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        if (args.size() >= 2)
            texturedMaterial->texture_normal = std::make_shared<GLS::Texture>(args[1]);
    } catch (std::exception& e) {
        std::cout << "error " << e.what() << std::endl;
    }

    auto pointlightNode = std::make_shared<GLS::Node>();
    auto pointlight = std::make_shared<GLS::Light>();
    pointlight->type = GLS::light_spot;
    pointlight->color = glm::vec3(1);
    pointlightNode->setLight(pointlight);

    std::shared_ptr<GLS::Node> cameraNode = std::make_shared<GLS::Node>();
    cameraNode->transform().moveBy(5, 10, 20);
    std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
    camera->aspect = (scene.getAspect());
    cameraNode->setCamera(camera);
    cameraNode->transform().moveBy(0, 2, 5);
    scene.setCameraNode(*cameraNode);
    scene.rootNode().addChildNode(cameraNode);
    cameraNode->addChildNode(pointlightNode);
    pointlightNode->transform().moveBy(1, 0, 0);
    pointlightNode->transform().rotateEulerAnglesBy(0, 0.2, 0);

    auto chunkNode = std::make_shared<GLS::Node>();
    auto chunkMesh = std::make_shared<GLS::VoxelChunk>();
    mainChunk = chunkMesh;
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
        chunkNode->addChildNode(adjChunkNode);
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
                    }
                }
    }
    neighbourgsChunks = adjChunks;

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

    chunkMesh->updateIdsBuffer();
    chunkMesh->generateBuffers();

    mainChunkNode = chunkNode;
    chunkNode->transform().scaleBy(glm::vec3(0.5));
}
