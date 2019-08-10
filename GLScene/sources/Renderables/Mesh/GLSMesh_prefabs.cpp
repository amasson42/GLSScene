//
//  GLSMesh_prefabs.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {
    
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

        glm::vec3 pos_000 = glm::vec3(-w, -h, -l);
        glm::vec3 pos_001 = glm::vec3(-w, -h, l);
        glm::vec3 pos_010 = glm::vec3(-w, h, -l);
        glm::vec3 pos_011 = glm::vec3(-w, h, l);

        glm::vec3 pos_100 = glm::vec3(w, -h, -l);
        glm::vec3 pos_101 = glm::vec3(w, -h, l);
        glm::vec3 pos_110 = glm::vec3(w, h, -l);
        glm::vec3 pos_111 = glm::vec3(w, h, l);

        glm::vec3 dir_x = glm::vec3(1, 0, 0);
        glm::vec3 dir_y = glm::vec3(0, 1, 0);
        glm::vec3 dir_z = glm::vec3(0, 0, 1);

        glm::vec2 uv_00 = glm::vec2(0, 0);
        glm::vec2 uv_01 = glm::vec2(0, 1);
        glm::vec2 uv_10 = glm::vec2(1, 0);
        glm::vec2 uv_11 = glm::vec2(1, 1);

        int io = 0;

        // face x
        vertices.push_back(Vertex(pos_100, dir_x, dir_z, dir_y, uv_00));
        vertices.push_back(Vertex(pos_101, dir_x, dir_z, dir_y, uv_10));
        vertices.push_back(Vertex(pos_111, dir_x, dir_z, dir_y, uv_11));
        vertices.push_back(Vertex(pos_110, dir_x, dir_z, dir_y, uv_01));
        indices.push_back(io + 0);indices.push_back(io + 2);indices.push_back(io + 1);
        indices.push_back(io + 0);indices.push_back(io + 3);indices.push_back(io + 2);
        io += 4;

        // face -x
        vertices.push_back(Vertex(pos_001, -dir_x, -dir_z, dir_y, uv_00));
        vertices.push_back(Vertex(pos_000, -dir_x, -dir_z, dir_y, uv_10));
        vertices.push_back(Vertex(pos_010, -dir_x, -dir_z, dir_y, uv_11));
        vertices.push_back(Vertex(pos_011, -dir_x, -dir_z, dir_y, uv_01));
        indices.push_back(io + 0);indices.push_back(io + 2);indices.push_back(io + 1);
        indices.push_back(io + 0);indices.push_back(io + 3);indices.push_back(io + 2);
        io += 4;

        // face y
        vertices.push_back(Vertex(pos_010, dir_y, dir_x, dir_z, uv_00));
        vertices.push_back(Vertex(pos_110, dir_y, dir_x, dir_z, uv_10));
        vertices.push_back(Vertex(pos_111, dir_y, dir_x, dir_z, uv_11));
        vertices.push_back(Vertex(pos_011, dir_y, dir_x, dir_z, uv_01));
        indices.push_back(io + 0);indices.push_back(io + 2);indices.push_back(io + 1);
        indices.push_back(io + 0);indices.push_back(io + 3);indices.push_back(io + 2);
        io += 4;

        // face -y
        vertices.push_back(Vertex(pos_100, -dir_y, -dir_x, -dir_z, uv_00));
        vertices.push_back(Vertex(pos_000, -dir_y, -dir_x, -dir_z, uv_10));
        vertices.push_back(Vertex(pos_001, -dir_y, -dir_x, -dir_z, uv_11));
        vertices.push_back(Vertex(pos_101, -dir_y, -dir_x, -dir_z, uv_01));
        indices.push_back(io + 0);indices.push_back(io + 2);indices.push_back(io + 1);
        indices.push_back(io + 0);indices.push_back(io + 3);indices.push_back(io + 2);
        io += 4;

        // face z
        vertices.push_back(Vertex(pos_101, dir_z, -dir_x, dir_y, uv_00));
        vertices.push_back(Vertex(pos_001, dir_z, -dir_x, dir_y, uv_10));
        vertices.push_back(Vertex(pos_011, dir_z, -dir_x, dir_y, uv_11));
        vertices.push_back(Vertex(pos_111, dir_z, -dir_x, dir_y, uv_01));
        indices.push_back(io + 0);indices.push_back(io + 2);indices.push_back(io + 1);
        indices.push_back(io + 0);indices.push_back(io + 3);indices.push_back(io + 2);
        io += 4;

        // face -z
        vertices.push_back(Vertex(pos_000, -dir_z, dir_x, dir_y, uv_00));
        vertices.push_back(Vertex(pos_100, -dir_z, dir_x, dir_y, uv_10));
        vertices.push_back(Vertex(pos_110, -dir_z, dir_x, dir_y, uv_11));
        vertices.push_back(Vertex(pos_010, -dir_z, dir_x, dir_y, uv_01));
        indices.push_back(io + 0);indices.push_back(io + 2);indices.push_back(io + 1);
        indices.push_back(io + 0);indices.push_back(io + 3);indices.push_back(io + 2);
        io += 4;

        if (generateBuffers)
            mesh->generateBuffers();
        return mesh;
   }

    std::shared_ptr<Mesh> Mesh::sphere(GLfloat radius, unsigned int ringCount, bool generateBuffers) {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        std::vector<Vertex>& vertices(mesh->verticesRef());
        std::vector<GLuint>& indices(mesh->indicesRef());
        const float PI = 3.141592f;
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

    std::shared_ptr<Mesh> Mesh::thinLine(glm::vec3 start, glm::vec3 end, bool generateBuffers) {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

        mesh->verticesRef().push_back(Vertex(start, glm::normalize(start - end), glm::vec2(0, 0)));
        mesh->verticesRef().push_back(Vertex(end, glm::normalize(end - start), glm::vec2(1, 0)));

        mesh->indicesRef().push_back(0);
        mesh->indicesRef().push_back(1);

        mesh->setDrawMode(GL_LINES);

        if (generateBuffers)
            mesh->generateBuffers();

        return mesh;
    }
    
}
