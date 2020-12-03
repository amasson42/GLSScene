//
//  GLSIAnimatable.hpp
//  GLScene
//
//  Created by Arthur Masson on 15/05/2019.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSAnimatable_h
#define GLSAnimatable_h

#include "GLSStructs.hpp"

namespace GLS {

    class IAnimatable {

        public:

        virtual void initAnimation() = 0;
        virtual void animate(timefloat deltaTime) = 0;
        virtual bool alive() const = 0;

        virtual void sendToFlux(std::ostream& flux, std::string linePrefix, std::string firstPrefix, std::string lastPrefix) const;

    };

}

#endif /* GLSAnimatable_h */
