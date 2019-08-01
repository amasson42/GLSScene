
#include "CLDevice.hpp"

namespace CLD {

    GPUDevice::GPUDevice() :
    _contexes(), _currentContext(-1),
    _constants()
    {
        int err;

        err = clGetPlatformIDs(1, &_platform, NULL);
        if (err != CL_SUCCESS) {
            throw LoadPlateformException();
        }

        // TODO FIXME : Find the right one

        cl_uint devices_count;
        err = clGetDeviceIDs(_platform, CL_DEVICE_TYPE_GPU, 0, NULL, &devices_count);
        if (err != CL_SUCCESS || devices_count == 0)
            throw LoadDeviceException();
        std::cout << "devices_count: " << devices_count << std::endl;

        cl_device_id *device_ids = new cl_device_id[devices_count];

        err = clGetDeviceIDs(_platform, CL_DEVICE_TYPE_GPU, devices_count, device_ids, &devices_count);
        if (err != CL_SUCCESS)
            throw LoadDeviceException();

        for (cl_uint i = 0; i < devices_count; i++) {
            _device_id = device_ids[i];
            std::cout << "device[" << i << "] {" << std::endl;
            std::cout << "  name : " << getStringInfo(CL_DEVICE_NAME) << std::endl;
            std::cout << "  version : " << getStringInfo(CL_DEVICE_VERSION) << std::endl;
            std::cout << "  driver_version : " << getStringInfo(CL_DRIVER_VERSION) << std::endl;
            std::cout << "  openCL_C_version : " << getStringInfo(CL_DEVICE_OPENCL_C_VERSION) << std::endl;
            std::cout << "  max_compute_units : " << getUIntInfo(CL_DEVICE_MAX_COMPUTE_UNITS) << std::endl;
            std::cout << "}" << std::endl;
        }

        delete[] device_ids;

        _constants = DeviceConstants(_device_id);

    }

    GPUDevice::~GPUDevice() {
        for (size_t i = 0; i < _contexes.size(); i++) {
            if (_contexes[i])
                delete _contexes[i];
        }
    }

    cl_platform_id GPUDevice::platform_id() const {
        return _platform;
    }

    cl_device_id GPUDevice::device_id() const {
        return _device_id;
    }

    std::string GPUDevice::getStringInfo(cl_device_info info) const {
        size_t infoSize;
        char *infoStr;

        clGetDeviceInfo(_device_id, info, 0, NULL, &infoSize);
        infoStr = new char[infoSize];
        clGetDeviceInfo(_device_id, info, infoSize, infoStr, NULL);
        std::string str(infoStr);
        delete[] infoStr;
        return str;
    }

    cl_uint GPUDevice::getUIntInfo(cl_device_info info) const {
        cl_uint value;
        clGetDeviceInfo(_device_id, info, sizeof(value), &value, NULL);
        return value;
    }

    const DeviceConstants& GPUDevice::constants() const {
        return _constants;
    }

}
