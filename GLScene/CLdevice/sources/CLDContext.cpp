
#include "CLDevice.hpp"

namespace CLD {

    Context::Context(GPUDevice *device, cl_context c) :
    gpuDevice(device),
    context(c)
    {}

    Context::~Context() {
        if (context) {
            for (size_t i = 0; i < command_queues.size(); i++)
                clReleaseCommandQueue(command_queues[i].command_queue);
            for (size_t i = 0; i < programs.size(); i++)
                clReleaseProgram(programs[i].program);
            for (size_t i = 0; i < kernels.size(); i++)
                clReleaseKernel(kernels[i].kernel);
            for (size_t i = 0; i < buffers.size(); i++)
                clReleaseMemObject(buffers[i].buffer);
            clReleaseContext(context);
        }
    }

    void Context::clearNullTailCommandQueue() {
        while (command_queues.size() > 0
            && command_queues.back().command_queue == 0) {
            command_queues.pop_back();
        }
    }

    void Context::clearNullTailProgram() {
        while (programs.size() > 0
            && programs.back().program == 0) {
            programs.pop_back();
        }
    }

    void Context::clearNullTailKernel() {
        while (kernels.size() > 0
            && kernels.back().kernel == 0) {
            kernels.pop_back();
        }
    }

    void Context::clearNullTailBuffer() {
        while (buffers.size() > 0
            && buffers.back().buffer == 0) {
            buffers.pop_back();
        }
    }

}