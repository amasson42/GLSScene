//
//  GLSStructs.h
//  GLScene
//
//  Created by Arthur Masson on 16/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSStructs_h
#define GLSStructs_h

namespace GLS {
    
    class Scene;
    class Node;
    class Camera;
    class Mesh;
    class Texture;
    class Shader;
    class ShaderProgram;
    
    struct Vector {
        GLfloat x;
        GLfloat y;
        GLfloat z;
        
        Vector(GLfloat vx = 0, GLfloat vy = 0, GLfloat vz = 0);
        
        GLfloat norme() const;
        GLfloat sqrNorme() const;
        Vector normalized() const;
        
        static Vector vectorialProduct(Vector a, Vector b);
        static GLfloat scalarProduct(Vector a, Vector b);
    };
    
    Vector operator+(Vector lhs, Vector rhs);
    Vector operator-(Vector lhs, Vector rhs);
    Vector operator*(Vector v, GLfloat t);
    Vector operator*(GLfloat t, Vector v);
    
    struct Matrix4x4 {
        GLfloat m[16];
        
        Matrix4x4();
        Matrix4x4(GLfloat mvs[16]);
        
        GLfloat& operator()(int l, int c);
        const GLfloat& operator()(int l, int c) const;
        
        Matrix4x4 transform(Matrix4x4 mat) const;
        Vector transform(Vector v, bool useOffset = true) const;
        
        GLfloat determinant() const;
        Matrix4x4 inverse() const;
        Matrix4x4 transpose() const;
        
        static Matrix4x4 zero();
        static Matrix4x4 identity();
        static Matrix4x4 translation(Vector translation);
        static Matrix4x4 translation(GLfloat x, GLfloat y, GLfloat z);
        static Matrix4x4 scale(Vector scale);
        static Matrix4x4 scale(GLfloat x, GLfloat y, GLfloat z);
        static Matrix4x4 rotx(GLfloat alpha);
        static Matrix4x4 roty(GLfloat alpha);
        static Matrix4x4 rotz(GLfloat alpha);
        static Matrix4x4 rotaxe(Vector axe, GLfloat alpha);
        static Matrix4x4 projection(GLfloat fov, GLfloat aspect,
                                    GLfloat near, GLfloat far);
    };
    
    Matrix4x4 operator*(const Matrix4x4& lhs, const Matrix4x4& rhs);
    
    struct Color {
        GLfloat r;
        GLfloat g;
        GLfloat b;
        GLfloat a;
        
        Color();
        Color(GLfloat cr, GLfloat cg, GLfloat cb, GLfloat ca = 1);
    };
    
    struct Vertex {
        GLfloat position[3];
        GLfloat normal[3];
        GLfloat rgb[3];
        GLfloat texture[2];
        
        Vertex(Vector o, Vector n, Color c);
        
        Vector getPosition() const;
        void setPosition(Vector newPosition);
        
        Vector getNormal() const;
        void setNormal(Vector newNormal);
        
        Color getColor() const;
        void setColor(Color color);
    };
    
}

#endif /* GLSStructs_h */
