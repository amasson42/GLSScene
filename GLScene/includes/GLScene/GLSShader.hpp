//
//  GLSShader.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSShader_h
#define GLSShader_h

#include <string>
#include "GLScene.hpp"

// TODO: throw errors when shaders aren't created

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
        
        Shader(std::ifstream& file, GLenum type);
        Shader(std::string src, GLenum type);
        virtual ~Shader();
        
        GLenum type() const;
        
        void clearSrcs();
        
        static std::shared_ptr<Shader> standardVertex();
        static std::shared_ptr<Shader> standardFragment();
        
    };
    
    class ShaderProgram {
        
        GLuint _program;
        
        ShaderProgram();
        ShaderProgram(const ShaderProgram& copy);
        ShaderProgram& operator=(const ShaderProgram& copy);
        
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
        
    };
    
}

#endif /* GLSShader_h */
