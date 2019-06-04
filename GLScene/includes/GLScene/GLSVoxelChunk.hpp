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

        int _blockIds[chunkBlockCount];

        GLuint _blocksBuffer;
        GLuint _blocksArray;

        std::shared_ptr<ShaderProgram> _shaderProgram;

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

        virtual std::pair<glm::vec3, glm::vec3> getBounds(glm::mat4 transform = glm::mat4(1)) const;

        // OpenGL Buffers
        
        void generateBuffers() throw(BufferCreationException);
        void deleteBuffers();
        
        bool bufferGenerated() const;
        
        void resetIdsBufferValues();
        
        // Rendering
        
        virtual void renderInContext(Scene& scene, const RenderUniforms& uniforms);
        virtual void postRenderInContext(Scene& scene, const RenderUniforms& uniforms, float priority);
        virtual void renderInDepthContext(Scene& scene, const RenderUniforms& uniforms);

    };

}

#endif /* GLSVoxelChunk_h */
