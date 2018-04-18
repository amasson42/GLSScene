//
//  GLSTexture.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSTexture_h
#define GLSTexture_h

#include <stdlib.h>
#include <string>
#include "GLScene.hpp"

namespace GLS {
    
    class Texture {
        
        int _width;
        int _height;
        int _bpp;
        uint8_t *_data;
        
        GLuint _buffer;
        
        Texture();
        Texture(const Texture& copy);
        Texture& operator=(const Texture& copy);
        
    public:
        
        Texture(std::string filename, GLenum format);
        virtual ~Texture();
        
        void clearData();
        GLuint buffer() const;
        
    };
    
}

#endif /* GLSTexture_h */
