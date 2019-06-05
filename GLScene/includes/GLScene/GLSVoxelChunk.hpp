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
        
        int *blockIds();
        const int& blockAt(int x, int y, int z) const;
        int& blockAt(int x, int y, int z);

        int blockIdAt(int x, int y, int z) const;
        
        void calculBlockAdjacence(const std::array<std::shared_ptr<VoxelChunk>, 6>& adjChunks);

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

    };

}

#endif /* GLSVoxelChunk_h */
