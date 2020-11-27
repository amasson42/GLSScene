//
//  GLSTransformInterpolator.cpp
//  GLScene
//
//  Created by Arthur Masson on 23/11/2020.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSTransform.hpp"
#include "GLSInterpolator.hpp"

float mix(float a, float b, double t) {
    return glm::mix<float, double>(a, b, t);
}

glm::vec3 mix(glm::vec3 a, glm::vec3 b, double t) {
    return glm::vec3(
        mix(a.x, b.x, t),
        mix(a.y, b.y, t),
        mix(a.z, b.z, t)
    );
}

glm::quat mix(glm::quat a, glm::quat b, double t) {
    if (a == b)
        return a;
    return glm::lerp<float>(a, b, t);
}

namespace GLS {
    
    TransformInterpolator::TransformInterpolator() {}

    TransformInterpolator::TransformInterpolator(const TransformInterpolator& copy) :
    _keyPositions(copy._keyPositions),
    _keyRotations(copy._keyRotations),
    _keyScales(copy._keyScales)
    {}

    TransformInterpolator::~TransformInterpolator() {}

    TransformInterpolator& TransformInterpolator::operator=(const TransformInterpolator& copy) {
        _keyPositions = copy._keyPositions;
        _keyRotations = copy._keyRotations;
        _keyScales = copy._keyScales;
        return *this;
    }

    Interpolator<glm::vec3>& TransformInterpolator::keyPositions() {
        return _keyPositions;
    }

    const Interpolator<glm::vec3>& TransformInterpolator::keyPositions() const {
        return _keyPositions;
    }

    Interpolator<glm::vec3>& TransformInterpolator::keyScales() {
        return _keyScales;
    }

    const Interpolator<glm::vec3>& TransformInterpolator::keyScales() const {
        return _keyScales;
    }

    Interpolator<glm::quat>& TransformInterpolator::keyRotations() {
        return _keyRotations;
    }

    const Interpolator<glm::quat>& TransformInterpolator::keyRotations() const {
        return _keyRotations;
    }

    void TransformInterpolator::addPositionAt(glm::vec3 position, double time, CurveFunction function) {
        _keyPositions.addKeyValueAt(position, time, function);
    }

    void TransformInterpolator::addRotationAt(glm::quat rotation, double time, CurveFunction function) {
        _keyRotations.addKeyValueAt(rotation, time, function);
    }

    void TransformInterpolator::addScaleAt(glm::vec3 rotation, double time, CurveFunction function) {
        _keyScales.addKeyValueAt(rotation, time, function);
    }

    void TransformInterpolator::addMatrixAt(glm::mat4 matrix, double time, CurveFunction function) {
        Transform t;
        t.setMatrix(matrix);
        addPositionAt(t.position(), time, function);
        addRotationAt(t.rotation(), time, function);
        addScaleAt(t.scale(), time, function);
    }

    Transform TransformInterpolator::transformAt(double time) const {
        Transform transframe;

        if (!_keyPositions.isEmpty()) {
            transframe.setPosition(_keyPositions.valueAt(time));
        }
        if (!_keyRotations.isEmpty()) {
            transframe.setRotation(_keyRotations.valueAt(time));
        }
        if (!_keyScales.isEmpty()) {
            transframe.setScale(_keyScales.valueAt(time));
        }

        return transframe;
    }

    double TransformInterpolator::duration() const {
        return std::max(_keyPositions.duration(),
                        std::max(_keyRotations.duration(),
                                _keyScales.duration()));
    }

}
