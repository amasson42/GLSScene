//
//  GLSInterpolator.hpp
//  GLScene
//
//  Created by Arthur Masson on 15/05/2019.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSInterpolator_h
#define GLSInterpolator_h

#include "GLSCurveFunction.hpp"

float mix(float a, float b, GLS::timefloat t);
glm::vec3 mix(glm::vec3 a, glm::vec3 b, GLS::timefloat t);
glm::quat mix(glm::quat a, glm::quat b, GLS::timefloat t);

namespace GLS {

    template <typename KeyValue>
    class Interpolator {

        struct Value {
            KeyValue value;
            CurveFunction<timefloat> function;

            Value() {}
            Value(KeyValue newValue, CurveFunction<timefloat> newFunction) :
            value(newValue),
            function(newFunction)
            {}
        };

        std::map<timefloat, Value> _keyValues;

        public:

        Interpolator() : _keyValues() {

        }

        Interpolator(const Interpolator<KeyValue>& copy) : _keyValues(copy._keyValues) {

        }

        virtual ~Interpolator() {

        }

        bool isEmpty() const {
            return _keyValues.empty();
        }

        timefloat duration() const {
            if (_keyValues.empty()) {
                return 0;
            } else {
                return _keyValues.rbegin()->first;
            }
        }

        void addKeyValueAt(const KeyValue& value, timefloat time, CurveFunction<timefloat> function = CurveFunction<timefloat>::linear()) {
            _keyValues[time] = Value(value, function);
        }

        void removeKeyValueAt(int index) {
            auto it = _keyValues.begin();
            std::advance(it, index);
            if (it != _keyValues.end()) {
                _keyValues.erase(it);
            }
        }

        void removeKeyValueAt(timefloat time) {
            auto it = _keyValues.lower_bound(time);
            if (it != _keyValues.end()) {
                _keyValues.erase(it);
            }
        }

        KeyValue valueAt(timefloat time) const {
            if (_keyValues.empty())
                throw std::runtime_error("access value from empty Interpolator");

            auto next = _keyValues.lower_bound(time);
            if (next == _keyValues.begin())
                return next->second.value;

            auto prev = std::prev(next);
            if (next == _keyValues.end())
                return prev->second.value;
            
            timefloat t = (time - prev->first) / (next->first - prev->first);
            t = next->second.function(t);
            return mix(prev->second.value, next->second.value, t);
        }

        void send_to_flux(std::ostream& flux) const {
            flux << "Interpolator" << std::endl;
            flux << "duration = " << duration() << std::endl;
            for (auto it = _keyValues.begin(); it != _keyValues.end(); ++it) {
                flux << "  time:" << it->first << " - " << it->second.value << std::endl;
            }
        }

    };

    template <typename KeyValue>
    std::ostream& operator<<(std::ostream& out, const Interpolator<KeyValue>& interpolator) {
        interpolator.send_to_flux(out);
        return out;
    }
    
    class TransformInterpolator {
        Interpolator<glm::vec3> _keyPositions;
        Interpolator<glm::quat> _keyRotations;
        Interpolator<glm::vec3> _keyScales;

        public:

        TransformInterpolator();
        TransformInterpolator(const TransformInterpolator& copy);

        virtual ~TransformInterpolator();

        TransformInterpolator& operator=(const TransformInterpolator& copy);

        Interpolator<glm::vec3>& keyPositions();
        const Interpolator<glm::vec3>& keyPositions() const;

        Interpolator<glm::quat>& keyRotations();
        const Interpolator<glm::quat>& keyRotations() const;

        Interpolator<glm::vec3>& keyScales();
        const Interpolator<glm::vec3>& keyScales() const;

        void addPositionAt(glm::vec3 position, timefloat time, CurveFunction<timefloat> function = CurveFunction<timefloat>());
        void addRotationAt(glm::quat rotation, timefloat time, CurveFunction<timefloat> function = CurveFunction<timefloat>());
        void addScaleAt(glm::vec3 rotation, timefloat time, CurveFunction<timefloat> function = CurveFunction<timefloat>());
        void addMatrixAt(glm::mat4 matrix, timefloat time, CurveFunction<timefloat> function = CurveFunction<timefloat>());

        Transform transformAt(timefloat time) const;

        timefloat duration() const;

    };

}

#endif /* GLSInterpolator_h */
