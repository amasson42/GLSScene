
#ifndef DEF_CLDEVICE_HPP
# define DEF_CLDEVICE_HPP

# ifdef __APPLE__
#  define GL_SILENCE_DEPRECATION
#  include <OpenGL/gl3.h>
#  define CL_SILENCE_DEPRECATION
#  include <OpenCL/OpenCL.h>
#  include <OpenGL/CGLDevice.h>
#  include <OpenGL/CGLCurrent.h>
#  include <OpenCL/cl_gl.h>
#  include <OpenCL/cl_gl_ext.h>
# elif defined _WIN32
#  include <windows.h>
#  include <glad/glad.h>
#  include <CL/cl.h>
#  include <CL/cl_gl.h>
# else
#  include <cl/cl.h>
#  include <cl/cl_gl.h>
#  include <cl/cl_gl_ext.h>
# endif

# include <vector>
# include <string>
# include <iostream>

namespace CLD {

    struct Context;

    struct Buffer {
        Context *context;
        cl_mem buffer;
        size_t size;
        bool read;
        bool write;
    };

    struct Program {
        Context *context;
        cl_program program;
    };

    struct Kernel {
        Context *context;
        cl_kernel kernel;
        std::string function;
        size_t workGroupSize;

        int setArgument(int index, size_t size, void *argument);
        int setArgument(int index, char c);
        int setArgument(int index, int i);
        int setArgument(int index, unsigned int u);
        int setArgument(int index, float f);
        int setArgument(int index, double lf);
        int setArgument(int index, Buffer& buffer);
        int setArgument(int index, cl_float3 buffer);
        int setArgument(int index, cl_int2 buffer);
        int setArgument(int index, float x, float y, float z);
    };

    struct CommandQueue {
        Context *context;
        cl_command_queue command_queue;

        int finish();
        int writeBuffer(Buffer& buffer, void *data, size_t size);
        int readBuffer(Buffer& buffer, void *data, size_t size);
        int task(Kernel& kernel);
        int runNDRangeKernel(Kernel& kernel, size_t count);
        int acquireGLObject(Buffer& buffer);
        int releaseGLObject(Buffer& buffer);
    };

    class GPUDevice;
    struct Context {
        const GPUDevice *gpuDevice;
        cl_context context;
        std::vector<CommandQueue> command_queues;
        std::vector<Program> programs;
        std::vector<Kernel> kernels;
        std::vector<Buffer> buffers;

        Context(GPUDevice *device, cl_context c);
        ~Context();

        void clearNullTailCommandQueue();
        void clearNullTailProgram();
        void clearNullTailKernel();
        void clearNullTailBuffer();
    };

    struct DeviceConstants {
        cl_int maxWorkItemDimensions;
        size_t maxWorkGroupSize;

        DeviceConstants();
        DeviceConstants(cl_device_id device_id);
    };

    class GPUDevice {

        /*
        ** this current version only allow one platform and one device
        */

        cl_platform_id _platform;
        cl_device_id _device_id;

        /*
        ** all contexes are saved in a vector and we reference them with
        ** their indices. If one get destroyed the pointer is set to null
        ** and is reused only if it was the last index
        */

        std::vector<Context*> _contexes;
        int _currentContext;

        DeviceConstants _constants;

        GPUDevice(const GPUDevice& copy);
        GPUDevice& operator=(const GPUDevice);

    public:

        class LoadPlateformException: std::exception {
            public: const char* what() const throw();
        };

        class LoadDeviceException: std::exception {
            public: const char* what() const throw();
        };

        class BuildProgramException: std::exception {
            std::string _buildInfo;
            public:
            BuildProgramException(cl_program p, cl_device_id id);
            const char* what() const throw();
        };

        class InvalidContextException: std::exception {
            public: const char* what() const throw();
        };

        GPUDevice(); // init the platform and device
        ~GPUDevice(); // destroy all contexes and release device and platform

        cl_platform_id platform_id() const;
        cl_device_id device_id() const;
    
        std::string getStringInfo(cl_device_info info) const;
        cl_uint getUIntInfo(cl_device_info info) const;
        const DeviceConstants& constants() const;

        /*
        ** every function that create an OpenCL object and store it in a vector will
        ** first seek if there is a null pointer in the vector then use it or
        ** push back if there is not
        */

        /*
        ** For any use of OpenCL we need a context.
        ** We can create one with the function createContext, if we want multiple context
        ** we must stock the index returned and switch current context with the function
        ** useContextIndex with the returned index from the creation function
        */

        int createContext(bool shareGL = true); // create a context with the device and platform then return it current index
        void useContextIndex(int i); // change the index of the current context
        Context* getContext(); // return a pointer to the indexed context
        int contextCount() const; // return the index of the highest context
        void destroyContext(); // delete the current context, set the pointer to null and pop every last null pointer of the vector
        void destroyContext(int i);

        // Context using

        /* What openCL can do is using a command queue to store the instruction
            we're going to send to the device
        */
		CommandQueue createCommandQueue(int* index = NULL);
        void destroyCommandQueue(int index);
        void destroyCommandQueue(CommandQueue cq);
        CommandQueue *commandQueue(int index);

        Program createProgram(const char** sources, int *index = NULL);
        void destroyProgram(int index);
        void destroyProgram(Program p);
        Program *program(int index);

        Kernel createKernel(int programIndex, const std::string& functionName, int *index = NULL);
        Kernel createKernel(Program program, const std::string& functionName, int *index = NULL);
        void destroyKernel(int index);
        void destroyKernel(Kernel k);
        Kernel *kernel(int index);

        Buffer createFlagBuffer(size_t size, cl_mem_flags flags, int *index = NULL);
        Buffer createFlagBuffer(size_t size, cl_mem_flags flags, void* data, int *index = NULL);
        Buffer createGLBuffer(unsigned int glbuffer, int *index = NULL);
        Buffer createBuffer(size_t size, int *index = NULL);
        Buffer createWriteBuffer(size_t size, int *index = NULL);
        Buffer createReadBuffer(size_t size, int *index = NULL);
        void destroyBuffer(int index);
        void destroyBuffer(Buffer b);
        Buffer *buffer(int index);

        friend std::ostream& operator<<(std::ostream& out, const GPUDevice& device);
    };

    std::ostream& operator<<(std::ostream& out, const Buffer& buffer);
}

#endif
