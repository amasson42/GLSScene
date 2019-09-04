//
//  GLSIMeshSource.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSIMeshSource_h
#define GLSIMeshSource_h

#include "GLSStructs.hpp"
#include "GLSMesh.hpp"

namespace GLS {

    class IMeshSource {
        public:
        virtual std::shared_ptr<Mesh> bakeMesh() const = 0;
    };

}

#endif /* GLSIMeshSource_h */
