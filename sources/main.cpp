//
//  main.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "AppEnv.hpp"

void printHelp(const char *argv0) {
    static bool didOnce = false;
    if (didOnce) return;
    didOnce = true;

    std::cout << "usage: " << argv0 << " [-effect post_processing_effect.glsl]" << std::endl;
}

int main(int argc, const char * argv[]) {
    bool loopLeaks = false;
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-leaks") == 0) {
            loopLeaks = true;
        } else if (strcmp(argv[i], "-help") == 0) {
            printHelp(argv[0]);
        } else {
            args.push_back(argv[i]);
        }
    }
    try {
        AppEnv env(args);
        env.loop();
    } catch (std::exception& e) {
        std::cerr << "Program stop with exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    if (loopLeaks) {
        std::cout << "test leaks..." << std::endl;
        while (1);
    }
    return EXIT_SUCCESS;
}
