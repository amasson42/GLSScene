
#include "CLDevice.hpp"

namespace CLD {

    Kernel GPUDevice::createKernel(int programIndex, const std::string& functionName, int *index) {
        Context *c = getContext();
        if (c == NULL)
            throw InvalidContextException();
        if (programIndex >= 0 && (size_t)programIndex < c->programs.size())
            return createKernel(c->programs[programIndex], functionName, index);
        if (index)
            *index = -1;
        Kernel nullk = {nullptr, 0, "", 0};
        return nullk;
    }

    Kernel GPUDevice::createKernel(Program program, const std::string& functionName, int *index) {
        Context *c = program.context;
        if (c == NULL)
            throw InvalidContextException();
        int err;
        cl_kernel kernel = clCreateKernel(program.program, functionName.c_str(), &err);
        if (kernel && err == CL_SUCCESS) {
            std::vector<Kernel>& vk(c->kernels);

            size_t wgs;
            clGetKernelWorkGroupInfo(kernel, _device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(wgs), &wgs, NULL);
            if (err != CL_SUCCESS)
                wgs = 0;
            Kernel k = {c, kernel, functionName, wgs};

            for (size_t i = 0; i < vk.size(); i++) {
                if (vk[i].kernel == 0) {
                    vk[i] = k;
                    if (index)
                        *index = (int)i;
                    return k;
                }
            }
            vk.push_back(k);
            if (index)
                *index = (int)vk.size() - 1;
            return k;
        }
        if (index)
            *index = -1;
        Kernel nullk = {nullptr, 0, "", 0};
        return nullk;
    }

    void GPUDevice::destroyKernel(int index) {
        if (index < 0)
            return ;
        Context *c = getContext();
        if (c && index < (int)c->kernels.size()) {
            cl_kernel k = c->kernels[index].kernel;
            if (k)
                clReleaseKernel(k);
            Kernel nullk = {nullptr, 0, "", 0};
            c->kernels[index] = nullk;
            c->clearNullTailKernel();
        }
    }

    void GPUDevice::destroyKernel(Kernel k) {
        if (k.context == nullptr || k.kernel == 0)
            return ;
        for (size_t i = 0; i < _contexes.size(); i++) {
            if (_contexes[i] == k.context) {
                Context *c = _contexes[i];
                std::vector<Kernel>& vk(c->kernels);
                for (size_t j = 0; j < vk.size(); j++) {
                    if (vk[j].kernel == k.kernel) {
                        clReleaseKernel(k.kernel);
                        Kernel nullk = {nullptr, 0, "", 0};
                        vk[j] = nullk;
                        c->clearNullTailKernel();
                        return ;
                    }
                }
                return ;
            }
        }
    }

    Kernel *GPUDevice::kernel(int index) {
        if (index >= 0) {
            Context *c = getContext();
            if (c && index < (int)c->kernels.size())
                return &c->kernels[index];
        }
        return nullptr;
    }

}