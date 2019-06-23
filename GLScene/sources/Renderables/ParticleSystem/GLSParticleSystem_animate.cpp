//
//  GLSParticleSystem_update.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSParticleSystem.hpp"

namespace GLS {

    void ParticleSystem::initAnimation() {
        if (bufferGenerated()) {
            CLD::Kernel *ik = _device->kernel(_initKernelIndex);
            _device->commandQueue(_commandQueueIndex)->runNDRangeKernel(*ik, _properties.count);
        }
    }

    void ParticleSystem::animateWithDeltaTime(double deltaTime) {
        if (bufferGenerated()) {
            CLD::Kernel *uk = _device->kernel(_updateKernelIndex);
            uk->setArgument(2, deltaTime);
            _device->commandQueue(_commandQueueIndex)->runNDRangeKernel(*uk, _properties.count);

            // TODO: do it at the end of all scene calls
            _device->commandQueue(_commandQueueIndex)->finish();
        }
    }

    CLD::Kernel *ParticleSystem::getInitKernel() {
        if (_initKernelIndex < 0)
            return NULL;
        return _device->kernel(_initKernelIndex);
    }

    CLD::Kernel *ParticleSystem::getAnimationKernel() {
        if (_updateKernelIndex < 0)
            return NULL;
        return _device->kernel(_updateKernelIndex);
    }

}
