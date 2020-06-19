
#include "CLDevice.hpp"

namespace CLD {

    int CommandQueue::finish() {
        return clFinish(command_queue);
    }

    int CommandQueue::writeBuffer(Buffer& buffer, void *data, size_t size) {
        return clEnqueueWriteBuffer(command_queue, buffer.buffer, CL_TRUE, 0, size, data, 0, NULL, NULL);
    }

    int CommandQueue::readBuffer(Buffer& buffer, void *data, size_t size) {
        return clEnqueueReadBuffer(command_queue, buffer.buffer, CL_TRUE, 0, size, data, 0, NULL, NULL);
    }

    int CommandQueue::task(Kernel& kernel) {
        return clEnqueueTask(command_queue, kernel.kernel, 0, NULL, NULL);
    }

    int CommandQueue::runNDRangeKernel(Kernel& kernel, size_t count) {
        return clEnqueueNDRangeKernel(command_queue, kernel.kernel, 1,
            NULL, &count, NULL, 0, NULL, NULL);
    }

    int CommandQueue::acquireGLObject(Buffer& glBuffer) {
        return clEnqueueAcquireGLObjects(command_queue, 1, &glBuffer.buffer, 0, NULL, NULL);
    }

    int CommandQueue::releaseGLObject(Buffer& glBuffer) {
        return clEnqueueReleaseGLObjects(command_queue, 1, &glBuffer.buffer, 0, NULL, NULL);
    }

}