
#include "CLDevice.hpp"

namespace CLD {

    int GPUDevice::createContext(bool shareGL) {

        cl_context clcontext;
        if (shareGL) {

#ifdef __APPLE__

            CGLContextObj currentGLContext = CGLGetCurrentContext();
            CGLShareGroupObj shareGroup = CGLGetShareGroup(currentGLContext);

            gcl_gl_set_sharegroup(shareGroup);

            cl_context_properties cps[] = {
                CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)shareGroup,
                CL_CONTEXT_PLATFORM, (cl_context_properties)_platform,
                0
            };

            clcontext = clCreateContext(cps, 1, &_device_id, NULL, NULL, NULL);
            if (!clcontext) {
                return -1;
            }

#elif defined _WIN32

            cl_context_properties cps[] =
            {
                CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
                CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
                CL_CONTEXT_PLATFORM, ((cl_context_properties)_platform),
                0
            };

            clcontext = clCreateContext(cps, 1, &_device_id, NULL, NULL, NULL);
            if (!clcontext) {
                return -1;
            }

#else

            clcontext = clCreateContext(NULL, 1, &_device_id, NULL, NULL, NULL);
            if (!clcontext) {
                return -1;
            }

#endif

        } else {
            clcontext = clCreateContext(0, 1, &_device_id, NULL, NULL, NULL);
            if (!clcontext) {
                return -1;
            }
        }
        Context *context = new Context(this, clcontext);
        if (context == nullptr) {
            clReleaseContext(clcontext);
            return -1;
        }
        for (size_t i = 0; i < _contexes.size(); i++)
            if (_contexes[i] == nullptr) {
                _contexes[i] = context;
                _currentContext = static_cast<int>(i);
                return (int)i;
            }
        _contexes.push_back(context);
        _currentContext = static_cast<int>(_contexes.size()) - 1;
        return _currentContext;
    }

    void GPUDevice::useContextIndex(int i) {
        if (i < (int)_contexes.size())
            _currentContext = i;
        else
            _currentContext = -1;
    }

    Context* GPUDevice::getContext() {
        if (_currentContext >= 0)
            return _contexes[_currentContext];
        else
            return nullptr;
    }

    int GPUDevice::contextCount() const {
        return (int)_contexes.size();
    }

    void GPUDevice::destroyContext() {
        Context *c = getContext();
        if (c) {
            delete c;
            _contexes[_currentContext] = nullptr;
        }
        _currentContext = -1;
        while (_contexes.size() > 0 && _contexes.back() == nullptr)
            _contexes.pop_back();
    }

    void GPUDevice::destroyContext(int i) {
        int tmp = _currentContext;
        useContextIndex(i);
        destroyContext();
        useContextIndex(tmp);
    }

}
