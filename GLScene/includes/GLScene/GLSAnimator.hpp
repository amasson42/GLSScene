//
//  GLSAnimator.hpp
//  GLScene
//
//  Created by Arthur Masson on 15/05/2019.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSAnimator_h
#define GLSAnimator_h

#include "GLSStructs.hpp"

namespace GLS {

    template <typename FrameValue>
    class KeyFrames {

        struct Frame {
            FrameValue value;
            int function;

            Frame() {}
            Frame(FrameValue newValue, int newFunction) :
            value(newValue),
            function(newFunction)
            {}
        };

        std::map<double, Frame> _keyframes;

        public:

        KeyFrames() : _keyframes() {

        }

        KeyFrames(const KeyFrames<FrameValue>& copy) : _keyframes(copy._keyframes) {

        }

        virtual ~KeyFrames() {

        }

        double duration() const {
            if (_keyframes.empty()) {
                return 0;
            } else {
                return _keyframes.rbegin()->first;
            }
        }

        void addKeyframeAt(double time, const FrameValue& value, int function = 0) {
            _keyframes[time] = Frame(value, function);
        }

        void removeKeyframeAt(int index) {
            auto it = _keyframes.begin();
            std::advance(it, index);
            if (it != _keyframes.end()) {
                _keyframes.erase(it);
            }
        }

        void removeKeyframeAt(double time) {
            auto it = _keyframes.lower_bound(time);
            if (it != _keyframes.end()) {
                _keyframes.erase(it);
            }
        }

        FrameValue frameAt(double time) const {
            if (_keyframes.empty())
                throw std::runtime_error("access frame from empty keyframes");

            auto next = _keyframes.lower_bound(time);
            if (next == _keyframes.begin())
                return next->second.value;

            auto prev = std::prev(next);
            if (next == _keyframes.end())
                return prev->second.value;
            
            double t = (time - prev->first) / (next->first - prev->first);
            return FrameValue::linearValue(prev->second.value, next->second.value, t);
        }

        void send_to_flux(std::ostream& flux) const {
            flux << "keyframes" << std::endl;
            flux << "duration = " << duration() << std::endl;
            for (auto it = _keyframes.begin(); it != _keyframes.end(); ++it) {
                flux << "  time:" << it->first << " - " << it->second.value << std::endl;
            }
        }

    };

    template <typename FrameValue>
    std::ostream& operator<<(std::ostream& out, const KeyFrames<FrameValue>& keyframes) {
        keyframes.send_to_flux(out);
        return out;
    }
    
}

#endif /* GLSAnimator_h */
