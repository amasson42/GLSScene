//
//  GLSVoxelChunk.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSVoxelChunk_h
#define GLSVoxelChunk_h

#include "GLSStructs.hpp"
#include "GLSIMeshSource.hpp"

namespace GLS {

	enum VoxelBlockMeshType : uint8_t {
		/** air */
		Empty, 			// 00
		/** Full block */
		Full,			// 01
		/** Used for water and soulsand */
		ReduceHeight,	// 02
		/** Down level slab */
		SlabLow,		// 03
		/** Up level slab */
		SlabHigh,		// 04
		/** Small plate on edge like leafs and ladders */
		FullEdge,		// 05
		/** Fence */
		Fence,			// 06
		/** Cactus */
		Curved,			// 07
		/** Grass and flowers */
		Cross,			// 08
		/** Regular stair */
		Stair,			// 09
		/** Upside down stair */
		StairReversed	// 0A
	};

	enum VoxelBlockOrientation : uint8_t {
		BlockOrientation_PositiveX,
		BlockOrientation_NegativeX,
		BlockOrientation_PositiveZ,
		BlockOrientation_NegativeZ,
	};

	struct VoxelBlock {
		uint8_t customData;
		VoxelBlockOrientation orientation;
		VoxelBlockMeshType meshType;
		uint8_t textureId;

		VoxelBlock();
		VoxelBlock(VoxelBlockMeshType type, uint8_t id);
		VoxelBlock(VoxelBlockOrientation orient, VoxelBlockMeshType type, uint8_t id);
		bool operator==(const VoxelBlock& other);
		bool operator!=(const VoxelBlock& other);
	};

	enum VoxelChunkEdge : uint8_t {
		Positive_X = 0,
		Negative_X = 1,
		Positive_Y = 2,
		Negative_Y = 3,
		Positive_Z = 4,
		Negative_Z = 5,
	};

    class VoxelChunk : public IMeshSource {
    
    public:
        static const uint32_t chunkSize = 16; // min = 2
        static const uint32_t chunkBlockCount = chunkSize * chunkSize * chunkSize;
        static uint32_t indexOfBlock(glm::ivec3 coord);
        static glm::ivec3 coordinatesOfBlock(uint32_t index);
        static VoxelChunkEdge opposedEdge(VoxelChunkEdge edge);

    protected:

		std::array<VoxelBlock, chunkBlockCount> _blocks;
        std::shared_ptr<Material> _material;

        std::array<std::weak_ptr<VoxelChunk>, 6> _adjChunks;
		std::function<bool(VoxelBlock, VoxelBlock, VoxelChunkEdge)> _adjacentFunction;

        void _setBlockAdjacence(glm::ivec3 coord);

    public:

        VoxelChunk();
        VoxelChunk(const VoxelChunk& copy);
        virtual ~VoxelChunk();
        
        VoxelChunk& operator=(const VoxelChunk& copy);
        
        
        // VoxelChunk utilities

		const std::array<VoxelBlock, chunkBlockCount>& getBlocks() const;
		std::array<VoxelBlock, chunkBlockCount>& getBlocks();
		const VoxelBlock& blockAt(uint32_t coord) const;
		VoxelBlock& blockAt(uint32_t coord);
		const VoxelBlock& blockAt(glm::ivec3 coord) const;
		VoxelBlock& blockAt(glm::ivec3 coord);
		VoxelBlock adjacentBlockOf(glm::ivec3 coord, VoxelChunkEdge edge) const;

        void setAdjacentChunk(std::shared_ptr<VoxelChunk> adjChunk, VoxelChunkEdge edge);
        void setAdjacentChunks(std::array<std::weak_ptr<VoxelChunk>, 6> adjChunks);

        std::shared_ptr<VoxelChunk> adjacentChunk(VoxelChunkEdge edgeIndex);
        std::array<std::shared_ptr<VoxelChunk>, 6> adjacentChunks();

		void setAdjacentFunction(std::function<bool(VoxelBlock, VoxelBlock, VoxelChunkEdge)> closure);

        void setMaterial(std::shared_ptr<Material> mat);
        std::shared_ptr<Material> getMaterial() const;

		// Mesh

		virtual std::shared_ptr<Mesh> bakeMesh() const;

    };

}

#endif /* GLSVoxelChunk_h */
