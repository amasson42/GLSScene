
#include "CustomSceneControllers/CaveRunning/CaveRunningGame.hpp"

int *initNoise(unsigned int seed);

CaveRunningWorldGenerator::CaveRunningWorldGenerator(unsigned int seed,
    std::shared_ptr<CLD::GPUDevice> device) :
    _seed(seed),
    _device(device),
    _chunkCreationCommandQueueIndex(-1),
    _perlinPermutationBufferIndex(-1),
    _chunkCreationKernelIndex(-1)
{
    int *p = initNoise(seed);
    _perlinPermutationBuffer = _device->createFlagBuffer(sizeof(int) * 512, CL_MEM_USE_HOST_PTR, p, &_perlinPermutationBufferIndex);
    if (_perlinPermutationBufferIndex < 0)
        throw std::runtime_error("could not create the perlin noise");
    _chunkCreationCommandQueue = _device->createCommandQueue(&_chunkCreationCommandQueueIndex);
    if (_chunkCreationCommandQueueIndex < 0) {
        _device->destroyBuffer(_perlinPermutationBufferIndex);
        throw std::runtime_error("could not create the command queue");
    }
}

CaveRunningWorldGenerator::~CaveRunningWorldGenerator() {
    deleteRoomGenerator();
    _device->destroyBuffer(_perlinPermutationBufferIndex);
}

void CaveRunningWorldGenerator::initRoomGenerator(const std::string& filename) {
    deleteRoomGenerator();

    std::string sourceCode;
    std::ifstream kernelCodeFile(filename.c_str());
    if (kernelCodeFile.is_open()) {
        std::string line;
        while (!kernelCodeFile.eof()) {
            getline(kernelCodeFile, line);
            sourceCode += line + '\n';
        }
        kernelCodeFile.close();
    } else throw std::runtime_error("could not open the kernel file");

    int genProgramIndex;
    const char *prgmSrcs[1] = {sourceCode.c_str()};
    _device->createProgram(prgmSrcs, &genProgramIndex);
    _chunkCreationKernel = _device->createKernel(genProgramIndex, "generateRoom", &_chunkCreationKernelIndex);
    _device->destroyProgram(genProgramIndex);

    // Material initialisation
    _worldMaterial = std::make_shared<GLS::Material>();
    _worldMaterial->texture_diffuse = std::make_shared<GLS::Texture>("assets/textures/ft_vox_textures_2.png");
    _worldMaterial->texture_diffuse->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    _worldMaterial->texture_diffuse->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    _worldMaterial->texture_diffuse->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    _worldMaterial->texture_diffuse->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    _worldMaterial->texture_mask = _worldMaterial->texture_diffuse;
    _worldMaterial->specular = glm::vec3(0.1f);
    _worldMaterial->shininess = 0;
    _worldMaterial->shininess = 64.0f;
}

void CaveRunningWorldGenerator::deleteRoomGenerator() {
    if (_chunkCreationKernelIndex >= 0)
        _device->destroyKernel(_chunkCreationKernelIndex);
    _chunkCreationKernelIndex = -1;
}

std::shared_ptr<CaveRunningRoom> CaveRunningWorldGenerator::generateRoom(glm::ivec2 roomPosition, CaveMaze::Room caveRoom) {
	if (_chunkCreationKernelIndex < 0)
		throw std::runtime_error("world generator was used before reading a kernel");
	if (_perlinPermutationBufferIndex < 0)
		throw std::runtime_error("world generator was used before setting a seed");
    std::shared_ptr<CaveRunningRoom> room = std::make_shared<CaveRunningRoom>();

    std::vector<GLS::VoxelBlock> blocks(GLS::VoxelChunk::chunkBlockCount * CaveRunningRoom::voxelCount);

    // _chunkCreationKernel;
    int blocksBufferIndex = 0;
    
    // LOCK MUTEX
    CLD::Buffer blocksArrayPointersBuffer = _device->createFlagBuffer(blocks.size() * sizeof(GLS::VoxelBlock), CL_MEM_USE_HOST_PTR, &(blocks.front()), &blocksBufferIndex);

    cl_int2 bcPos;
    bcPos.s[0] = roomPosition.x;
    bcPos.s[1] = roomPosition.y;

    _chunkCreationKernel.setArgument(0, _perlinPermutationBuffer);
    _chunkCreationKernel.setArgument(1, blocksArrayPointersBuffer);
    _chunkCreationKernel.setArgument(2, bcPos);
    _chunkCreationKernel.setArgument(3, GLS::VoxelChunk::chunkSize);
    _chunkCreationKernel.setArgument(4, CaveRunningRoom::voxelWidth);

    _chunkCreationCommandQueue.runNDRangeKernel(_chunkCreationKernel, blocks.size());
    _chunkCreationCommandQueue.readBuffer(blocksArrayPointersBuffer, &(blocks.front()), blocks.size() * sizeof(GLS::VoxelBlock));
    _chunkCreationCommandQueue.finish();

    room->environementNode = std::make_shared<GLS::Node>();
    for (int i = 0; i < CaveRunningRoom::voxelCount; i++) {
        GLS::VoxelChunk currentChunk;
        currentChunk.setMaterial(_worldMaterial);
        std::memcpy(&(currentChunk.getBlocks().front()),
            &(blocks[i * GLS::VoxelChunk::chunkBlockCount]),
            sizeof(GLS::VoxelBlock) * GLS::VoxelChunk::chunkBlockCount);
        std::shared_ptr<GLS::Mesh> voxelMesh = currentChunk.bakeMesh();
        voxelMesh->generateBuffers();
        std::shared_ptr<GLS::Node> voxelOffsetNode = std::make_shared<GLS::Node>();
        voxelOffsetNode->addRenderable(voxelMesh);
        const int ww = CaveRunningRoom::voxelWidth * CaveRunningRoom::voxelWidth;
        glm::ivec3 currentPos;
        currentPos.x = (i % ww) / CaveRunningRoom::voxelWidth;
        currentPos.y = i / ww;
        currentPos.z = i % CaveRunningRoom::voxelWidth;
        voxelOffsetNode->transform().setPosition(glm::vec3(
            GLS::VoxelChunk::chunkSize * currentPos.x,
            GLS::VoxelChunk::chunkSize * currentPos.y,
            GLS::VoxelChunk::chunkSize * currentPos.z));
        room->environementNode->addChildNode(voxelOffsetNode);
    }
    _device->destroyBuffer(blocksBufferIndex);

    // UNLOCK MUTEX

    // auto theCube = std::make_shared<GLS::Node>();
    // theCube->addRenderable(GLS::Mesh::cube(1, 1, 1, 1));
    // room->environementNode->addChildNode(theCube);

    return room;
}
