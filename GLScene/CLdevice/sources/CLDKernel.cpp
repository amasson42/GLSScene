
#include "CLDevice.hpp"

namespace CLD {

    int Kernel::setArgument(int index, size_t size, void *argument) {
        return clSetKernelArg(kernel, index, size, argument);
    }

    int Kernel::setArgument(int index, char c) {
        return setArgument(index, sizeof(char), &c);
    }

    int Kernel::setArgument(int index, int i) {
        return setArgument(index, sizeof(int), &i);
    }

    int Kernel::setArgument(int index, unsigned int u) {
        return setArgument(index, sizeof(unsigned int), &u);
    }

    int Kernel::setArgument(int index, float f) {
        return setArgument(index, sizeof(float), &f);
    }

    int Kernel::setArgument(int index, double lf) {
        return setArgument(index, sizeof(double), &lf);
    }

    int Kernel::setArgument(int index, Buffer& buffer) {
        return setArgument(index, sizeof(cl_mem), &buffer.buffer);
    }

    int Kernel::setArgument(int index, cl_float3 vec3) {
        return setArgument(index, sizeof(cl_float3), &vec3);
    }

    int Kernel::setArgument(int index, cl_int2 vec2) {
        return setArgument(index, sizeof(cl_int2), &vec2);
    }

    int Kernel::setArgument(int index, float x, float y, float z) {
        float v[3] = {x, y, z};
        return setArgument(index, sizeof(v), v);
    }

}
