#include "DynamicWorld.hpp"

namespace glm {
	bool operator<(const glm::ivec2& lhs, const glm::ivec2& rhs);
	// std::ostream& operator<<(std::ostream& out, const glm::ivec2& iv);
}

const float DynamicWorld::minRenderDistance = 50.0f;
const float DynamicWorld::maxRenderDistance = 250.0f;

// world/
//  info.json ->
//  {
//     "settings": {
//       "seed": int,
//       "generatorName": string,
//		 "renderDistance": int,
//       "meshmerizeEffect": float,
//       "castShadow": bool
//	   },
//     "player": {
//       "position": [float, float, float],
//       "rotation": [float, float],
//       "speed": float,
//       "fov": float,
//       "pickedBlockIndex": int
//	   },
//  }
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

	std::vector < std::pair<glm::ivec2, std::shared_ptr<BigChunk>>>::iterator it = _loadedChunks.begin();

	while (it != _loadedChunks.end()) {
		glm::vec3 chunkPosition = it->second->getNode()->transform().position();
		glm::vec3 chunkOffset = (chunkPosition + chunkMid) - cameraFlatPosition;
		if (glm::dot(chunkOffset, chunkOffset) > _loadingDistance * _loadingDistance) {
			// std::async(std::launch::async, &BigChunk::save, it->second, getBigChunkFileNameAt(it->first));
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
	float minCosCameraVision = static_cast<float>(cos(1.4 * camera->fov * camera->aspect / 2));
	glm::vec3 chunkMid = glm::vec3(CHUNKSIZE * BigChunk::bigChunkWidth / 2,
									0,
									CHUNKSIZE * BigChunk::bigChunkWidth / 2);

	glm::ivec2 minPosition = worldToBigChunkPosition(cameraFlatPosition - glm::vec3(_loadingDistance, 0, _loadingDistance) - chunkMid);
	glm::ivec2 maxPosition = worldToBigChunkPosition(cameraFlatPosition + glm::vec3(_loadingDistance, 0, _loadingDistance) + chunkMid);

	for (int x = minPosition.x; x <= maxPosition.x; x++) {
		for (int y = minPosition.y; y <= maxPosition.y; y++) {

			glm::ivec2 pos(x, y);
			auto it1 = std::find_if(_loadedChunks.begin(), _loadedChunks.end(), [pos](std::pair<glm::ivec2, std::shared_ptr<BigChunk>>& elem) {
				return elem.first.x == pos.x && elem.first.y == pos.y;
			});
			auto it2 = std::find_if(_loadingChunks.begin(), _loadingChunks.end(), [pos](std::pair<glm::ivec2, std::future<std::shared_ptr<BigChunk>>>& elem) {
				return elem.first.x == pos.x && elem.first.y == pos.y;
			});
			if (it1 != _loadedChunks.end() || it2 != _loadingChunks.end()) {
				continue;
			}
			glm::vec3 chunkOffset = bigChunkPositionToWorld(pos) + chunkMid - cameraFlatPosition;

			float chunkOffsetSquaredLength = glm::dot(chunkOffset, chunkOffset);
			if (chunkOffsetSquaredLength < _loadingDistance * _loadingDistance) {

				glm::vec3 cameraDirection = glm::vec3(cameraNode->transform().matrix() * glm::vec4(0, 0, -1, 0));
				cameraDirection = glm::normalize(glm::vec3(1, 0, 1) * cameraDirection);

				if (chunkOffsetSquaredLength < glm::dot(chunkMid, chunkMid)
					|| glm::dot(glm::normalize(chunkOffset), cameraDirection) > minCosCameraVision) {
					_loadingChunks.push_back(std::make_pair(pos, (std::async(std::launch::async, [this](glm::ivec2 pos) {
						std::ifstream chunkStream(getBigChunkFileNameAt(pos), std::ios::binary);
						std::shared_ptr<BigChunk> chunk;
						if (false && chunkStream.good()) {
							chunk = std::make_shared<BigChunk>(_generator->usedMaterial);
							chunk->loadFromStream(chunkStream);
							chunkStream.close();
						} else {
							chunk = _generator->generateBigChunkAt(pos);
						}
						chunk->calculAllAdjacences();
						chunk->generateAllMeshes();
						return chunk;
					}, glm::ivec2(x, y)))));
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
		_loadedChunks.push_back(std::make_pair(it->first, generatedChunk));
		_worldNode->addChildNode(generatedChunk->getNode());

		// define adjacent
		auto adjacent = _loadedChunks.begin();
		int x = it->first.x;
		int y = it->first.y;

		glm::ivec2 pos(x + 1, y);
		adjacent = std::find_if(_loadedChunks.begin(), _loadedChunks.end(), [pos](std::pair<glm::ivec2, std::shared_ptr<BigChunk>>& elem) {
			return elem.first.x == pos.x && elem.first.y == pos.y;
		});
		if (adjacent != _loadedChunks.end()) {
			generatedChunk->setAdjacentBigChunk_positiveX(adjacent->second);
			adjacent->second->setAdjacentBigChunk_negativeX(generatedChunk);
		}

		pos = glm::ivec2(x - 1, y);
		adjacent = std::find_if(_loadedChunks.begin(), _loadedChunks.end(), [pos](std::pair<glm::ivec2, std::shared_ptr<BigChunk>>& elem) {
			return elem.first.x == pos.x && elem.first.y == pos.y;
		});
		if (adjacent != _loadedChunks.end()) {
			generatedChunk->setAdjacentBigChunk_negativeX(adjacent->second);
			adjacent->second->setAdjacentBigChunk_positiveX(generatedChunk);
		}

		pos = glm::ivec2(x, y + 1);
		adjacent = std::find_if(_loadedChunks.begin(), _loadedChunks.end(), [pos](std::pair<glm::ivec2, std::shared_ptr<BigChunk>>& elem) {
			return elem.first.x == pos.x && elem.first.y == pos.y;
		});
		if (adjacent != _loadedChunks.end()) {
			generatedChunk->setAdjacentBigChunk_positiveZ(adjacent->second);
			adjacent->second->setAdjacentBigChunk_negativeZ(generatedChunk);
		}

		pos = glm::ivec2(x, y - 1);
		adjacent = std::find_if(_loadedChunks.begin(), _loadedChunks.end(), [pos](std::pair<glm::ivec2, std::shared_ptr<BigChunk>>& elem) {
			return elem.first.x == pos.x && elem.first.y == pos.y;
		});
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
	float minCosCameraVision = static_cast<float>(cos(1.4 * camera->fov * camera->aspect / 2));

	int updatedMeshCount = 0;
	std::vector < std::pair <glm::ivec2, std::shared_ptr<BigChunk> >>::iterator it = _loadedChunks.begin();
	it = _loadedChunks.begin();
	while (it != _loadedChunks.end()) {
		std::shared_ptr<BigChunk> bigChunk = it->second;
		for (int i = 0; i < BigChunk::bigChunkCount; i++) {
			std::shared_ptr<GameVoxelChunk> chunk(bigChunk->chunkAt(i));
			glm::vec3 chunkWorldPos = bigChunk->getNode()->transform().position() + chunk->node->transform().position() + glm::vec3(CHUNKSIZE / 2);
			glm::vec3 cameraPos = cameraNode->transform().position();
			glm::vec3 chunkDirection = chunkWorldPos - cameraPos;
			float squaredLength = glm::dot(chunkDirection, chunkDirection);
			if (squaredLength < _visibleDistance * _visibleDistance) {
				if (squaredLength < 3 * CHUNKSIZE * CHUNKSIZE) {
					chunk->node->setActive(true);
				} else {
					chunkDirection = glm::normalize(chunkDirection);
					glm::vec3 cameraDirection = glm::vec3(cameraNode->transform().matrix() * glm::vec4(0, 0, -1, 0));
					float cosangle = glm::dot(chunkDirection, cameraDirection);
					chunk->node->setActive(cosangle > minCosCameraVision);
				}
			} else {
				chunk->node->setActive(false);
			}
			if (chunk->mustUpdateMesh && chunk->node->isActive()) {
				if (updatedMeshCount > 300) {
					continue;
				}
				chunk->updateMesh();
				updatedMeshCount++;
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
	_loadingDistance = distance + 10.0f;
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

void DynamicWorld::setBlockAt(const glm::vec3& worldPosition, GLS::VoxelBlock block) {
	glm::ivec2 bigChunkTargetPos = worldToBigChunkPosition(worldPosition);
	if (worldPosition.x < 0)
		bigChunkTargetPos.x--;
	if (worldPosition.z < 0)
		bigChunkTargetPos.y--;

	auto it = std::find_if(_loadedChunks.begin(), _loadedChunks.end(), [bigChunkTargetPos](std::pair<glm::ivec2, std::shared_ptr<BigChunk>>& elem) {
		return bigChunkTargetPos.x == elem.first.x && bigChunkTargetPos.y == elem.first.y;
	});
	if (it == _loadedChunks.end()) {
		return;
	}
	std::shared_ptr<BigChunk> targetBigChunk = it->second;
	glm::vec3 inBigChunkPos = worldPosition - targetBigChunk->getNode()->transform().position();
	std::shared_ptr<GameVoxelChunk> targetVoxel = targetBigChunk->chunkAt(inBigChunkPos);
	if (targetVoxel == nullptr) {
		return;
	}
	glm::vec3 inVoxelPos = inBigChunkPos - targetVoxel->node->transform().position();
	targetVoxel->setBlockAt(glm::ivec3(inVoxelPos), block);
	targetVoxel->updateMesh();
}

void DynamicWorld::setWorldDirName(std::string worldDirName) {
	_worldDirName = worldDirName;
}
