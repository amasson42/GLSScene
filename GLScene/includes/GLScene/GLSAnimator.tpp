//
//  GLSAnimator.tpp
//  GLScene
//
//  Created by Arthur Masson on 15/05/2019.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSAnimator_t
#define GLSAnimator_t

#include "GLScene.hpp"

static inline std::vector<std::string> _splitWithSpaces(const std::string& str) {
    std::vector<std::string> words;
    std::stringstream ss(str);
    std::string tok;
    while (std::getline(ss, tok, ' ')) {
        words.push_back(tok);
    }
    return words;
}

namespace GLS {

    template <typename Aframe>
    class Animator {

        std::map<double, Aframe> _keyframes;

        public:

        Animator() : _keyframes() {

        }

        Animator(const Animator<Aframe>& copy) : _keyframes(copy._keyframes) {

        }

        bool readValueFromWords(const std::vector<std::string>& words, Aframe& frame) {
            if (words.size() < 2)
                return false;
            if (words[0] == "time") {
                addKeyframeAt(std::atof(words[1].c_str()), frame);
                frame = Aframe();
                return true;
            }
            return false;
        }

        Animator(std::ifstream& input) : _keyframes() {
            Aframe frame = Aframe();
            std::string line;
            while (std::getline(input, line)) {
                std::vector<std::string> words = _splitWithSpaces(line);
                if (readValueFromWords(words, frame) == 0)
                    frame.readValuesFromWords(words);
            }
        }

        virtual ~Animator() {

        }

        double time() const {
            if (_keyframes.empty()) {
                return 0;
            } else {
                return _keyframes.rbegin()->first;
            }
        }

        void addKeyframeAt(double time, const Aframe& aframe) {
            _keyframes[time] = aframe;
        }

        void removeKeyframeAt(double time) {
            auto it = _keyframes.find(time);
            if (it != _keyframes.end()) {
                _keyframes.erase(it);
            }
        }

        Aframe frameAt(double time) const {
            auto it = _keyframes.begin();
            if (it->first > time) {
                return it->second;
            }
            auto previous = it;
            ++it;
            while (it != _keyframes.end()) {
                if (it->first > time) {
                    double t = it->first - previous->first;
                    t = (time - previous->first) / t;
                    return Aframe::linearValue(previous->second, it->second, t);
                }
                previous = it;
                ++it;
            }
            return previous->second;
        }

        Aframe& firstFrameAfter(double time) {
            for (auto it = _keyframes.begin(); it != _keyframes.end(); ++it) {
                if (it->first > time)
                    return it->second;
            }
            return _keyframes.rbegin()->second;
        }

        void send_to_flux(std::ostream& flux) const {
            flux << "animator" << std::endl;
            flux << "time = " << time() << std::endl;
            for (auto it = _keyframes.begin(); it != _keyframes.end(); ++it) {
                flux << "time:" << it->first << " - " << it->second << std::endl;
            }
        }

    };

    template <typename Aframe>
    std::ostream& operator<<(std::ostream& out, const GLS::Animator<Aframe>& animator) {
        animator.send_to_flux(out);
        return out;
    }
    
}

#endif /* GLSAnimator_t */
