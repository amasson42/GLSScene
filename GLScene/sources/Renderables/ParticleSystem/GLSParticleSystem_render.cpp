//
//  GLSParticleSystem_render.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSParticleSystem.hpp"

namespace GLS {

    void ParticleSystem::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
        (void)scene;
        if (!bufferGenerated()) {
            generateBuffers();
            if (!bufferGenerated())
                return;
        }

        std::shared_ptr<ShaderProgram> program;
        if (1) {
            program = ShaderProgram::standardShaderProgramDotParticleSystem();
            glEnable(GL_PROGRAM_POINT_SIZE);
        } else {
            program = ShaderProgram::standardShaderProgramTexturedParticleSystem();
        }
        program->use();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glDisable(GL_STENCIL_TEST);

        uniforms.sendUniformsToShaderProgram(program);

        glBindVertexArray(_glVertexArray);
        glDrawArrays(GL_POINTS, 0, _properties.count);
    }

    void ParticleSystem::renderInDepthContext(Scene& scene, const RenderUniforms& uniforms) {
        return renderInContext(scene, uniforms);
    }

}
