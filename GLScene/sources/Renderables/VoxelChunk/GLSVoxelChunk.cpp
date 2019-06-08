//
//  GLSVoxelChunk.cpp
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

    const char *VoxelChunk::BufferCreationException::what() const throw() {
        return "could not create additional buffer";
    }

    VoxelChunk::VoxelChunk() :
    _blocksBuffer(0), _blocksArray(0),
    _shaderProgram(nullptr),
    _material(nullptr),
    _adjChunks(),
    _isEmpty(true)
    {
        for (int i = 0; i < chunkBlockCount; i++)
            _blockIds[i] = 0;
        for (int i = 0; i < 6; i++)
            _fullEdges[i] = false;
    }

    VoxelChunk::VoxelChunk(const VoxelChunk& copy) :
    _blocksBuffer(0), _blocksArray(0),
    _shaderProgram(nullptr),
    _material(copy._material),
    _adjChunks(),
    _isEmpty(copy._isEmpty)
    {
        for (int i = 0; i < chunkBlockCount; i++)
            _blockIds[i] = copy._blockIds[i];
        if (copy.bufferGenerated())
            generateBuffers();
        for (int i = 0; i < 6; i++)
            _fullEdges[i] = copy._fullEdges[i];
    }

    VoxelChunk::~VoxelChunk() {
        deleteBuffers();
    }

    VoxelChunk& VoxelChunk::operator=(const VoxelChunk& copy) {
        for (int i = 0; i < chunkBlockCount; i++)
            _blockIds[i] = copy._blockIds[i];
        deleteBuffers();
        _material = copy._material;
        _isEmpty = copy._isEmpty;
        for (int i = 0; i < 6; i++)
            _fullEdges[i] = copy._fullEdges[i];
        if (copy.bufferGenerated())
            generateBuffers();
        return (*this);
    }


    // VoxelChunk utilities

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

    void VoxelChunk::setBlockIdAt(int x, int y, int z, int id) {
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

    static int _opposedFace(int f) {
        return ((f / 2) * 2) + (1 - (f % 2));
    }

    bool VoxelChunk::isSurrounded() const {
        for (int i = 0; i < 6; i++) {
            if (_adjChunks[i].expired())
                return false;
            std::shared_ptr<VoxelChunk> adj = _adjChunks[i].lock();
            if (adj->isFullOnEdge(_opposedFace(i)) == false)
                return false;
        }
        return true;
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

    std::pair<glm::vec3, glm::vec3> VoxelChunk::getBounds(glm::mat4 transform) const {
        (void)transform;
        return std::pair<glm::vec3, glm::vec3>(0, 0);
    }

    void VoxelChunk::generateBuffers() throw(BufferCreationException) {
        deleteBuffers();

        glGenVertexArrays(1, &_blocksArray);
        if (_blocksArray == 0)
            throw BufferCreationException();
        glBindVertexArray(_blocksArray);

        glGenBuffers(1, &_blocksBuffer);
        if (_blocksBuffer == 0) {
            glDeleteVertexArrays(1, &_blocksArray);
            throw BufferCreationException();
        }
        glBindBuffer(GL_ARRAY_BUFFER, _blocksBuffer);

        glVertexAttribPointer(0, 1, GL_INT, GL_FALSE, sizeof(int), NULL);
        glEnableVertexAttribArray(0);

        updateIdsBuffer();
    }

    void VoxelChunk::deleteBuffers() {
        if (_blocksBuffer != 0)
            glDeleteBuffers(1, &_blocksBuffer);
        _blocksBuffer = 0;
        if (_blocksArray != 0)
            glDeleteVertexArrays(1, &_blocksArray);
        _blocksArray = 0;
    }

    bool VoxelChunk::bufferGenerated() const {
        return _blocksBuffer != 0 && _blocksArray != 0;
    }

    void VoxelChunk::updateIdsBuffer() {
        if (bufferGenerated()) {
            glBindVertexArray(_blocksArray);
            glBindBuffer(GL_ARRAY_BUFFER, _blocksBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(int) * chunkBlockCount, _blockIds, GL_DYNAMIC_DRAW);
        }
    }

    void VoxelChunk::setMaterial(std::shared_ptr<Material> mat) {
        _material = mat;
    }

    std::shared_ptr<Material> VoxelChunk::getMaterial() const {
        return _material;
    }

    void VoxelChunk::setProgram(std::shared_ptr<ShaderProgram> shaderProgram) {
        _shaderProgram = shaderProgram;
    }

    void VoxelChunk::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
        if (!bufferGenerated())
            generateBuffers();
        if (!bufferGenerated())
            return;
        
        std::shared_ptr<ShaderProgram> program;
        if (_shaderProgram) {
            program = _shaderProgram;
            program->use();
            scene.sendLightsValueToShader(program);
        } else {
            program = ShaderProgram::standardShaderProgramVoxelChunk();
            program->use();
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        uniforms.sendUniformsToShaderProgram(program);

        glStencilMask(0x00);

        if (_material == nullptr)
            Material().sendUniformToShaderProgram(program);
        else
            _material->sendUniformToShaderProgram(program);

        glBindVertexArray(_blocksArray);
        glDrawArrays(GL_POINTS, 0, chunkBlockCount);
    }

    void VoxelChunk::renderInDepthContext(Scene& scene, const RenderUniforms& uniforms) {
        if (!bufferGenerated())
            generateBuffers();
        if (!bufferGenerated())
            return;
        
        std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramVoxelChunkSimpleColor();
        program->use();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
        glUniformMatrix4fv(program->getLocation("u_mat_view"), 1, GL_FALSE, glm::value_ptr(uniforms.view));
        glUniformMatrix4fv(program->getLocation("u_mat_model"), 1, GL_FALSE, glm::value_ptr(uniforms.model));

        glBindVertexArray(_blocksArray);
        glDrawArrays(GL_POINTS, 0, chunkBlockCount);
        (void)scene;
    }


    // Shader Uniforms

    std::string VoxelChunk::shaderUniformsVertex() {
        return
        "layout (location = 0) in int blockId;\n"
        "\n"
        "const int chunkSize = " + std::to_string(chunkSize) + ";\n"
        "\n"
        "out GS_IN {\n"
        "    int position_x;\n"
        "    int position_y;\n"
        "    int position_z;\n"
        "    flat int blockAdj;\n"
        "    flat int blockId;\n"
        "} vs_out;\n"
        "\n";
    }

    std::string VoxelChunk::shaderUniformsGeometry() {
        return
        "uniform mat3 u_mat_normal;\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "\n"
        "in GS_IN {\n"
        "    int position_x;\n"
        "    int position_y;\n"
        "    int position_z;\n"
        "    flat int blockAdj;\n"
        "    flat int blockId;\n"
        "} gs_in[];\n"
        "\n"
        "out VS_OUT {\n"
        "    vec4 position;\n"
        "    vec3 wposition;\n"
        "    vec2 uv;\n"
        "    vec3 wnormal;\n"
        "    vec3 wtangent;\n"
        "    vec3 wbitangent;\n"
        "} gs_out;\n"
        "\n";
    }

    std::string VoxelChunk::shaderUniformsFragment() {
        return Mesh::shaderUniformsFragment();
    }


    // Turn into mesh

    static glm::vec2 _getBlockIdUvs(glm::vec2 uv, int blockId, int face) {
        blockId--;
        int gridX = (blockId % 10);
        int gridY = (blockId / 10);
        uv.x = ((gridX * 3) + face + uv.x) * (1.0 / 30.0);
        uv.y = (gridY + (1 - uv.y)) * (1.0 / 10.0);
        return uv;
    }

    static void _indices_appendFace(std::vector<GLuint>& indices, size_t verticesSize) {
        indices.push_back(verticesSize - 4);
        indices.push_back(verticesSize - 3);
        indices.push_back(verticesSize - 2);
        indices.push_back(verticesSize - 3);
        indices.push_back(verticesSize - 1);
        indices.push_back(verticesSize - 2);
    }

    static void _drawFace_positiveX_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(1, 0, 0) + glm::vec3(0, 1, 0) * uv.y + glm::vec3(0, 0, 1) * (1 - uv.x);
        v.normal = glm::vec3(1, 0, 0);
        v.tangent = glm::vec3(0, 0, -1);
        v.bitangent = glm::vec3(0, 1, 0);
        v.uv = _getBlockIdUvs(uv, blockId, 1);
        vertices.push_back(v);
    }

    static void _drawFace_positiveX(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int blockId, glm::vec3 coords) {
        _drawFace_positiveX_emitVertex(vertices, glm::vec2(0, 0), blockId, coords);
        _drawFace_positiveX_emitVertex(vertices, glm::vec2(1, 0), blockId, coords);
        _drawFace_positiveX_emitVertex(vertices, glm::vec2(0, 1), blockId, coords);
        _drawFace_positiveX_emitVertex(vertices, glm::vec2(1, 1), blockId, coords);
        _indices_appendFace(indices, vertices.size());
    }

    static void _drawFace_negativeX_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(0, 1, 0) * uv.y + glm::vec3(0, 0, 1) * uv.x;
        v.normal = glm::vec3(-1, 0, 0);
        v.tangent = glm::vec3(0, 0, 1);
        v.bitangent = glm::vec3(0, 1, 0);
        v.uv = _getBlockIdUvs(uv, blockId, 1);
        vertices.push_back(v);
    }

    static void _drawFace_negativeX(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int blockId, glm::vec3 coords) {
        _drawFace_negativeX_emitVertex(vertices, glm::vec2(0, 0), blockId, coords);
        _drawFace_negativeX_emitVertex(vertices, glm::vec2(1, 0), blockId, coords);
        _drawFace_negativeX_emitVertex(vertices, glm::vec2(0, 1), blockId, coords);
        _drawFace_negativeX_emitVertex(vertices, glm::vec2(1, 1), blockId, coords);
        _indices_appendFace(indices, vertices.size());
    }

    static void _drawFace_positiveY_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(1, 0, 0) * uv.x + glm::vec3(0, 1, 0) + glm::vec3(0, 0, 1) * (1 - uv.y);
        v.normal = glm::vec3(0, 1, 0);
        v.tangent = glm::vec3(1, 0, 0);
        v.bitangent = glm::vec3(0, 0, -1);
        v.uv = _getBlockIdUvs(uv, blockId, 2);
        vertices.push_back(v);
    }

    static void _drawFace_positiveY(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int blockId, glm::vec3 coords) {
        _drawFace_positiveY_emitVertex(vertices, glm::vec2(0, 0), blockId, coords);
        _drawFace_positiveY_emitVertex(vertices, glm::vec2(1, 0), blockId, coords);
        _drawFace_positiveY_emitVertex(vertices, glm::vec2(0, 1), blockId, coords);
        _drawFace_positiveY_emitVertex(vertices, glm::vec2(1, 1), blockId, coords);
        _indices_appendFace(indices, vertices.size());
    }

    static void _drawFace_negativeY_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(1, 0, 0) * (1 - uv.x) + glm::vec3(0, 0, 1) * (1 - uv.y);
        v.normal = glm::vec3(0, -1, 0);
        v.tangent = glm::vec3(-1, 0, 0);
        v.bitangent = glm::vec3(0, 0, -1);
        v.uv = _getBlockIdUvs(uv, blockId, 0);
        vertices.push_back(v);
    }

    static void _drawFace_negativeY(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int blockId, glm::vec3 coords) {
        _drawFace_negativeY_emitVertex(vertices, glm::vec2(0, 0), blockId, coords);
        _drawFace_negativeY_emitVertex(vertices, glm::vec2(1, 0), blockId, coords);
        _drawFace_negativeY_emitVertex(vertices, glm::vec2(0, 1), blockId, coords);
        _drawFace_negativeY_emitVertex(vertices, glm::vec2(1, 1), blockId, coords);
        _indices_appendFace(indices, vertices.size());
    }

    static void _drawFace_positiveZ_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(1, 0, 0) * uv.x + glm::vec3(0, 1, 0) * uv.y + glm::vec3(0, 0, 1);
        v.normal = glm::vec3(0, 0, 1);
        v.tangent = glm::vec3(1, 0, 0);
        v.bitangent = glm::vec3(0, 1, 0);
        v.uv = _getBlockIdUvs(uv, blockId, 1);
        vertices.push_back(v);
    }

    static void _drawFace_positiveZ(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int blockId, glm::vec3 coords) {
        _drawFace_positiveZ_emitVertex(vertices, glm::vec2(0, 0), blockId, coords);
        _drawFace_positiveZ_emitVertex(vertices, glm::vec2(1, 0), blockId, coords);
        _drawFace_positiveZ_emitVertex(vertices, glm::vec2(0, 1), blockId, coords);
        _drawFace_positiveZ_emitVertex(vertices, glm::vec2(1, 1), blockId, coords);
        _indices_appendFace(indices, vertices.size());
    }

    static void _drawFace_negativeZ_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(1, 0, 0) * (1 - uv.x) + glm::vec3(0, 1, 0) * uv.y;
        v.normal = glm::vec3(0, 0, -1);
        v.tangent = glm::vec3(-1, 0, 0);
        v.bitangent = glm::vec3(0, 1, 0);
        v.uv = _getBlockIdUvs(uv, blockId, 1);
        vertices.push_back(v);
    }

    static void _drawFace_negativeZ(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int blockId, glm::vec3 coords) {
        _drawFace_negativeZ_emitVertex(vertices, glm::vec2(0, 0), blockId, coords);
        _drawFace_negativeZ_emitVertex(vertices, glm::vec2(1, 0), blockId, coords);
        _drawFace_negativeZ_emitVertex(vertices, glm::vec2(0, 1), blockId, coords);
        _drawFace_negativeZ_emitVertex(vertices, glm::vec2(1, 1), blockId, coords);
        _indices_appendFace(indices, vertices.size());
    }
    

    std::shared_ptr<Mesh> Mesh::voxelChunk(std::shared_ptr<VoxelChunk> chunk, bool generateBuffers) {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

        std::vector<Vertex>& vertices(mesh->verticesRef());
        std::vector<GLuint>& indices(mesh->indicesRef());

        for (int x = 0; x < VoxelChunk::chunkSize; x++) {
            for (int y = 0; y < VoxelChunk::chunkSize; y++) {
                for (int z = 0; z < VoxelChunk::chunkSize; z++) {
                    int blockId = chunk->blockIdAt(x, y, z);
                    if (blockId == 0)
                        continue;
                    int blockAdj = chunk->blockAdjAt(x, y, z);
                    glm::vec3 coords(x, y, z);
                    if ((blockAdj & (1 << 0)) != 0)
                        _drawFace_positiveX(vertices, indices, blockId, coords);
                    if ((blockAdj & (1 << 1)) != 0)
                        _drawFace_negativeX(vertices, indices, blockId, coords);
                    if ((blockAdj & (1 << 2)) != 0)
                        _drawFace_positiveY(vertices, indices, blockId, coords);
                    if ((blockAdj & (1 << 3)) != 0)
                        _drawFace_negativeY(vertices, indices, blockId, coords);
                    if ((blockAdj & (1 << 4)) != 0)
                        _drawFace_positiveZ(vertices, indices, blockId, coords);
                    if ((blockAdj & (1 << 5)) != 0)
                        _drawFace_negativeZ(vertices, indices, blockId, coords);
                }
            }
        }
        mesh->setMaterial(chunk->getMaterial());
        if (generateBuffers)
            mesh->generateBuffers();
        return mesh;
    }

}
