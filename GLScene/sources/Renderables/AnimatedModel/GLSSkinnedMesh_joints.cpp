//
//  GLSSkinnedMesh_joints.cpp
//  GLScene
//
//  Created by Arthur Masson on 26/06/2019.
//  Copyright © 2019 Arthur Masson. All rights reserved.
//

#include "GLSSkinnedMesh.hpp"

namespace GLS {

    Joint::Joint(int arg_id, std::string arg_name) :
    id(arg_id), name(arg_name),
    childs(),
    transform() {
        
    }

    void Joint::sendUniformsToShaderProgram(std::shared_ptr<ShaderProgram> program, glm::mat4 parentMatrix) const {
        program->use();
        glm::mat4 modelMatrix = parentMatrix * transform.matrix();
        if (id < SkinnedMesh::maxBones) {
            glUniformMatrix4fv(program->getLocation("u_mat_joints[" + std::to_string(id) + "]"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        }
        for (size_t i = 0; i < childs.size(); i++) {
            childs[i].sendUniformsToShaderProgram(program, modelMatrix);
        }
    }

}
