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
    
    class Shader {
        
        std::string _src;
        GLuint _shader;
        GLenum _type;
        
        Shader();
        Shader(const Shader& copy);
        Shader& operator=(const Shader& copy);
        
        void compile();
        
        friend ShaderProgram;
        
    public:
        
        Shader(std::ifstream& file, GLenum type);
        Shader(std::string src, GLenum type);
        virtual ~Shader();
        
        GLenum type() const;
        
        void clearSrcs();
        
    };
    
    class ShaderProgram {
        
        GLuint _program;
        
        ShaderProgram();
        ShaderProgram(const ShaderProgram& copy);
        ShaderProgram& operator=(const ShaderProgram& copy);
        
    public:
        
        ShaderProgram(const Shader& vertex, const Shader& fragment);
        virtual ~ShaderProgram();
        
        GLuint program() const;
        GLint getLocation(std::string name) const;
        
        void use() const;
        
    };
    
}

#endif /* GLSShader_h */
