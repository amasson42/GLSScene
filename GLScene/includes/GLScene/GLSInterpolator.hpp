//
//  GLSInterpolator.hpp
//  GLScene
//
//  Created by Arthur Masson on 15/05/2019.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSInterpolator_h
#define GLSInterpolator_h

#include "GLSStructs.hpp"

float mix(float a, float b, double t);
glm::vec3 mix(glm::vec3 a, glm::vec3 b, double t);
glm::quat mix(glm::quat a, glm::quat b, double t);

namespace GLS {

    class CurveFunction {

        double (*_f)(double);
        
        public:

        CurveFunction();
        CurveFunction(double (*f)(double));
        CurveFunction(const CurveFunction& copy);
        virtual ~CurveFunction();

        CurveFunction& operator=(const CurveFunction& copy);

        double operator()(double t) const;

        static CurveFunction custom(double (*f)(double));
        static CurveFunction linear();
        // TODO: add all other functions
    };

    template <typename KeyValue>
    class Interpolator {

        struct Value {
            KeyValue value;
            CurveFunction function;

            Value() {}
            Value(KeyValue newValue, CurveFunction newFunction) :
            value(newValue),
            function(newFunction)
            {}
        };

        std::map<double, Value> _keyValues;

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

        double duration() const {
            if (_keyValues.empty()) {
                return 0;
            } else {
                return _keyValues.rbegin()->first;
            }
        }

        void addKeyValueAt(const KeyValue& value, double time, CurveFunction function = CurveFunction::linear()) {
            _keyValues[time] = Value(value, function);
        }

        void removeKeyValueAt(int index) {
            auto it = _keyValues.begin();
            std::advance(it, index);
            if (it != _keyValues.end()) {
                _keyValues.erase(it);
            }
        }

        void removeKeyValueAt(double time) {
            auto it = _keyValues.lower_bound(time);
            if (it != _keyValues.end()) {
                _keyValues.erase(it);
            }
        }

        KeyValue valueAt(double time) const {
            if (_keyValues.empty())
                throw std::runtime_error("access value from empty Interpolator");

            auto next = _keyValues.lower_bound(time);
            if (next == _keyValues.begin())
                return next->second.value;

            auto prev = std::prev(next);
            if (next == _keyValues.end())
                return prev->second.value;
            
            double t = (time - prev->first) / (next->first - prev->first);
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

        void addPositionAt(glm::vec3 position, double time, CurveFunction function = CurveFunction::linear());
        void addRotationAt(glm::quat rotation, double time, CurveFunction function = CurveFunction::linear());
        void addScaleAt(glm::vec3 rotation, double time, CurveFunction function = CurveFunction::linear());
        void addMatrixAt(glm::mat4 matrix, double time, CurveFunction function = CurveFunction::linear());

        Transform transformAt(double time) const;

    };

}

#endif /* GLSInterpolator_h */
