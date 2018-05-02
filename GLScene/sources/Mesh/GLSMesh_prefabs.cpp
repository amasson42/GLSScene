//
//  GLSMesh.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {
    
    // TODO: prefabs
    std::shared_ptr<Mesh> Mesh::plane(GLfloat width, GLfloat height, bool generateBuffers) {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        width /= 2.0f;
        height /= 2.0f;
        mesh->verticesRef().push_back(Vertex(glm::vec3(-width, -height, 0),
                                             glm::vec3(0, 0, 1),
                                             glm::vec4(1),
                                             glm::vec2(0, 0)));
        mesh->verticesRef().push_back(Vertex(glm::vec3(-width, height, 0),
                                             glm::vec3(0, 0, 1),
                                             glm::vec4(1),
                                             glm::vec2(0, 1)));
        mesh->verticesRef().push_back(Vertex(glm::vec3(width, -height, 0),
                                             glm::vec3(0, 0, 1),
                                             glm::vec4(1),
                                             glm::vec2(1, 0)));
        mesh->verticesRef().push_back(Vertex(glm::vec3(width, height, 0),
                                             glm::vec3(0, 0, 1),
                                             glm::vec4(1),
                                             glm::vec2(1, 1)));
        const GLuint indices[] = {0, 2, 3, 0, 3, 1};
        mesh->indicesRef() = std::vector<GLuint>(indices, indices + sizeof(indices) / sizeof(*indices));
        if (generateBuffers)
            mesh->generateBuffers();
        return mesh;
    }
    
//    Mesh Mesh::cube(GLfloat width, GLfloat height, GLfloat length);
//    Mesh Mesh::sphere(GLfloat radius, unsigned int ringCount = 12);
//    Mesh Mesh::objModel(const char *filename, int options);
//    Mesh Mesh::objModel(std::istream& file, int options);
    
}
