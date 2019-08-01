
#include "AppEnv.hpp"

namespace glm {
	bool operator<(const glm::ivec2& lhs, const glm::ivec2& rhs) {
		if (lhs.x < rhs.x)
			return true;
		else
			return lhs.y < rhs.y;
	}

	std::ostream& operator<<(std::ostream& out, const glm::ivec2& iv) {
		out << "[" << iv.x << "; " << iv.y << "]";
		return out;
	}
}


VoxelSceneController::VoxelSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window) {
    removeBlockCD = 1.0;
    i = 0;
}

VoxelSceneController::~VoxelSceneController() {

}

void VoxelSceneController::update() {
    ISceneController::update();
    if (!mustUpdate)
        return;
    if (_window.expired())
        return;
    std::shared_ptr<GLSWindow> win = _window.lock();
    float et = win->elapsedTime();
    float dt = win->deltaTime();

    if (mainChunkNode != nullptr) {
        mainChunkNode->transform().setEulerAngles(glm::vec3(0, sin(et * 0.05), 0));
    }
    if (mainChunk != nullptr) {
        removeBlockCD -= dt;
        if (removeBlockCD <= 0) {
            while (i < GLS::VoxelChunk::chunkBlockCount && mainChunk->blockIdAt(GLS::VoxelChunk::coordinatesOfBlock(i)) == 0)
                i++;
            if (i < GLS::VoxelChunk::chunkBlockCount) {
                std::tuple<int, int, int> coord = GLS::VoxelChunk::coordinatesOfBlock(i);
                mainChunk->setBlockIdAt(std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), 0);
                i++;
            }
            mainChunk->calculBlockAdjacence();
            mainChunk->updateIdsBuffer();
            for (int c = 0; c < 6; c++) {
                if (neighbourgsChunks[c].expired())
                    continue;
                std::shared_ptr<GLS::VoxelChunk> neighbourg = neighbourgsChunks[c].lock();
                neighbourg->calculBlockAdjacence();
                neighbourg->updateIdsBuffer();
            }
            removeBlockCD += 0.05;
        }
    }
}

void VoxelSceneController::makeScene() {
    if (_window.expired())
        return;
    AppEnv *env = _window.lock()->getAppEnvPtr();
    GLS::Scene& scene(*_scene);


    auto texturedMaterial = std::make_shared<GLS::Material>();
    texturedMaterial->specular = glm::vec3(0.1);
    texturedMaterial->shininess = 64;
    try {
        std::shared_ptr<std::string> diffuseName = env->getArgument("-diffuse");
        if (diffuseName != nullptr) {
            texturedMaterial->texture_diffuse = std::make_shared<GLS::Texture>(*diffuseName);
            texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        std::shared_ptr<std::string> normalName = env->getArgument("-normal");
        if (normalName != nullptr)
            texturedMaterial->texture_normal = std::make_shared<GLS::Texture>(*normalName);
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
    scene.setCameraNode(cameraNode);
    scene.rootNode()->addChildNode(cameraNode);
    cameraNode->addChildNode(pointlightNode);
    pointlightNode->transform().moveBy(1, 0, 0);
    pointlightNode->transform().rotateEulerAnglesBy(0, 0.2, 0);

    auto chunkNode = std::make_shared<GLS::Node>();
    auto chunkMesh = std::make_shared<GLS::VoxelChunk>();
    mainChunk = chunkMesh;
    chunkNode->addRenderable(chunkMesh);
    chunkMesh->setMaterial(texturedMaterial);
    scene.rootNode()->addChildNode(chunkNode);
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
                        chunkMesh->setBlockIdAt(ix, iy, iz, 1);
                    }
                }
    }

    std::array<std::weak_ptr<GLS::VoxelChunk>, 6> adjChunks;
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
                        // || (ix + iy + iz) % 10 <= 3
                        )
                    {
                        adjChunkMesh->setBlockIdAt(ix, iy, iz, 1);
                    }
                }
    }
    neighbourgsChunks = adjChunks;

    chunkMesh->setAdjacentChunks(adjChunks);
    chunkMesh->calculBlockAdjacence();
    std::weak_ptr<GLS::VoxelChunk> nochunk;
    adjChunks[0].lock()->setAdjacentChunks({{nochunk, chunkMesh, nochunk, nochunk, nochunk, nochunk}});
    adjChunks[1].lock()->setAdjacentChunks({{chunkMesh, nochunk, nochunk, nochunk, nochunk, nochunk}});
    adjChunks[2].lock()->setAdjacentChunks({{nochunk, nochunk, nochunk, chunkMesh, nochunk, nochunk}});
    adjChunks[3].lock()->setAdjacentChunks({{nochunk, nochunk, chunkMesh, nochunk, nochunk, nochunk}});
    adjChunks[4].lock()->setAdjacentChunks({{nochunk, nochunk, nochunk, nochunk, nochunk, chunkMesh}});
    adjChunks[5].lock()->setAdjacentChunks({{nochunk, nochunk, nochunk, nochunk, chunkMesh, nochunk}});
    for (int i = 0; i < 6; i++) {
        adjChunks[i].lock()->calculBlockAdjacence();
        adjChunks[i].lock()->generateBuffers();
    }

    chunkMesh->updateIdsBuffer();
    chunkMesh->generateBuffers();

    mainChunkNode = chunkNode;
    chunkNode->transform().scaleBy(glm::vec3(0.5));
}
