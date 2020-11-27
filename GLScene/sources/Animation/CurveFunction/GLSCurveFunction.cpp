//
//  GLSCurveFunction.cpp
//  GLScene
//
//  Created by Arthur Masson on 27/11/2020.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSInterpolator.hpp"

namespace GLS {

    CurveFunction::CurveFunction() :
    _f(linearCurve)
    {}

    CurveFunction::CurveFunction(double (*f)(double)) :
    _f(f)
    {}

    CurveFunction::CurveFunction(const CurveFunction& copy) :
    _f(copy._f)
    {}

    CurveFunction::~CurveFunction()
    {}

    CurveFunction& CurveFunction::operator=(const CurveFunction& copy) {
        _f = copy._f;
        return *this;
    }

    double CurveFunction::operator()(double t) const {
        return _f(t);
    }

    CurveFunction CurveFunction::custom(double (*f)(double)) {
        return CurveFunction(f);
    }

    CurveFunction CurveFunction::linear() {
        return CurveFunction();
    }

    CurveFunction CurveFunction::easeIn() {
        return CurveFunction(easeInQuad);
    }

    CurveFunction CurveFunction::easeOut() {
        return CurveFunction(easeOutQuad);
    }

    CurveFunction CurveFunction::easeInOut() {
        return CurveFunction(easeInOutQuad);
    }

    CurveFunction CurveFunction::elastic() {
        return CurveFunction(easeOutElastic);
    }

    CurveFunction CurveFunction::bounce() {
        return CurveFunction(easeOutBounce);
    }


}
