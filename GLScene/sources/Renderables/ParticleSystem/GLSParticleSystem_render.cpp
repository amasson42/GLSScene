//
//  GLSParticleSystem_render.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSParticleSystem.hpp"
#include "GLSScene.hpp"

namespace GLS {

    void ParticleSystem::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
        (void)scene;
        if (!bufferGenerated()) {
            generateBuffers();
            if (!bufferGenerated())
                return;
        }

        std::shared_ptr<ShaderProgram> program;
        if (_texture == nullptr) {
            program = ShaderProgram::standardShaderProgramDotParticleSystem();
            program->use();
            glEnable(GL_PROGRAM_POINT_SIZE);
        } else {
            program = ShaderProgram::standardShaderProgramTexturedParticleSystem();
            program->use();
            _texture->sendUniformsToShaderProgram(program, "particle_texture", 0);
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        uniforms.sendUniformsToShaderProgram(program);

        glBindVertexArray(_glVertexArray);
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(_properties.count));
    }

    void ParticleSystem::renderInDepthContext(Scene& scene, const RenderUniforms& uniforms) {
        return renderInContext(scene, uniforms);
    }

}
