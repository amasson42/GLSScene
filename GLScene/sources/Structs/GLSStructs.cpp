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
        stream << GLS::mat4ValueAt(m, i / 4, i % 4) << ((i + 1) % 4 ? ' ' : '\n');
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, glm::mat3 m) {
    for (int i = 0; i < 9; i++) {
        stream << GLS::mat3ValueAt(m, i / 3, i % 3) << ((i + 1) % 3 ? ' ' : '\n');
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, glm::vec3 v) {
    stream << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    return stream;
}

std::ostream& operator<<(std::ostream& stream, glm::vec2 v) {
    stream << '(' << v.x << ',' << v.y << ')';
    return stream;
}

std::ostream& operator<<(std::ostream& stream, glm::quat q) {
    stream << '(' << q.x << ',' << q.y << ',' << q.z << ',' << q.w << ')';
    return stream;
}

namespace GLS {
    
    float& mat4ValueAt(glm::mat4& m, int l, int c) {
        return glm::value_ptr(m)[4 * c + l];
    }

    float& mat3ValueAt(glm::mat3& m, int l, int c) {
        return glm::value_ptr(m)[3 * c + l];
    }

}
