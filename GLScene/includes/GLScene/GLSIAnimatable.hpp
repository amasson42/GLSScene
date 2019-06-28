//
//  GLSIAnimatable.hpp
//  GLScene
//
//  Created by Arthur Masson on 15/05/2019.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSAnimatable_h
#define GLSAnimatable_h

#include "GLScene.hpp"

namespace GLS {

    class IAnimatable {

        public:

        virtual void initAnimation() = 0;
        virtual void animate(float deltaTime) = 0;

    };

}

#endif /* GLSAnimatable_h */
