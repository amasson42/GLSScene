//
//  GLSShader.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSShader_h
#define GLSShader_h

#include "GLScene.hpp"

namespace GLS {
    
    class ShaderProgram;
    
    class Shader {
        
        std::string _src;
        GLuint _shader;
        GLenum _type;
        
        Shader();
        Shader(const Shader& copy);
        Shader& operator=(const Shader& copy);
        
        void compile() throw(CreationException, CompilationException);
        
        friend class ShaderProgram;
        
    public:
        
        class CreationException : public std::exception {
        public:
            const char *what() const throw();
        };
        
        class CompilationException : public std::exception {
            friend class Shader;
            char _infoLogBuffer[1024];
        public:
            const char *what() const throw();
            const char *infoLog() const;
        };
        
        Shader(std::ifstream& file, GLenum type) throw(CreationException, CompilationException);
        Shader(std::string src, GLenum type) throw(CreationException, CompilationException);
        virtual ~Shader();
        
        GLenum type() const;
        
        void clearSrcs();
        
        static std::shared_ptr<Shader> standardVertexMesh();
        static std::shared_ptr<Shader> standardFragmentMesh();
        
    };
    
    class ShaderProgram {
        
        GLuint _program;
        
        ShaderProgram();
        ShaderProgram(const ShaderProgram& copy);
        ShaderProgram& operator=(const ShaderProgram& copy);
        
        static std::shared_ptr<ShaderProgram> _standardShaderProgram;

    public:
        
        class CreationException : public std::exception {
        public:
            const char *what() const throw();
        };
        
        class LinkException : public std::exception {
            friend class ShaderProgram;
            char _infoLogBuffer[1024];
        public:
            const char *what() const throw();
            const char *infoLog() const;
        };
        
        ShaderProgram(const Shader& vertex, const Shader& fragment);
        virtual ~ShaderProgram();
        
        GLuint program() const;
        GLint getLocation(std::string name) const;
        
        void use() const;
        
        static std::shared_ptr<ShaderProgram> standardShaderProgramMesh();

    };
    
}

#endif /* GLSShader_h */
