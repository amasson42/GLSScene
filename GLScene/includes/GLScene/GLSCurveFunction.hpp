//
//  GLSCurveFunction.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSCurveFunction_h
#define GLSCurveFunction_h

#include "GLSStructs.hpp"

namespace GLS {

    template <typename T = timefloat>
    T linearCurve(T x) {
        return x;
    }

    template <typename T = timefloat>
    T easeInSine(T x) {
        return 1 - cos((x * M_PI) / 2);
    }
    
    template <typename T = timefloat>
    T easeOutSine(T x) {
        return sin((x * M_PI) / 2);
    }
    
    template <typename T = timefloat>
    T easeInOutSine(T x) {
        return -(cos(M_PI * x) - 1) / 2;
    }
    
    template <typename T = timefloat>
    T easeInQuad(T x) {
        return x * x;
    }
    
    template <typename T = timefloat>
    T easeOutQuad(T x) {
        return 1 - (1 - x) * (1 - x);
    }
    
    template <typename T = timefloat>
    T easeInOutQuad(T x) {
        return x < 0.5 ? 2 * x * x : 1 - pow(-2 * x + 2, 2) / 2;
    }
    
    template <typename T = timefloat>
    T easeInCubic(T x) {
        return x * x * x;
    }
    
    template <typename T = timefloat>
    T easeOutCubic(T x) {
        return 1 - pow(1 - x, 3);
    }
    
    template <typename T = timefloat>
    T easeInOutCubic(T x) {
        return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
    }
    
    template <typename T = timefloat>
    T easeInQuart(T x) {
        return x * x * x * x;
    }
    
    template <typename T = timefloat>
    T easeOutQuart(T x) {
        return 1 - pow(1 - x, 4);
    }
    
    template <typename T = timefloat>
    T easeInOutQuart(T x) {
        return x < 0.5 ? 8 * x * x * x * x : 1 - pow(-2 * x + 2, 4) / 2;
    }
    
    template <typename T = timefloat>
    T easeInQuint(T x) {
        return x * x * x * x * x;
    }
    
    template <typename T = timefloat>
    T easeOutQuint(T x) {
        return 1 - pow(1 - x, 5);
    }
    
    template <typename T = timefloat>
    T easeInOutQuint(T x) {
        return x < 0.5 ? 16 * x * x * x * x * x : 1 - pow(-2 * x + 2, 5) / 2;
    }
    
    template <typename T = timefloat>
    T easeInExpo(T x) {
        return x == 0 ? 0 : pow(2, 10 * x - 10);
    }
    
    template <typename T = timefloat>
    T easeOutExpo(T x) {
        return x == 1 ? 1 : 1 - pow(2, -10 * x);
    }
    
    template <typename T = timefloat>
    T easeInOutExpo(T x) {
        return x == 0
            ? 0
            : x == 1
            ? 1
            : x < 0.5 ? pow(2, 20 * x - 10) / 2
            : (2 - pow(2, -20 * x + 10)) / 2;
    }
    
    template <typename T = timefloat>
    T easeInCirc(T x) {
        return 1 - sqrt(1 - pow(x, 2));
    }
    
    template <typename T = timefloat>
    T easeOutCirc(T x) {
        return sqrt(1 - pow(x - 1, 2));
    }
    
    template <typename T = timefloat>
    T easeInOutCirc(T x) {
        return x < 0.5
            ? (1 - sqrt(1 - pow(2 * x, 2))) / 2
            : (sqrt(1 - pow(-2 * x + 2, 2)) + 1) / 2;
    }
    
    template <typename T = timefloat>
    T easeInBack(T x) {
        T c1 = 1.70158;
        T c3 = c1 + 1;
        
        return c3 * x * x * x - c1 * x * x;
    }
    
    template <typename T = timefloat>
    T easeOutBack(T x) {
        T c1 = 1.70158;
        T c3 = c1 + 1;
        
        return 1 + c3 * pow(x - 1, 3) + c1 * pow(x - 1, 2);
    }
    
