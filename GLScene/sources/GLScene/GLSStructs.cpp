//
//  GLSStructs.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLScene.hpp"

namespace GLS {
    
    Color::Color() :
    r(1), g(1), b(1), a(1)
    {
        
    }
    
    Color::Color(GLfloat cr, GLfloat cg, GLfloat cb, GLfloat ca) :
    r(cr), g(cg), b(cb), a(ca)
    {
        
    }
    
    Vertex::Vertex(Vector o, Vector n, Color c, Vector u) {
        setPosition(o);
        setNormal(n);
        setColor(c);
        texture[0] = u.x;
        texture[1] = u.y;
    }
    
    Vector Vertex::getPosition() const {
        return Vector(position[0], position[1], position[2]);
    }
    
    void Vertex::setPosition(Vector newPosition) {
        position[0] = newPosition.x;
        position[1] = newPosition.y;
        position[2] = newPosition.z;
    }
    
    Vector Vertex::getNormal() const {
        return Vector(normal[0], normal[1], normal[2]);
    }
    
    void Vertex::setNormal(Vector newNormal) {
        normal[0] = newNormal.x;
        normal[1] = newNormal.y;
        normal[2] = newNormal.z;
    }
    
    Color Vertex::getColor() const {
        return Color(rgb[0], rgb[1], rgb[2]);
    }
    
    void Vertex::setColor(Color color) {
        rgb[0] = color.r;
        rgb[1] = color.g;
        rgb[2] = color.b;
    }
    
}
