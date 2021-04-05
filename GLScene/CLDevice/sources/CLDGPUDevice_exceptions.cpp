
#include "CLDevice.hpp"
#include <fstream>

std::string file_to_string(const std::string file_path) {
    std::ifstream file(file_path, std::ios::binary);
    std::string fileStr;

    std::istreambuf_iterator<char> inputIt(file), emptyInputIt;
    std::back_insert_iterator<std::string> stringInsert(fileStr);

    copy(inputIt, emptyInputIt, stringInsert);

    return fileStr;
}

namespace CLD {

    DeviceConstants::DeviceConstants() {

    }

    DeviceConstants::DeviceConstants(cl_device_id device_id) {
        clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_int), &maxWorkItemDimensions, NULL);
        clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL);
    }

    const char* GPUDevice::LoadPlateformException::what() const throw() {
        return "could not load any plateform";
    }

    const char* GPUDevice::LoadDeviceException::what() const throw() {
        return "could not load any device";
    }

    GPUDevice::BuildProgramException::BuildProgramException(cl_program p, cl_device_id id) throw()
    {
        _buildInfo = "Program build error:\n";
        size_t error_size;
        clGetProgramBuildInfo(p, id, CL_PROGRAM_BUILD_LOG, 0, NULL, &error_size);
        char *error_buffer = new char[error_size];
        if (error_buffer) {
            clGetProgramBuildInfo(p, id, CL_PROGRAM_BUILD_LOG, error_size, error_buffer, &error_size);
            _buildInfo += error_buffer;
            delete[] error_buffer;
        } else {
            _buildInfo += "-you're so bad the error is too long to display-\n";
        }
    }

    GPUDevice::BuildProgramException::~BuildProgramException() throw() {

    }

    const char* GPUDevice::BuildProgramException::what() const throw() {
        return _buildInfo.c_str();
    }

    const char* GPUDevice::InvalidContextException::what() const throw() {
        return "There is no valid current context";
    }

}
