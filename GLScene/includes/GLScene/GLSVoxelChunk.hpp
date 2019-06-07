//
//  GLSVoxelChunk.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSVoxelChunk_h
#define GLSVoxelChunk_h

#include "GLScene.hpp"

namespace GLS {
    
    class VoxelChunk : public IRenderable {
    
    public:
        static const int chunkSize = 16;
        static const int chunkBlockCount = chunkSize * chunkSize * chunkSize;
        static int indexOfBlock(int x, int y, int z);
        static std::tuple<int, int, int> coordinatesOfBlock(int i);

    protected:

        // blockId & 0x00FFFF = id;
        // blockId & 0xFF0000 = adjacence
        int _blockIds[chunkBlockCount];

        GLuint _blocksBuffer;
        GLuint _blocksArray;

        std::shared_ptr<ShaderProgram> _shaderProgram;
        std::shared_ptr<Material> _material;

        std::array<std::weak_ptr<VoxelChunk>, 6> _adjChunks;

        bool _isEmpty;
        bool _fullEdges[6];

    public:

        class BufferCreationException : public std::exception {
            public:
            const char* what() const throw();
        };

        VoxelChunk();
        VoxelChunk(const VoxelChunk& copy);
        virtual ~VoxelChunk();
        
        VoxelChunk& operator=(const VoxelChunk& copy);
        
        
        // VoxelChunk utilities

        int blockIdAt(int x, int y, int z) const;
        int blockIdAt(std::tuple<int, int, int> coord) const;
        void setBlockIdAt(int x, int y, int z, int id);

        int blockAdjAt(int x, int y, int z) const;

        bool isEmpty() const; // only contains air blocks
        bool isFullOnEdge(int edgeIndex) const; // contains full blocks on the indexed edge
        bool isSurrounded() const; // all adjacent are full on the edge

        void setAdjacentChunks(std::array<std::weak_ptr<VoxelChunk>, 6> adjChunks);
        std::weak_ptr<VoxelChunk> adjacentChunk(int edgeIndex);

        // TODO: allow the update of a small part of the chunk and add the possibility to update the target part of the buffer with glBufferSubData
        void calculBlockAdjacence();

        virtual std::pair<glm::vec3, glm::vec3> getBounds(glm::mat4 transform = glm::mat4(1)) const;

        void setMaterial(std::shared_ptr<Material> mat);
        std::shared_ptr<Material> getMaterial() const;


        // OpenGL Buffers
        
        void generateBuffers() throw(BufferCreationException);
        void deleteBuffers();
        
        bool bufferGenerated() const;
        
        void updateIdsBuffer();


        // Rendering
        
        void setProgram(std::shared_ptr<ShaderProgram> shaderProgram);

        virtual void renderInContext(Scene& scene, const RenderUniforms& uniforms);
        virtual void renderInDepthContext(Scene& scene, const RenderUniforms& uniforms);


        // Shader Uniforms

        static std::string shaderUniformsVertex();
        static std::string shaderUniformsGeometry();
        static std::string shaderUniformsFragment();

    };

}

#endif /* GLSVoxelChunk_h */
