//
//  GLSShader.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSShader_h
#define GLSShader_h

#include "GLSStructs.hpp"

namespace GLS {
    
    class ShaderProgram;
    
    class Shader {
        
        std::string _src;
        GLuint _shader;
        GLenum _type;
        
        Shader();
        Shader(const Shader& copy);
        Shader& operator=(const Shader& copy);
        
        void compile();
        
        friend class ShaderProgram;
        
    public:
        
        Shader(std::ifstream& file, GLenum type);
        Shader(std::string src, GLenum type);
        virtual ~Shader();
        
        GLenum type() const;
        
        void clearSrcs();
        
        static std::shared_ptr<Shader> standardVertexMesh();
        static std::shared_ptr<Shader> standardVertexInstancedMesh();
        static std::shared_ptr<Shader> standardVertexSkinnedMesh();
        static std::shared_ptr<Shader> standardVertexScreenTexture();
        static std::shared_ptr<Shader> standardVertexSkybox();
        static std::shared_ptr<Shader> standardVertexParticleSystem();

        static std::shared_ptr<Shader> standardGeometryTexturedParticleSystem();

        static std::shared_ptr<Shader> standardFragmentMesh();
        static std::shared_ptr<Shader> standardFragmentMeshSimpleColor();
        static std::shared_ptr<Shader> standardFragmentScreenTexture();
        static std::shared_ptr<Shader> standardFragmentSkybox();
        static std::shared_ptr<Shader> standardFragmentDotParticleSystem();
        static std::shared_ptr<Shader> standardFragmentTexturedParticleSystem();
        
    };
    
    class ShaderProgram {
        
        GLuint _program;
        
        ShaderProgram();
        ShaderProgram(const ShaderProgram& copy);
        ShaderProgram& operator=(const ShaderProgram& copy);
        
        static std::shared_ptr<ShaderProgram> _standardShaderProgramMesh;
        static std::shared_ptr<ShaderProgram> _standardShaderProgramMeshSimpleColor;
        static std::shared_ptr<ShaderProgram> _standardShaderProgramInstancedMesh;
        static std::shared_ptr<ShaderProgram> _standardShaderProgramInstancedMeshSimpleColor;
        static std::shared_ptr<ShaderProgram> _standardShaderProgramSkinnedMesh;
        static std::shared_ptr<ShaderProgram> _standardShaderProgramSkinnedMeshSimpleColor;
        static std::shared_ptr<ShaderProgram> _standardShaderProgramScreenTexture;
        static std::shared_ptr<ShaderProgram> _standardShaderProgramSkybox;
        static std::shared_ptr<ShaderProgram> _standardShaderProgramDotParticleSystem;
        static std::shared_ptr<ShaderProgram> _standardShaderProgramTexturedParticleSystem;
        friend void glsInit(void* addr);
        friend void glsDeinit();

    public:

        ShaderProgram(const Shader& vertex, const Shader& fragment);
        ShaderProgram(const Shader& vertex, const Shader& geometry, const Shader& fragment);
        virtual ~ShaderProgram();
        
        GLuint program() const;
        GLint getLocation(std::string name) const;
        
        void use() const;
        
        static std::shared_ptr<ShaderProgram> standardShaderProgramMesh();
        static std::shared_ptr<ShaderProgram> standardShaderProgramMeshSimpleColor();
        static std::shared_ptr<ShaderProgram> standardShaderProgramSkinnedMesh();
        static std::shared_ptr<ShaderProgram> standardShaderProgramSkinnedMeshSimpleColor();
        static std::shared_ptr<ShaderProgram> standardShaderProgramInstancedMesh();
        static std::shared_ptr<ShaderProgram> standardShaderProgramInstancedMeshSimpleColor();
        static std::shared_ptr<ShaderProgram> standardShaderProgramScreenTexture();
        static std::shared_ptr<ShaderProgram> standardShaderProgramSkybox();
        static std::shared_ptr<ShaderProgram> standardShaderProgramDotParticleSystem();
        static std::shared_ptr<ShaderProgram> standardShaderProgramTexturedParticleSystem();

    };
    
}

#endif /* GLSShader_h */
