//
//  GLSVoxelChunk_utilities.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSVoxelChunk.hpp"

namespace GLS {

    int VoxelChunk::indexOfBlock(int x, int y, int z) {
        return (z * chunkSize * chunkSize + y * chunkSize + x);
    }

    std::tuple<int, int, int> VoxelChunk::coordinatesOfBlock(int i) {
        return std::make_tuple(i % chunkSize,
                                (i % (chunkSize * chunkSize)) / chunkSize,
                                i / (chunkSize * chunkSize));
    }

    int VoxelChunk::blockIdAt(int x, int y, int z) const {
        return _blockIds[indexOfBlock(x, y, z)] & 0xFFFF;
    }

    int VoxelChunk::blockIdAt(std::tuple<int, int, int> coord) const {
        return blockIdAt(std::get<0>(coord), std::get<1>(coord), std::get<2>(coord));
    }

    static bool _checkIsEmpty(int *blockIds) {
        for (int i = 0; i < VoxelChunk::chunkBlockCount; i++) {
            if ((blockIds[i] & 0xFFFF) != 0)
                return false;
        }
        return true;
    }

    static bool _checkFullEdge_px(int *blockIds) {
        for (int i = 0; i < VoxelChunk::chunkSize; i++)
            for (int j = 0; j < VoxelChunk::chunkSize; j++)
                if ((blockIds[VoxelChunk::indexOfBlock(VoxelChunk::chunkSize - 1, i, j)]) == 0)
                    return false;
        return true;
    }

    static bool _checkFullEdge_nx(int *blockIds) {
        for (int i = 0; i < VoxelChunk::chunkSize; i++)
            for (int j = 0; j < VoxelChunk::chunkSize; j++)
                if ((blockIds[VoxelChunk::indexOfBlock(0, i, j)]) == 0)
                    return false;
        return true;
    }

    static bool _checkFullEdge_py(int *blockIds) {
        for (int i = 0; i < VoxelChunk::chunkSize; i++)
            for (int j = 0; j < VoxelChunk::chunkSize; j++)
                if ((blockIds[VoxelChunk::indexOfBlock(i, VoxelChunk::chunkSize - 1, j)]) == 0)
                    return false;
        return true;
    }

    static bool _checkFullEdge_ny(int *blockIds) {
        for (int i = 0; i < VoxelChunk::chunkSize; i++)
            for (int j = 0; j < VoxelChunk::chunkSize; j++)
                if ((blockIds[VoxelChunk::indexOfBlock(i, 0, j)]) == 0)
                    return false;
        return true;
    }

    static bool _checkFullEdge_pz(int *blockIds) {
        for (int i = 0; i < VoxelChunk::chunkSize; i++)
            for (int j = 0; j < VoxelChunk::chunkSize; j++)
                if ((blockIds[VoxelChunk::indexOfBlock(i, j, VoxelChunk::chunkSize - 1)]) == 0)
                    return false;
        return true;
    }

    static bool _checkFullEdge_nz(int *blockIds) {
        for (int i = 0; i < VoxelChunk::chunkSize; i++)
            for (int j = 0; j < VoxelChunk::chunkSize; j++)
                if ((blockIds[VoxelChunk::indexOfBlock(i, j, 0)]) == 0)
                    return false;
        return true;
    }

    void VoxelChunk::setBlockIdAt(int x, int y, int z, int id, bool sureInside) {
        
        if (!sureInside) {
            if (x < 0) {
                if (!_adjChunks[1].expired())
                    _adjChunks[1].lock()->setBlockIdAt(x + chunkSize, y, z, id, false);
                return;
            }
            if (x >= chunkSize) {
                if (!_adjChunks[0].expired())
                    _adjChunks[0].lock()->setBlockIdAt(x - chunkSize, y, z, id, false);
                return;
            }
            if (y < 0) {
                if (!_adjChunks[3].expired())
                    _adjChunks[3].lock()->setBlockIdAt(x, y + chunkSize, z, id, false);
                return;
            }
            if (y >= chunkSize) {
                if (!_adjChunks[2].expired())
                    _adjChunks[2].lock()->setBlockIdAt(x, y - chunkSize, z, id, false);
                return;
            }
            if (z < 0) {
                if (!_adjChunks[5].expired())
                    _adjChunks[5].lock()->setBlockIdAt(x, y, z + chunkSize, id, false);
                return;
            }
            if (z >= chunkSize) {
                if (!_adjChunks[4].expired())
                    _adjChunks[4].lock()->setBlockIdAt(x, y, z - chunkSize, id, false);
                return;
            }
        }

        _blockIds[indexOfBlock(x, y, z)] = id;

        if (id == 0 && !_isEmpty)
            _isEmpty = _checkIsEmpty(_blockIds);
        else
            _isEmpty = false;

        if (x == chunkSize - 1) {
            if (id == 0)
                _fullEdges[0] = false;
            else
                _fullEdges[0] = _checkFullEdge_px(_blockIds);
        } else if (x == 0) {
            if (id == 0)
                _fullEdges[1] = false;
            else
                _fullEdges[1] = _checkFullEdge_nx(_blockIds);
        }
        if (y == chunkSize - 1) {
            if (id == 0)
                _fullEdges[2] = false;
            else
                _fullEdges[2] = _checkFullEdge_py(_blockIds);
        } else if (y == 0) {
            if (id == 0)
                _fullEdges[3] = false;
            else
                _fullEdges[3] = _checkFullEdge_ny(_blockIds);
        }
        if (z == chunkSize - 1) {
            if (id == 0)
                _fullEdges[4] = false;
            else
                _fullEdges[4] = _checkFullEdge_pz(_blockIds);
        } else if (z == 0) {
            if (id == 0)
                _fullEdges[5] = false;
            else
                _fullEdges[5] = _checkFullEdge_nz(_blockIds);
        }

    }

