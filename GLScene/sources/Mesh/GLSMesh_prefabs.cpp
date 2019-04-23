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
                                             glm::vec2(0, 0)));
        mesh->verticesRef().push_back(Vertex(glm::vec3(-width, height, 0),
                                             glm::vec3(0, 0, 1),
                                             glm::vec2(0, 1)));
        mesh->verticesRef().push_back(Vertex(glm::vec3(width, -height, 0),
                                             glm::vec3(0, 0, 1),
                                             glm::vec2(1, 0)));
        mesh->verticesRef().push_back(Vertex(glm::vec3(width, height, 0),
                                             glm::vec3(0, 0, 1),
                                             glm::vec2(1, 1)));
        const GLuint indices[] = {0, 2, 3, 0, 3, 1};
        mesh->indicesRef() = std::vector<GLuint>(indices, indices + sizeof(indices) / sizeof(*indices));
        if (generateBuffers)
            mesh->generateBuffers();
        return mesh;
    }
    
    std::shared_ptr<Mesh> Mesh::cube(GLfloat w, GLfloat h, GLfloat l, bool generateBuffers) {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        std::vector<Vertex>& vertices(mesh->verticesRef());
        std::vector<GLuint>& indices(mesh->indicesRef());
        w /= 2.0;
        h /= 2.0;
        l /= 2.0;
        vertices.push_back(Vertex(glm::vec3(-w, -h, -l), glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(0, 0)));
        vertices.push_back(Vertex(glm::vec3(-w, -h, l), glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec2(1, 1)));
        vertices.push_back(Vertex(glm::vec3(-w, h, -l), glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec2(0, 1)));
        vertices.push_back(Vertex(glm::vec3(-w, h, l), glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec2(1, 0)));
        vertices.push_back(Vertex(glm::vec3(w, -h, -l), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec2(1, 0)));
        vertices.push_back(Vertex(glm::vec3(w, -h, l), glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(0, 1)));
        vertices.push_back(Vertex(glm::vec3(w, h, -l), glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(1, 1)));
        vertices.push_back(Vertex(glm::vec3(w, h, l), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0, 0)));
        indices = {1, 5, 3, 5, 7, 3,
                    2, 4, 0, 2, 6, 4,
                    0, 1, 2, 1, 3, 2,
                    6, 5, 4, 6, 7, 5,
                    2, 3, 6, 6, 3, 7,
                    4, 1, 0, 5, 1, 4};
        if (generateBuffers)
            mesh->generateBuffers();
        return mesh;
   }

    std::shared_ptr<Mesh> Mesh::sphere(GLfloat radius, unsigned int ringCount, bool generateBuffers) {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        std::vector<Vertex>& vertices(mesh->verticesRef());
        std::vector<GLuint>& indices(mesh->indicesRef());
        const float PI = 3.141592;
        int rx = 2 * ringCount + 1;
        int ry = ringCount + 2;
        vertices.resize(rx * ry);
        for (int i = 0; i < rx; i++) {
            for (int j = 0; j < ry; j++) {
                float px = (float)i / (float)(rx - 1) * PI;
                float py = (float)j / (float)(ry - 1) * PI;
                Vertex& v(vertices[j * rx + i]);
                v.normal = glm::vec3(sin(px * 2) * sin(py), cos(py), sin(px * 2 + PI / 2) * sin(py));
                v.position = glm::vec3(v.normal.x * radius, v.normal.y * radius, v.normal.z * radius);
                v.uv = glm::vec2(2 * px / PI, py / PI);
                v = Vertex(v.position, v.normal, v.uv);
                if (i > 0 && j > 0) {
                    indices.push_back((j - 0) * rx + (i - 0));
                    indices.push_back((j - 1) * rx + (i - 0));
                    indices.push_back((j - 0) * rx + (i - 1));
                    indices.push_back((j - 1) * rx + (i - 1));
                    indices.push_back((j - 0) * rx + (i - 1));
                    indices.push_back((j - 1) * rx + (i - 0));
                }
            }
        }
        if (generateBuffers)
            mesh->generateBuffers();
        return mesh;
    }
    
//    Mesh Mesh::objModel(const char *filename, int options);
//    Mesh Mesh::objModel(std::istream& file, int options);
    
}
