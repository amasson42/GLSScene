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

    RenderUniforms::RenderUniforms() {
        projection = glm::mat4(1);
        view = glm::mat4(1);
        model = glm::mat4(1);
        camera_position = glm::vec3(0);
    }

    RenderUniforms::RenderUniforms(const glm::mat4& p,
                                   const glm::mat4& v,
                                   const glm::mat4& m,
                                   const glm::vec3& c) {
        projection = p;
        view = v;
        model = m;
        camera_position = c;
    }

    void RenderUniforms::sendUniformsToShaderProgram(std::shared_ptr<ShaderProgram> program) const {
        program->use();
        glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(program->getLocation("u_mat_view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(program->getLocation("u_mat_model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(program->getLocation("u_camera_position"), camera_position.x,
                                                               camera_position.y,
                                                               camera_position.z);
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
        else if (n == glm::vec3(0, -1, 0))
            tangent = glm::vec3(-1, 0, 0);
        else
            tangent = glm::cross(glm::vec3(0, 1, 0), n);
        bitangent = glm::cross(tangent, n);
        uv = u;
    }

    void glsInit() {
        Framebuffer::_createRectBuffer();
        Skybox::_createCubeBuffer();
        
        ShaderProgram::standardShaderProgramMesh();
        ShaderProgram::standardShaderProgramMeshSimpleColor();
        ShaderProgram::standardShaderProgramInstancedMesh();
        ShaderProgram::standardShaderProgramInstancedMeshSimpleColor();
        ShaderProgram::standardShaderProgramScreenTexture();
        ShaderProgram::standardShaderProgramSkybox();
        ShaderProgram::standardShaderProgramVoxelChunk();
        ShaderProgram::standardShaderProgramVoxelChunkSimpleColor();
    }

    void glsDeinit() {
        Framebuffer::_destroyRectBuffer();
        Skybox::_destroyCubeBuffer();

        ShaderProgram::_standardShaderProgramMesh = nullptr;
        ShaderProgram::_standardShaderProgramMeshSimpleColor = nullptr;
        ShaderProgram::_standardShaderProgramInstancedMesh = nullptr;
        ShaderProgram::_standardShaderProgramInstancedMeshSimpleColor = nullptr;
        ShaderProgram::_standardShaderProgramScreenTexture = nullptr;
        ShaderProgram::_standardShaderProgramSkybox = nullptr;
        ShaderProgram::_standardShaderProgramVoxelChunk = nullptr;
        ShaderProgram::_standardShaderProgramVoxelChunkSimpleColor = nullptr;
    }

}
