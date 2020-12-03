//
//  GLSIAnimatable.cpp
//  GLScene
//
//  Created by Arthur Masson on 02/12/2020.
//  Copyright © 2020 Arthur Masson. All rights reserved.
//

#include "GLSIAnimatable.hpp"

namespace GLS {

    void IAnimatable::sendToFlux(std::ostream& flux, std::string linePrefix, std::string firstPrefix, std::string lastPrefix) const {
        (void)linePrefix;
        flux << firstPrefix << "[IAnimatable]" << std::endl;
        flux << lastPrefix << "" << std::endl;
    }

}
