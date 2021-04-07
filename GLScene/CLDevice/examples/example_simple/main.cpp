
#include "CLDevice.hpp"
#include <iostream>

int getSequentialNumber(int n, bool display) {
	if (display)
		std::cout << n;
	if (n > 0) {
		int v = 0;
		while (n != 1) {
			n = n % 2 ? n * 3 + 1 : n / 2;
			if (display)
				std::cout << " -> " << n;
			v++;
		}
		if (display)
			std::cout << std::endl << "total: " << v << std::endl;
		return v;
	} else {
		if (display)
			std::cout << " -> 0" << std::endl << "total: 0" << std::endl;
		return 0;
	}
}

int main(int argc, char const *argv[]) {
	for (int i = 0; i < argc; i++)
		std::cout << argv[i] << std::endl;
	if (argc > 1) {
		int n = atoi(argv[1]);
		getSequentialNumber(n, true);
	} else {
		CLD::GPUDevice device;
		device.createContext(false);

		try {
		    std::cout << device << std::endl;
		} catch (std::exception& e) {
			std::cout << "Expeption: " << e.what() << std::endl;
		}

		const char *prgm_source1 = ""
		"kernel void initBuffer(global float *buffer, const unsigned int count) {\n"
		"	 unsigned int i = get_global_id(0);\n"
		"	 if (i < count)\n"
		"	 	 buffer[i] = i;\n"
		"	 else printf(\"init out... bye !\\n\");\n"
		"}\n";

		const char *prgm_source2 = ""
		"kernel void operation(global float *input, global float *output, const unsigned int count) {\n"
		"	unsigned int i = get_global_id(0);\n"
		"	if (i < count) {\n"
		"		int n = (int)input[i];\n"
		"		if (n <= 0) {output[i] = 0; return;}\n"
		"		int iter = 0;\n"
		"		while (n != 1) {\n"
		"			n = n % 2 ? n * 3 + 1 : n / 2;\n"
		"			iter++;\n"
		"		}\n"
		"		output[i] = iter;\n"
		"	}\n"
		"	 else printf(\"operation out... bye !\\n\");\n"
		"}\n"

		"kernel void printing(global float *buffer, unsigned int count) {\n"
		"	unsigned int i = get_global_id(0);\n"
		"	if (i < count)\n"
		"		printf(\"buff[%d]=%f\\n\", i, buffer[i]);\n"
		"	else printf(\"print out... bye !\\n\");\n"
		"}\n";

		const char *prgm_source3 = "\n"
        "static float random1f(float x) {\n"
        "    float v = (1.0 + (sin(x) * 293567.354)) / 2.0;\n"
        "    return (v - floor(v));\n"
        "}\n"
        "\n"
        "kernel void particlesystem_init(global float *buffer, const unsigned int count) {\n"
        "    unsigned int i = get_global_id(0);\n"
        "    if (i < count) {\n"
        "        int p_i = i * 9;\n"

        // position
        "        buffer[p_i + 0] = 0;\n"
        "        buffer[p_i + 1] = 0;\n"
        "        buffer[p_i + 2] = 0;\n"

        "        float velocity = random1f(i);\n"
        "        float angleX = random1f(i + 0.2);\n"
        "        float angleY = random1f(i + 0.3);\n"

        // velocity
        "        buffer[p_i + 3] = velocity * sin(angleY) * cos(angleX);\n"
        "        buffer[p_i + 4] = velocity * sin(angleX);\n"
        "        buffer[p_i + 5] = velocity * sin(angleY) * cos(angleX);\n"

        // color
        "        buffer[p_i + 6] = 1.0;\n"
        "        buffer[p_i + 7] = 0.8;\n"
        "        buffer[p_i + 8] = 0.8;\n"
        "    }\n"
        "    else printf(\"init out... bye !\\n\");\n"
        "}\n"
        "\n";

		CLD::Program prgm1;
		CLD::Program prgm2;
		try {
			prgm1 = device.createProgram(&prgm_source1, NULL);
			prgm2 = device.createProgram(&prgm_source2, NULL);
			device.createProgram(&prgm_source3);
		} catch (CLD::GPUDevice::BuildProgramException& e) {
			std::cout << "Exception: " << e.what() << std::endl;
		}
		std::cout << "Program compiled" << std::endl;

		CLD::Kernel initKernel = device.createKernel(prgm1, "initBuffer");
		CLD::Kernel operationKernel = device.createKernel(prgm2, "operation");
		CLD::Kernel printKernel = device.createKernel(prgm2, "printing");

		device.destroyProgram(prgm1);
		device.destroyProgram(prgm2);

		const size_t buffSize = 100;
		CLD::Buffer buff = device.createBuffer(sizeof(float) * buffSize);

		int cqi;
		CLD::CommandQueue queue = device.createCommandQueue(&cqi);
		std::cout << "cqi = " << cqi << std::endl;

		initKernel.setArgument(0, buff);
		initKernel.setArgument(1, (int)buffSize);

		queue.runNDRangeKernel(initKernel, buffSize);

		operationKernel.setArgument(0, buff);
		operationKernel.setArgument(1, buff);
		operationKernel.setArgument(2, (int) buffSize);

		queue.runNDRangeKernel(operationKernel, buffSize);

		float outputBuffer[buffSize];
		queue.readBuffer(buff, outputBuffer, sizeof(float) * buffSize);

		for (size_t i = 0; i < buffSize; i++) {
			std::cout << ", [" << i << "-" << outputBuffer[i] << "]";
		}
		std::cout << std::endl;

		printKernel.setArgument(0, buff);
		printKernel.setArgument(1, (int)buffSize);

		queue.runNDRangeKernel(printKernel, buffSize);

	}
	return (0);
}
