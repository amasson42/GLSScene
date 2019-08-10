
#include "CLDevice.hpp"

namespace CLD {

	Buffer GPUDevice::createFlagBuffer(size_t size, cl_mem_flags flags, void* data, int *index) {
        Context *c = getContext();
        if (c == NULL)
            throw InvalidContextException();
        cl_mem buffer = clCreateBuffer(c->context, flags, size, data, NULL);
        if (buffer) {
            std::vector<Buffer>& vb(c->buffers);

            Buffer b = {c, buffer, size,
                flags & CL_MEM_READ_ONLY || flags & CL_MEM_READ_WRITE,
                flags & CL_MEM_WRITE_ONLY || flags & CL_MEM_READ_WRITE
            };

            for (size_t i = 0; i < vb.size(); i++) {
                if (vb[i].buffer == 0) {
                    vb[i] = b;
                    if (index)
                        *index = (int)i;
                    return b;
                }
            }
            vb.push_back(b);
            if (index)
                *index = (int)vb.size() - 1;
            return b;
        }
        if (index)
            *index = -1;
        Buffer nullb = {nullptr, 0, 0, 0, 0};
        return nullb;
	}

    Buffer GPUDevice::createFlagBuffer(size_t size, cl_mem_flags flags, int *index) {
		return createFlagBuffer(size, flags, NULL, index);
    }



    Buffer GPUDevice::createGLBuffer(unsigned int glbuffer, int *index) {
        Context *c = getContext();
        if (c == NULL)
            throw InvalidContextException();
        cl_mem buffer = clCreateFromGLBuffer(c->context, CL_MEM_READ_WRITE, glbuffer, NULL);
        if (buffer) {
            std::vector<Buffer>& vb(c->buffers);

            int size;
            glBindBuffer(GL_ARRAY_BUFFER, glbuffer);
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            Buffer b = {c, buffer, static_cast<size_t>(size), true, true};

            for (size_t i = 0; i < vb.size(); i++) {
                if (vb[i].buffer == 0) {
                    vb[i] = b;
                    if (index)
                        *index = (int)i;
                    return b;
                }
            }
            vb.push_back(b);
            if (index)
                *index = (int)vb.size() - 1;
            return b;
        }
        if (index)
            *index = -1;
        Buffer nullb = {nullptr, 0, 0, 0, 0};
        return nullb;
    }

    Buffer GPUDevice::createBuffer(size_t size, int *index) {
        return createFlagBuffer(size, CL_MEM_READ_WRITE, index);
    }

    Buffer GPUDevice::createWriteBuffer(size_t size, int *index) {
        return createFlagBuffer(size, CL_MEM_WRITE_ONLY, index);
    }

    Buffer GPUDevice::createReadBuffer(size_t size, int *index) {
        return createFlagBuffer(size, CL_MEM_READ_ONLY, index);
    }

    void GPUDevice::destroyBuffer(int index) {
        if (index < 0)
            return ;
        Context *c = getContext();
        if (c && index < (int)c->buffers.size()) {
            cl_mem b = c->buffers[index].buffer;
            if (b)
                clReleaseMemObject(b);
            Buffer nullb = {nullptr, 0, 0, 0, 0};
            c->buffers[index] = nullb;
            c->clearNullTailBuffer();
        }
    }

    void GPUDevice::destroyBuffer(Buffer b) {
        if (b.context == nullptr || b.buffer == 0)
            return ;
        for (size_t i = 0; i < _contexes.size(); i++) {
            if (_contexes[i] == b.context) {
                Context *c = _contexes[i];
                std::vector<Buffer>& vb(c->buffers);
                for (size_t j = 0; j < vb.size(); j++) {
                    if (vb[j].buffer == b.buffer) {
                        clReleaseMemObject(b.buffer);
                        Buffer nullb = {nullptr, 0, 0, 0, 0};
                        vb[j] = nullb;
                        c->clearNullTailBuffer();
                        return ;
                    }
                }
                return ;
            }
        }
    }

    Buffer *GPUDevice::buffer(int index) {
        if (index >= 0) {
            Context *c = getContext();
            if (c && index < (int)c->buffers.size())
                return &c->buffers[index];
        }
        return nullptr;
    }

    std::ostream& operator<<(std::ostream& out, const Buffer& buffer) {
        if (buffer.buffer == 0) {
            out << "(null)";
        } else {
            out << buffer.size << ' ' << (buffer.read ? "r" : "-") << (buffer.write ? "w" : "-");
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const Context& context) {
        out << context.context << "; ";
        int count;

        count = 0;
        for (size_t i = 0; i < context.command_queues.size(); i++)
            if (context.command_queues[i].command_queue != 0)
                count++;
        out << "cq(" << count << '/' << context.command_queues.size() << "); ";

        count = 0;
        for (size_t i = 0; i < context.programs.size(); i++)
            if (context.programs[i].program != 0)
                count++;
        out << "p(" << count << '/' << context.programs.size() << "); ";

        count = 0;
        for (size_t i = 0; i < context.kernels.size(); i++)
            if (context.kernels[i].kernel != 0)
                count++;
        out << "k(" << count << '/' << context.kernels.size() << "); ";

        count = 0;
        for (size_t i = 0; i < context.buffers.size(); i++)
            if (context.buffers[i].buffer != 0)
                count++;
        out << "b(" << count << '/' << context.buffers.size() << "); ";

        return out;
    }

    std::ostream& operator<<(std::ostream& out, const GPUDevice& device) {
        out << device.getStringInfo(CL_DEVICE_NAME);
        out << " - " << device.getStringInfo(CL_DEVICE_VERSION);
        out << " - " << device.contextCount() << " contexes";
        for (int i = 0; i < device.contextCount(); i++) {
            out << " - {" << *device._contexes[i] << "}";
        }
        return out;
    }

}