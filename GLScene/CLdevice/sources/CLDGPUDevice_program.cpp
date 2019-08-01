
#include "CLDevice.hpp"

namespace CLD {

    Program GPUDevice::createProgram(const char **sources, int *index) throw(BuildProgramException, InvalidContextException) {
        Context *c = getContext();
        if (c == NULL)
            throw InvalidContextException();
        cl_program program = clCreateProgramWithSource(c->context, 1, sources, 0, NULL);
        if (program) {
            int err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
            if (err != CL_SUCCESS) {
                BuildProgramException e(program, _device_id);
                clReleaseProgram(program);
                throw e;
            } else {
                std::vector<Program>& vp(c->programs);

                Program p = {c, program};

                for (size_t i = 0; i < vp.size(); i++) {
                    if (vp[i].program == 0) {
                        vp[i] = p;
                        if (index)
                            *index = i;
                        return p;
                    }
                }
                vp.push_back(p);
                if (index)
                    *index = (int)vp.size() - 1;
                return p;
            }
        }
        if (index)
            *index = -1;
        Program nullp = {nullptr, 0};
        return nullp;
    }

    void GPUDevice::destroyProgram(int index) {
        if (index < 0)
            return ;
        Context *c = getContext();
        if (c && index < (int)c->programs.size()) {
            cl_program p = c->programs[index].program;
            if (p)
                clReleaseProgram(p);
            Program nullp = {nullptr, 0};
            c->programs[index] = nullp;
            c->clearNullTailProgram();
        }
    }

    void GPUDevice::destroyProgram(Program p) {
        if (p.context == nullptr || p.program == 0)
            return ;
        for (size_t i = 0; i < _contexes.size(); i++) {
            if (_contexes[i] == p.context) {
                Context *c = _contexes[i];
                std::vector<Program>& vp(c->programs);
                for (size_t j = 0; j < vp.size(); j++) {
                    if (vp[j].program == p.program) {
                        clReleaseProgram(p.program);
                        Program nullp = {nullptr, 0};
                        vp[j] = nullp;
                        c->clearNullTailProgram();
                        return ;
                    }
                }
                return ;
            }
        }
    }

    Program *GPUDevice::program(int index) {
        if (index >= 0) {
            Context *c = getContext();
            if (c && index < (int)c->programs.size())
                return &c->programs[index];
        }
        return nullptr;
    }

}
