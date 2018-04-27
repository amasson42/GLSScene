//
//  GLSMath.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLScene.hpp"
#include <cmath>

namespace GLS {
    
    // Vector
    
    Vector::Vector(GLfloat vx, GLfloat vy, GLfloat vz) :
    x(vx), y(vy), z(vz)
    {
        
    }
    
    GLfloat Vector::norme() const {
        return sqrt(sqrNorme());
    }
    
    GLfloat Vector::sqrNorme() const {
        return x * x + y * y + z * z;
    }
    
    Vector Vector::normalized() const {
        return *this * (1.0f / norme());
    }
    
    Vector Vector::vectorialProduct(Vector a, Vector b) {
        return Vector(a.y * b.z - a.z * b.y,
                      a.z * b.x - a.x * b.z,
                      a.x * b.y - a.y * b.x);
    }
    
    GLfloat Vector::scalarProduct(Vector a, Vector b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    
    Vector operator+(Vector lhs, Vector rhs) {
        return Vector(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }
    
    Vector operator-(Vector lhs, Vector rhs) {
        return Vector(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }
    
    Vector operator*(Vector v, GLfloat t) {
        return Vector(v.x * t, v.y * t, v.z * t);
    }
    
    Vector operator*(GLfloat t, Vector v) {
        return operator*(v, t);
    }
    
    
    // Matrix4x4
    
    Matrix4x4::Matrix4x4() {
        for (int i = 0; i < 16; i++)
            m[i] = (i / 4) == (i % 4) ? 1.0f : 0.0f;
    }
    
    Matrix4x4::Matrix4x4(GLfloat mvs[16]) {
        for (int i = 0; i < 16; i++)
            m[i] = mvs[i];
    }
    
    GLfloat& Matrix4x4::operator()(int l, int c) {
        return m[4 * c + l];
    }
    
    const GLfloat& Matrix4x4::operator()(int l, int c) const {
        return m[4 * c + l];
    }
    
    static GLfloat matrixProductAt(const Matrix4x4& a, const Matrix4x4& b, int l, int c) {
        GLfloat r = 0;
        for (int i = 0; i < 4; i++)
            r += a(l, i) * b(i, c);
        return r;
    }
    
    Matrix4x4 Matrix4x4::transform(Matrix4x4 mat) const {
        Matrix4x4 r;
        for (int i = 0; i < 16; i++)
            r.m[i] = matrixProductAt(*this, mat, i % 4, i / 4);
        return r;
    }
    
    Vector Matrix4x4::transform(Vector v, bool useOffset) const {
        return Vector(m[0] * v.x + m[4] * v.y + m[8] * v.z + (useOffset ? m[12] : 0),
                      m[1] * v.x + m[5] * v.y + m[9] * v.z + (useOffset ? m[13] : 0),
                      m[2] * v.x + m[6] * v.y + m[10] * v.z + (useOffset ? m[14] : 0));
    }
    
    /** !!! Not available yet !!! */
    GLfloat Matrix4x4::determinant() const {
        return 0;
    }
    
    /** !!! Not available yet !!! */
    Matrix4x4 Matrix4x4::inverse() const {
        return identity();
    }
    
    /** !!! Not available yet !!! */
    Matrix4x4 Matrix4x4::transpose() const {
        return identity();
    }
    
    Matrix4x4 Matrix4x4::zero() {
        Matrix4x4 r;
        for (int i = 0; i < 16; i++)
            r.m[i] = 0;
        return r;
    }
    
    Matrix4x4 Matrix4x4::identity() {
        return Matrix4x4();
    }
    
    Matrix4x4 Matrix4x4::translation(Vector translation) {
        Matrix4x4 r;
        r(0, 3) = translation.x;
        r(1, 3) = translation.y;
        r(2, 3) = translation.z;
        return r;
    }
    
    Matrix4x4 Matrix4x4::translation(GLfloat x, GLfloat y, GLfloat z) {
        Matrix4x4 r;
        r(0, 3) = x;
        r(1, 3) = y;
        r(2, 3) = z;
        return r;
    }
    
    Matrix4x4 Matrix4x4::scale(Vector scale) {
        Matrix4x4 r;
        r(0, 0) = scale.x;
        r(1, 1) = scale.y;
        r(2, 2) = scale.z;
        return r;
    }
    
    Matrix4x4 Matrix4x4::scale(GLfloat x, GLfloat y, GLfloat z) {
        Matrix4x4 r;
        r(0, 0) = x;
        r(1, 1) = y;
        r(2, 2) = z;
        return r;
    }
    
    Matrix4x4 Matrix4x4::rotx(GLfloat alpha) {
        GLfloat c = cos(alpha);
        GLfloat s = sin(alpha);
        return Matrix4x4((GLfloat[]) {
            1, 0, 0, 0,
            0, c, s, 0,
            0, -s, c, 0,
            0, 0, 0, 1
        });
    }
    
    Matrix4x4 Matrix4x4::roty(GLfloat alpha) {
        GLfloat c = cos(alpha);
        GLfloat s = sin(alpha);
        return Matrix4x4((GLfloat[]) {
            c, 0, -s, 0,
            0, 1, 0, 0,
            s, 0, c, 0,
            0, 0, 0, 1
        });
    }
    
    Matrix4x4 Matrix4x4::rotz(GLfloat alpha) {
        GLfloat c = cos(alpha);
        GLfloat s = sin(alpha);
        return Matrix4x4((GLfloat[]) {
            c, s, 0, 0,
            -s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        });
    }
    
    Matrix4x4 Matrix4x4::rotaxe(Vector axe, GLfloat alpha) {
        float c = cos(alpha);
        float s = sin(alpha);
        float l = 1.0f - c;
        Vector v = axe.normalized();
        return Matrix4x4((GLfloat[]) {
            v.x * v.x * l + c, v.x * v.y * l + v.z * s, v.x * v.z * l - v.y * s, 0,
            v.x * v.y * l - v.z * s, v.y * v.y * l + c, v.y * v.z * l + v.x * s, 0,
            v.x * v.z * l + v.y * s, v.y * v.z * l - v.x * s, v.z * v.z * l + c, 0,
            0, 0, 0, 1
        });
    }
    
    Matrix4x4 Matrix4x4::projection(GLfloat fov, GLfloat aspect,
                                    GLfloat near, GLfloat far) {
        float y = 1 / tan(fov * 0.5f);
        float x = y / aspect;
        float z = far / (near - far);
        return Matrix4x4((GLfloat[]) {
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, -1,
            0, 0, z * near, 0
        });
    }
    
    Matrix4x4 operator*(const Matrix4x4& lhs, const Matrix4x4& rhs) {
        return lhs.transform(rhs);
    }
    
}
