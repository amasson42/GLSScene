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
    
    std::shared_ptr<Mesh> Mesh::cube(GLfloat w, GLfloat h, GLfloat l, bool generateBuffers) {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        std::vector<Vertex>& vertices(mesh->verticesRef());
        std::vector<GLuint>& indices(mesh->indicesRef());
        w /= 2.0;
        h /= 2.0;
        l /= 2.0;
        vertices.push_back(Vertex(glm::vec3(-w, -h, -l), glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec4(1), glm::vec2(0, 0)));
        vertices.push_back(Vertex(glm::vec3(-w, -h, l), glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec4(1), glm::vec2(1, 1)));
        vertices.push_back(Vertex(glm::vec3(-w, h, -l), glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec4(1), glm::vec2(0, 1)));
        vertices.push_back(Vertex(glm::vec3(-w, h, l), glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec4(1), glm::vec2(1, 0)));
        vertices.push_back(Vertex(glm::vec3(w, -h, -l), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec4(1), glm::vec2(1, 0)));
        vertices.push_back(Vertex(glm::vec3(w, -h, l), glm::vec3(1.0f, -1.0f, 1.0f), glm::vec4(1), glm::vec2(0, 1)));
        vertices.push_back(Vertex(glm::vec3(w, h, -l), glm::vec3(1.0f, 1.0f, -1.0f), glm::vec4(1), glm::vec2(1, 1)));
        vertices.push_back(Vertex(glm::vec3(w, h, l), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(1), glm::vec2(0, 0)));
            // TODO: better way maybe
        indices.push_back(1);
        indices.push_back(5);
        indices.push_back(3);
        indices.push_back(5);
        indices.push_back(7);
        indices.push_back(3);
        indices.push_back(2);
        indices.push_back(4);
        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(6);
        indices.push_back(4);
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(1);
        indices.push_back(3);
        indices.push_back(2);
        indices.push_back(6);
        indices.push_back(5);
        indices.push_back(4);
        indices.push_back(6);
        indices.push_back(7);
        indices.push_back(5);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(6);
        indices.push_back(6);
        indices.push_back(3);
        indices.push_back(7);
        indices.push_back(4);
        indices.push_back(1);
        indices.push_back(0);
        indices.push_back(5);
        indices.push_back(1);
        indices.push_back(4);
        if (generateBuffers)
            mesh->generateBuffers();
        return mesh;
   }

    std::shared_ptr<Mesh> Mesh::sphere(GLfloat radius, unsigned int ringCount, bool generateBuffers) {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        std::vector<Vertex>& vertices(mesh->verticesRef());
        std::vector<GLuint>& indices(mesh->indicesRef());
        const float PI = 3.141592;
        int r0 = 2 * ringCount + 1;
        int r1 = ringCount + 2;
        int indiceIndex;
        for (int i = 0; i < r0; i++) {
            for (int j = 0; j < r1; j++) {
                float px = (float)i / (float)(r0 - 1) * PI;
                float py = (float)j / (float)(r1 - 1) * PI;
                Vertex v;
                v.normal = glm::vec3(sin(px * 2) * sin(py), cos(py), sin(px * 2 + PI / 2) * sin(py);
                v.position = glm::vec3(v.normal.x * radius, v.normal.y * radius, v.normal.z * radius);
                v.color = glm::vec4(1);
                v.texture = glm::vec2(2 * px / PI, py / PI);
                vertices[j * r0 + i] = Vertex(glm::vec3());
            }
        }
        if (generateBuffers)
            mesh->generateBuffers();
        return mesh;
    }
//    Mesh Mesh::sphere(GLfloat radius, unsigned int ringCount = 12);
//    Mesh Mesh::objModel(const char *filename, int options);
//    Mesh Mesh::objModel(std::istream& file, int options);
    
}
