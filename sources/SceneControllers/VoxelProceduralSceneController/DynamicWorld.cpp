#include "DynamicWorld.hpp"

namespace glm {
	bool operator<(const glm::ivec2& lhs, const glm::ivec2& rhs);
	std::ostream& operator<<(std::ostream& out, const glm::ivec2& iv);
}

const float DynamicWorld::minRenderDistance = 50.0f;
const float DynamicWorld::maxRenderDistance = 250.0f;

// world/
//  info.json -> {"seed": 421337}
//  C_X_Z.chunk
//  C_[...]_[...].chunk
DynamicWorld::DynamicWorld(std::shared_ptr<GLS::Node> worldNode, std::string worldName) :
	_loadedChunks(), _loadingDistance(200.0f), _visibleDistance(200.0f) {
		_worldNode = worldNode;
		_worldDirName = worldName;
		_generator = std::make_shared<ProceduralWorldGenerator>();
}

glm::ivec2 DynamicWorld::worldToBigChunkPosition(glm::vec3 position) {
	return glm::ivec2(position.x / (BigChunk::bigChunkWidth * CHUNKSIZE),
						position.z / (BigChunk::bigChunkWidth * CHUNKSIZE));
}

glm::vec3 DynamicWorld::bigChunkPositionToWorld(glm::ivec2 position) {
	return glm::vec3(position.x * BigChunk::bigChunkWidth * CHUNKSIZE,
						0,
						position.y * BigChunk::bigChunkWidth * CHUNKSIZE);
}

const std::string DynamicWorld::getBigChunkFileNameAt(glm::ivec2 position) {
	return _worldDirName + "/C_" + std::to_string(position.x) + "_" + std::to_string(position.y) + ".chunk";
}


void DynamicWorld::_cleanChunks(const glm::vec3& cameraFlatPosition) {

	glm::vec3 chunkMid = glm::vec3(CHUNKSIZE * BigChunk::bigChunkWidth / 2,
									0,
									CHUNKSIZE * BigChunk::bigChunkWidth / 2);

	std::map<glm::ivec2, std::shared_ptr<BigChunk> >::iterator it = _loadedChunks.begin();

	while (it != _loadedChunks.end()) {
		glm::vec3 chunkPosition = it->second->getNode()->transform().position();
		glm::vec3 chunkOffset = (chunkPosition + chunkMid) - cameraFlatPosition;
		if (glm::dot(chunkOffset, chunkOffset) > _loadingDistance * _loadingDistance) {

			// it->second->save(getBigChunkFileNameAt(it->first));

			it = _loadedChunks.erase(it);
		} else {

			++it;
		}

	}
}

void DynamicWorld::_generateChunks(const glm::vec3& cameraFlatPosition, std::shared_ptr<GLS::Node> cameraNode) {
	std::shared_ptr<GLS::Camera> camera = cameraNode->camera();
	if (camera == nullptr)
		return;
	float minCosCameraVision = cos(1.4 * camera->fov * camera->aspect / 2);
	glm::vec3 chunkMid = glm::vec3(CHUNKSIZE * BigChunk::bigChunkWidth / 2,
									0,
									CHUNKSIZE * BigChunk::bigChunkWidth / 2);

	glm::ivec2 minPosition = worldToBigChunkPosition(cameraFlatPosition - glm::vec3(_loadingDistance, 0, _loadingDistance) - chunkMid);
	glm::ivec2 maxPosition = worldToBigChunkPosition(cameraFlatPosition + glm::vec3(_loadingDistance, 0, _loadingDistance) + chunkMid);

	for (int x = minPosition.x; x < maxPosition.x; x++) {
		for (int y = minPosition.y; y < maxPosition.y; y++) {

			if (_loadedChunks.count(glm::ivec2(x, y)) > 0 || _loadingChunks.count(glm::ivec2(x, y)) > 0)
				continue;
			glm::vec3 chunkOffset = bigChunkPositionToWorld(glm::ivec2(x, y)) + chunkMid - cameraFlatPosition;

			float chunkOffsetSquaredLength = glm::dot(chunkOffset, chunkOffset);
			if (chunkOffsetSquaredLength < _loadingDistance * _loadingDistance) {

				glm::vec3 cameraDirection = glm::vec3(cameraNode->transform().matrix() * glm::vec4(0, 0, -1, 0));
				cameraDirection = glm::normalize(glm::vec3(1, 0, 1) * cameraDirection);

				if (chunkOffsetSquaredLength < glm::dot(chunkMid, chunkMid)
					|| glm::dot(glm::normalize(chunkOffset), cameraDirection) > minCosCameraVision) {

					_loadingChunks[glm::ivec2(x, y)] = (std::async(std::launch::async, [this](glm::ivec2 pos) {
						std::ifstream chunkStream(getBigChunkFileNameAt(pos), std::ios::binary);
						std::shared_ptr<BigChunk> chunk;
						if (false && chunkStream.good()) {
							chunk = std::make_shared<BigChunk>(_generator->usedMaterial);
							chunk->loadFromStream(chunkStream);
							chunkStream.close();
						} else {
							chunk =  _generator->generateBigChunkAt(pos);
						}
						chunk->calculAllAdjacences();
						chunk->generateAllMeshes();
						return chunk;
					}, glm::ivec2(x, y)));

				}

			}
		}
	}

	for (auto it = _loadingChunks.begin(); it != _loadingChunks.end();) {
		auto status = it->second.wait_for(std::chrono::milliseconds(0));
		if (status != std::future_status::ready) {
			++it;
			continue;
		}
		std::shared_ptr<BigChunk> generatedChunk = it->second.get();

		generatedChunk->getNode()->transform().setPosition(bigChunkPositionToWorld(it->first));

		_loadedChunks[it->first] = generatedChunk;
		_worldNode->addChildNode(generatedChunk->getNode());

		// define adjacent
		auto adjacent = _loadedChunks.begin();
		int x = it->first.x;
		int y = it->first.y;

		adjacent = _loadedChunks.find(glm::ivec2(x + 1, y));
		if (adjacent != _loadedChunks.end()) {
			generatedChunk->setAdjacentBigChunk_positiveX(adjacent->second);
			adjacent->second->setAdjacentBigChunk_negativeX(generatedChunk);
		}

		adjacent = _loadedChunks.find(glm::ivec2(x - 1, y));
		if (adjacent != _loadedChunks.end()) {
			generatedChunk->setAdjacentBigChunk_negativeX(adjacent->second);
			adjacent->second->setAdjacentBigChunk_positiveX(generatedChunk);
		}

		adjacent = _loadedChunks.find(glm::ivec2(x, y + 1));
		if (adjacent != _loadedChunks.end()) {
			generatedChunk->setAdjacentBigChunk_positiveZ(adjacent->second);
			adjacent->second->setAdjacentBigChunk_negativeZ(generatedChunk);
		}

		adjacent = _loadedChunks.find(glm::ivec2(x, y - 1));
		if (adjacent != _loadedChunks.end()) {
			generatedChunk->setAdjacentBigChunk_negativeZ(adjacent->second);
			adjacent->second->setAdjacentBigChunk_positiveZ(generatedChunk);
		}

		it = _loadingChunks.erase(it);
	}
	
}

