
#include "CLDevice.hpp"

namespace CLD {

    CommandQueue GPUDevice::createCommandQueue(int *index) throw(InvalidContextException) {
        Context *c = getContext();
        if (c == NULL)
            throw InvalidContextException();
        cl_command_queue commands = clCreateCommandQueue(c->context, _device_id, 0, NULL);
        if (commands) {
            std::vector<CommandQueue>& vcq(c->command_queues);

            CommandQueue cq = {c, commands};
            
            for (size_t i = 0; i < vcq.size(); i++) {
                if (vcq[i].command_queue == 0) {
                    vcq[i] = cq;
                    if (index)
                        *index = (int)i;
                    return cq;
                }
            }
            vcq.push_back(cq);
            if (index)
                *index = (int)vcq.size() - 1;
            return cq;
        }
        if (index)
            *index = -1;
        CommandQueue nullcq = {nullptr, 0};
        return nullcq;
    }

    void GPUDevice::destroyCommandQueue(int index) {
        if (index < 0)
            return ;
        Context *c = getContext();
        if (c && index < (int)c->command_queues.size()) {
            cl_command_queue cq = c->command_queues[index].command_queue;
            if (cq)
                clReleaseCommandQueue(cq);
            c->command_queues[index].command_queue = 0;
            c->clearNullTailCommandQueue();
        } else {
            std::cout << "destroy error to context " << c << std::endl;
        }
    }

    void GPUDevice::destroyCommandQueue(CommandQueue cq) {
        if (cq.context == nullptr || cq.command_queue == 0)
            return ;
        for (size_t i = 0; i < _contexes.size(); i++) {
            if (_contexes[i] == cq.context) {
                Context *c = _contexes[i];
                std::vector<CommandQueue>& vcq(c->command_queues);
                for (size_t j = 0; j < vcq.size(); j++) {
                    if (vcq[j].command_queue == cq.command_queue) {
                        clReleaseCommandQueue(cq.command_queue);
                        CommandQueue nullcq = {nullptr, 0};
                        vcq[j] = nullcq;
                        c->clearNullTailCommandQueue();
                        return ;
                    }
                }
                return ;
            }
        }
    }

    CommandQueue *GPUDevice::commandQueue(int index) {
        if (index >= 0) {
            Context *c = getContext();
            if (c && index < (int)c->command_queues.size())
                return &c->command_queues[index];
        }
        return nullptr;
    }

}
