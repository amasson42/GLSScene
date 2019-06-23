
kernel void particlesystem_init(global float *buffer, const unsigned int count) {
    unsigned int i = get_global_id(0);
    if (i < count) {
        int p_i = i * 11;
        global float3 *position = (global float3*)(buffer + p_i + 0);
        global float3 *velocity = (global float3*)(buffer + p_i + 3);
        global float3 *color = (global float3*)(buffer + p_i + 6);
        global float *size = (global float*)(buffer + p_i + 9);
        global float *age = (global float*)(buffer + p_i + 10);

        int cubeSize = rootn((float)count, 3);

        float cubeWidth = 0.5;

        // position
        position->x = (cubeWidth / cubeSize) * (i % cubeSize) - cubeWidth / 2;
        position->y = (cubeWidth / cubeSize) * ((i % (cubeSize * cubeSize)) / cubeSize) - cubeWidth / 2;
        position->z = (cubeWidth / cubeSize) * (i / (cubeSize * cubeSize)) - cubeWidth / 2;

        // velocity
        velocity->x = 0;
        velocity->y = 1;
        velocity->z = 0;

        // color
        color->x = (cubeWidth / 2 + position->x) / cubeWidth;
        color->y = (cubeWidth / 2 + position->y) / cubeWidth;
        color->z = (cubeWidth / 2 + position->z) / cubeWidth;

        // size
        *size = 1;
        *age = 0;
    }
}

kernel void particlesystem_update(global float *buffer, const unsigned int count, double dt, float3 gravityCenter) {
    unsigned int i = get_global_id(0);
    if (i < count) {
        int p_i = i * 11;
        global float3 *position = (global float3*)(buffer + p_i + 0);
        global float3 *velocity = (global float3*)(buffer + p_i + 3);
        global float3 *color = (global float3*)(buffer + p_i + 6);
        global float *size = (global float*)(buffer + p_i + 9);
        global float *age = (global float*)(buffer + p_i + 10);
        float fdt = (float)dt;

        position->x += fdt * velocity->x;
        position->y += fdt * velocity->y;
        position->z += fdt * velocity->z;
        *age += fdt;

        //float3 gravityCenter = (float3)(0, 1, 3);
        float squaredDistance = length(gravityCenter - *position);
        squaredDistance *= squaredDistance;
        if (squaredDistance < 1)
            squaredDistance = 1;
        if (squaredDistance != 0) {
            velocity->x += fdt * (gravityCenter.x - position->x) / squaredDistance;
            velocity->y += fdt * (gravityCenter.y - position->y) / squaredDistance;
            velocity->z += fdt * (gravityCenter.z - position->z) / squaredDistance;
        }

    }
}