    int VoxelChunk::blockAdjAt(int x, int y, int z) const {
        return (_blockIds[indexOfBlock(x, y, z)] & 0xFF0000) >> 16;
    }

    bool VoxelChunk::isEmpty() const {
        return _isEmpty;
    }

    bool VoxelChunk::isFullOnEdge(int edgeIndex) const {
        return _fullEdges[edgeIndex];
    }

    int VoxelChunk::opposedFace(int f) {
        return ((f / 2) * 2) + (1 - (f % 2));
    }

    bool VoxelChunk::isSurrounded() const {
        for (int i = 0; i < 6; i++) {
            if (_adjChunks[i].expired())
                return false;
            std::shared_ptr<VoxelChunk> adj = _adjChunks[i].lock();
            if (adj->isFullOnEdge(opposedFace(i)) == false)
                return false;
        }
        return true;
    }

    void VoxelChunk::setAdjacentChunk(std::shared_ptr<VoxelChunk> adjChunk, int adji) {
        _adjChunks[adji] = adjChunk;
    }

    void VoxelChunk::setAdjacentChunks(std::array<std::weak_ptr<VoxelChunk>, 6> adjChunks) {
        _adjChunks = adjChunks;
    }

    std::shared_ptr<VoxelChunk> VoxelChunk::adjacentChunk(int edgeIndex) {
        return _adjChunks[edgeIndex].expired() ? nullptr : _adjChunks[edgeIndex].lock();
    }

    std::array<std::shared_ptr<VoxelChunk>, 6> VoxelChunk::adjacentChunks() {
        std::array<std::shared_ptr<VoxelChunk>, 6> adj;
        for (int i = 0; i < 6; i++)
            adj[i] = _adjChunks[i].expired() ? nullptr : _adjChunks[i].lock();
        return adj;
    }

    static int _calculBlockAdjacence(const VoxelChunk& chunk, int x, int y, int z, const std::array<std::shared_ptr<VoxelChunk>, 6>& adjChunks) {
        if (chunk.blockIdAt(x, y, z) == 0)
            return 0;
        
        int adj = 0;
        if (x >= VoxelChunk::chunkSize - 1) {
            if (adjChunks[0] == nullptr || adjChunks[0]->blockIdAt(0, y, z) == 0)
                adj |= (1 << 0);
        } else if (chunk.blockIdAt(x + 1, y, z) == 0)
            adj |= (1 << 0);
        
        if (x <= 0) {
            if (adjChunks[1] == nullptr || adjChunks[1]->blockIdAt(VoxelChunk::chunkSize - 1, y, z) == 0)
                adj |= (1 << 1);
        } else if (chunk.blockIdAt(x - 1, y, z) == 0)
            adj |= (1 << 1);

        if (y >= VoxelChunk::chunkSize - 1) {
            if (adjChunks[2] == nullptr || adjChunks[2]->blockIdAt(x, 0, z) == 0)
                adj |= (1 << 2);
        } else if (chunk.blockIdAt(x, y + 1, z) == 0)
            adj |= (1 << 2);
        
        if (y <= 0) {
            if (adjChunks[3] == nullptr || adjChunks[3]->blockIdAt(x, VoxelChunk::chunkSize - 1, z) == 0)
                adj |= (1 << 3);
        } else if (chunk.blockIdAt(x, y - 1, z) == 0)
            adj |= (1 << 3);

        if (z >= VoxelChunk::chunkSize - 1) {
            if (adjChunks[4] == nullptr || adjChunks[4]->blockIdAt(x, y, 0) == 0)
                adj |= (1 << 4);
        } else if (chunk.blockIdAt(x, y, z + 1) == 0)
            adj |= (1 << 4);
        
        if (z <= 0) {
            if (adjChunks[5] == nullptr || adjChunks[5]->blockIdAt(x, y, VoxelChunk::chunkSize - 1) == 0)
                adj |= (1 << 5);
        } else if (chunk.blockIdAt(x, y, z - 1) == 0)
            adj |= (1 << 5);

        return adj;
    }