void DynamicWorld::_generateMeshes(std::shared_ptr<GLS::Node> cameraNode) {

	std::shared_ptr<GLS::Camera> camera = cameraNode->camera();
	if (camera == nullptr)
		return;
	float minCosCameraVision = cos(1.4 * camera->fov * camera->aspect / 2);

	std::map<glm::ivec2, std::shared_ptr<BigChunk> >::iterator it = _loadedChunks.begin();
	it = _loadedChunks.begin();
	while (it != _loadedChunks.end()) {
		std::shared_ptr<BigChunk> bigChunk = it->second;
		for (int i = 0; i < BigChunk::bigChunkCount; i++) {
			GameVoxelChunk& chunk(bigChunk->chunkAt(i));
			if (chunk.mustUpdateMesh) {
				chunk.updateMesh();
			}
			glm::vec3 chunkWorldPos = bigChunk->getNode()->transform().position() + chunk.node->transform().position() + glm::vec3(CHUNKSIZE / 2);
			glm::vec3 cameraPos = cameraNode->transform().position();
			glm::vec3 chunkDirection = chunkWorldPos - cameraPos;
			float squaredLength = glm::dot(chunkDirection, chunkDirection);
			if (squaredLength < _visibleDistance * _visibleDistance) {
				if (squaredLength < 3 * CHUNKSIZE * CHUNKSIZE) {
					chunk.node->setActive(true);
				} else {
					chunkDirection = glm::normalize(chunkDirection);
					glm::vec3 cameraDirection = glm::vec3(cameraNode->transform().matrix() * glm::vec4(0, 0, -1, 0));
					float cosangle = glm::dot(chunkDirection, cameraDirection);
					chunk.node->setActive(cosangle > minCosCameraVision);
				}
			} else {
				chunk.node->setActive(false);
			}
		}

		++it;
	}
}

void DynamicWorld::loadPosition(std::shared_ptr<GLS::Node> cameraNode) {

	glm::vec3 cameraFlatPosition = cameraNode->transform().position();
	cameraFlatPosition.y = 0;


	auto start = std::chrono::system_clock::now();
	_cleanChunks(cameraFlatPosition);
	auto end = std::chrono::system_clock::now();
	// std::cout << "removeFromParent: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl; 


	start = std::chrono::system_clock::now();	   
	_generateChunks(cameraFlatPosition, cameraNode);
	end = std::chrono::system_clock::now();
	// std::cout << "generateBigChunkLoop: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl; 


	start = std::chrono::system_clock::now();
	_generateMeshes(cameraNode);
	end = std::chrono::system_clock::now();
	// std::cout << "setActiveChunks && updateMesh: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl; 

	// std::cout << "We have " << _loadedChunks.size() << " chunks" << std::endl;
}

void DynamicWorld::setRenderDistance(float distance) {
	_visibleDistance = distance;
	_loadingDistance = distance + 10.0;
}

float DynamicWorld::getRenderDistance() const {
	return _visibleDistance;
}

void DynamicWorld::reloadChunks() {
	float loadingDistance = _loadingDistance;
	_loadingDistance = 0;
	_cleanChunks(glm::vec3(0));
	_loadingDistance = loadingDistance;
	_loadedChunks.clear();
}

std::shared_ptr<ProceduralWorldGenerator> DynamicWorld::getGenerator() {
	return _generator;
}
