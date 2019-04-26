//
//  GLSStructs.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLScene.hpp"

std::ostream& operator<<(std::ostream& stream, glm::mat4 m) {
    for (int i = 0; i < 16; i++) {
        stream << GLS::mat4ValueAt(m, i % 4, i / 4) << ((i + 1) % 4 ? ' ' : '\n');
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, glm::mat3 m) {
    for (int i = 0; i < 9; i++) {
        stream << GLS::mat3ValueAt(m, i % 3, i / 3) << ((i + 1) % 3 ? ' ' : '\n');
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, glm::vec3 v) {
    stream << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    return stream;
}

std::ostream& operator<<(std::ostream& stream, glm::quat q) {
    stream << '(' << q.x << ',' << q.y << ',' << q.z << ',' << q.w << ')';
    return stream;
}

namespace GLS {
    
    float& mat4ValueAt(glm::mat4& m, int l, int c) {
        float *values = glm::value_ptr(m);
        return (values[4 * l + c]);
    }

    float& mat3ValueAt(glm::mat3& m, int l, int c) {
        float *values = glm::value_ptr(m);
        return (values[3 * l + c]);
    }


    Vertex::Vertex() {
        position = glm::vec3(0);
        normal = glm::vec3(0, 0, -1);
        tangent = glm::vec3(1, 0, 0);
        bitangent = glm::vec3(0, 1, 0);
        uv = glm::vec2(0);
    }

    Vertex::Vertex(const glm::vec3 p,
               const glm::vec3 n,
               const glm::vec3 t,
               const glm::vec3 b,
               const glm::vec2 u) {
        position = p;
        normal = n;
        tangent = t;
        bitangent = b;
        uv = u;
    }
    
    Vertex::Vertex(const glm::vec3& o,
           const glm::vec3& n,
           const glm::vec2& u) {
        position = o;
        normal = n;
        if (n == glm::vec3(0, 1, 0))
            tangent = glm::vec3(1, 0, 0);
        else
            tangent = glm::cross(glm::vec3(0, 1, 0), n);
        bitangent = glm::cross(n, tangent);
        uv = u;
    }

}
