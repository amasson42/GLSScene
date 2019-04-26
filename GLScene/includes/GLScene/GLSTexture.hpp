//
//  GLSTexture.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSTexture_h
#define GLSTexture_h

#include "GLScene.hpp"

namespace GLS {
    
    class Texture {
        
        int _width;
        int _height;
        int _bpp;
        uint8_t *_data;
        std::string _path;
        
        GLuint _buffer;
        
        Texture();
        Texture(const Texture& copy);
        Texture& operator=(const Texture& copy);
        
    public:
        
        class LoadingException : public std::exception {
            public:
            const char* what() const throw();
        };

        class CreationException : public std::exception {
            public:
            const char* what() const throw();
        };

        Texture(std::string path, GLenum format);
        virtual ~Texture();
        
        void clearData();
        GLuint buffer() const;
        void setParameter(GLenum pname, GLint param);
        void setParameters(std::vector<GLenum> pnames, std::vector<GLint> params);

    };
    
}

#endif /* GLSTexture_h */