    void VoxelChunk::calculBlockAdjacence() {
        std::array<std::shared_ptr<VoxelChunk>, 6> adjChunks = adjacentChunks();

        for (int x = 0; x < chunkSize; x++)
            for (int y = 0; y < chunkSize; y++)
                for (int z = 0; z < chunkSize; z++) {
                    _blockIds[indexOfBlock(x, y, z)] = blockIdAt(x, y, z) | (_calculBlockAdjacence(*this, x, y, z, adjChunks) << 16);
                }
    }

    void VoxelChunk::calculBlockAdjacence(int x, int y, int z) {
        std::array<std::shared_ptr<VoxelChunk>, 6> adjChunks = adjacentChunks();

        _calculBlockAdjacence(*this, x, y, z, adjChunks);
        if (x == 0) {
            if (adjChunks[1] != nullptr)
                _calculBlockAdjacence(*adjChunks[1].get(), chunkSize - 1, y, z, adjChunks[1]->adjacentChunks());
        } else {
            _calculBlockAdjacence(*this, x - 1, y, z, adjChunks);
        }

        if (x == chunkSize - 1) {
            if (adjChunks[0] != nullptr)
                _calculBlockAdjacence(*adjChunks[0].get(), 0, y, z, adjChunks[0]->adjacentChunks());
        } else {
            _calculBlockAdjacence(*this, x + 1, y, z, adjChunks);
        }

        if (y == 0) {
            if (adjChunks[3] != nullptr)
                _calculBlockAdjacence(*adjChunks[3].get(), x, chunkSize - 1, z, adjChunks[3]->adjacentChunks());
        } else {
            _calculBlockAdjacence(*this, x, y - 1, z, adjChunks);
        }

        if (y == chunkSize - 1) {
            if (adjChunks[2] != nullptr)
                _calculBlockAdjacence(*adjChunks[2].get(), x, 0, z, adjChunks[2]->adjacentChunks());
        } else {
            _calculBlockAdjacence(*this, x, y + 1, z, adjChunks);
        }

        if (z == 0) {
            if (adjChunks[5] != nullptr)
                _calculBlockAdjacence(*adjChunks[5].get(), x, y, chunkSize - 1, adjChunks[5]->adjacentChunks());
        } else {
            _calculBlockAdjacence(*this, x, y, z - 1, adjChunks);
        }

        if (z == chunkSize - 1) {
            if (adjChunks[4] != nullptr)
                _calculBlockAdjacence(*adjChunks[4].get(), x, y, 0, adjChunks[4]->adjacentChunks());
        } else {
            _calculBlockAdjacence(*this, x, y, z + 1, adjChunks);
        }

    }

    void VoxelChunk::calculBlockAdjacenceEdge(int edge) {
        int xs, xe, ys, ye, zs, ze;
        const int cs = chunkSize - 1;
        switch (edge) {
            case 0: xs = cs;        xe = chunkSize; ys = 0;         ye = chunkSize; zs = 0;         ze = chunkSize; break; // +X
            case 1: xs = 0;         xe = 1;         ys = 0;         ye = chunkSize; zs = 0;         ze = chunkSize; break; // -X
            case 2: xs = 0;         xe = chunkSize; ys = cs;        ye = chunkSize; zs = 0;         ze = chunkSize; break; // +Y
            case 3: xs = 0;         xe = chunkSize; ys = 0;         ye = 1;         zs = 0;         ze = chunkSize; break; // -Y
            case 4: xs = 0;         xe = chunkSize; ys = 0;         ye = chunkSize; zs = cs;        ze = chunkSize; break; // +Z
            case 5: xs = 0;         xe = chunkSize; ys = 0;         ye = chunkSize; zs = 0;         ze = 1;         break; // -Z
            default: xs = xe = ys = ye = zs = ze = 0; break;
        }
        std::array<std::shared_ptr<VoxelChunk>, 6> adjChunks = adjacentChunks();
        for (int x = xs; x < xe; x++)
            for (int y = ys; y < ye; y++)
                for (int z = zs; z < ze; z++) {
                    _blockIds[indexOfBlock(x, y, z)] = blockIdAt(x, y, z) | (_calculBlockAdjacence(*this, x, y, z, adjChunks) << 16);
                }
    }

    std::pair<glm::vec3, glm::vec3> VoxelChunk::getBounds(glm::mat4 transform) const {
        (void)transform;
        return std::pair<glm::vec3, glm::vec3>(0, 0);
    }

    void VoxelChunk::setMaterial(std::shared_ptr<Material> mat) {
        _material = mat;
    }

    std::shared_ptr<Material> VoxelChunk::getMaterial() const {
        return _material;
    }

}