    template <typename T = timefloat>
    T easeInOutBack(T x) {
        T c1 = 1.70158;
        T c2 = c1 * 1.525;
        
        return x < 0.5
            ? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
            : (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
    }
    
    template <typename T = timefloat>
    T easeInElastic(T x) {
        T c4 = (2 * M_PI) / 3;
        
        return x == 0
            ? 0
            : x == 1
            ? 1
            : -pow(2, 10 * x - 10) * sin((x * 10 - 10.75) * c4);
    }
    
    template <typename T = timefloat>
    T easeOutElastic(T x) {
        T c4 = (2 * M_PI) / 3;
        
        return x == 0
            ? 0
            : x == 1
            ? 1
            : pow(2, -10 * x) * sin((x * 10 - 0.75) * c4) + 1;
    }
    
    template <typename T = timefloat>
    T easeInOutElastic(T x) {
        T c5 = (2 * M_PI) / 4.5;
        
        return x == 0
            ? 0
            : x == 1
            ? 1
            : x < 0.5
            ? -(pow(2, 20 * x - 10) * sin((20 * x - 11.125) * c5)) / 2
            : (pow(2, -20 * x + 10) * sin((20 * x - 11.125) * c5)) / 2 + 1;
    }
    
    template <typename T = timefloat>
    T easeInBounce(T x) {
        return 1 - easeOutBounce(1 - x);
    }
    
    template <typename T = timefloat>
    T easeOutBounce(T x) {
        T n1 = 7.5625;
        T d1 = 2.75;
        
        if (x < 1 / d1) {
            return n1 * x * x;
        } else if (x < 2 / d1) {
            T xm = x - (1.5 / d1);
            return n1 * xm * xm + 0.75;
        } else if (x < 2.5 / d1) {
            T xm = x - (2.25 / d1);
            return n1 * xm * xm + 0.9375;
        } else {
            T xm = x - (2.625 / d1);
            return n1 * xm * xm + 0.984375;
        }
    }

    template <typename T = timefloat>
    T easeInTeleport(T x) {
        return x < 1.0 ? 0.0 : 1.0;
    }

    template <typename T = timefloat>
    T easeOutTeleport(T x) {
        return x < 0.0 ? 0.0 : 1.0;
    }

    template <typename T = timefloat>
    T easeInOutTeleport(T x) {
        return x < 0.5 ? 0.0 : 1.0;
    }
    
    template <typename T = timefloat>
    T easeInOutBounce(T x) {
        return x < 0.5
            ? (1 - easeOutBounce(1 - 2 * x)) / 2
            : (1 + easeOutBounce(2 * x - 1)) / 2;
    }

    template <typename T = timefloat>
    class CurveFunction {

        T (*_f)(T);
        
        public:

        CurveFunction() : _f(linearCurve<T>) {}
        CurveFunction(T (*f)(T)) : _f(f) {}
        CurveFunction(const CurveFunction& copy) : _f(copy._f) {}
        virtual ~CurveFunction() {}

        CurveFunction& operator=(const CurveFunction& copy) {
            _f = copy._f;
            return *this;
        }

        T operator()(T t) const {
            return _f(t);
        }

        static CurveFunction custom(T (*f)(T))  { return CurveFunction(f); }
        static CurveFunction linear()           { return CurveFunction(); }
        static CurveFunction easeIn()           { return CurveFunction(easeInQuad); }
        static CurveFunction easeOut()          { return CurveFunction(easeOutQuad); }
        static CurveFunction easeInOut()        { return CurveFunction(easeInOutQuad); }
        static CurveFunction elastic()          { return CurveFunction(easeOutElastic); }
        static CurveFunction bounce()           { return CurveFunction(easeOutBounce); }
    };

}

#endif /* GLSCurveFunction_h */
