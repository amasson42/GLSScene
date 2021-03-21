
#define position(k) buffer[p_i + 0 + k]
#define velocity(k) buffer[p_i + 3 + k]
#define color(k)    buffer[p_i + 6 + k]
#define size        buffer[p_i + 9]
#define age         buffer[p_i + 10]

kernel void particlesystem_init(global float *buffer, const unsigned int count) {
    unsigned int i = get_global_id(0);
    if (i < count) {
        int p_i = i * 11;

        int cubeSize = rootn((float)count, 3);

        float cubeWidth = 0.5;

        // position
        position(0) = (cubeWidth / cubeSize) * (i % cubeSize) - cubeWidth / 2;
        position(1) = (cubeWidth / cubeSize) * ((i % (cubeSize * cubeSize)) / cubeSize) - cubeWidth / 2;
        position(2) = (cubeWidth / cubeSize) * (i / (cubeSize * cubeSize)) - cubeWidth / 2;

        // velocity
        velocity(0) = 0;
        velocity(1) = 1;
        velocity(2) = 0;

        // color
        color(0) = (cubeWidth / 2 + position(0)) / cubeWidth;
        color(1) = (cubeWidth / 2 + position(1)) / cubeWidth;
        color(2) = (cubeWidth / 2 + position(2)) / cubeWidth;

        // size
        size = 5;
        age = 0;
    }
}

kernel void particlesystem_update(global float *buffer, const unsigned int count, float dt, float3 gravityCenter) {
    unsigned int i = get_global_id(0);
    if (i < count) {
        int p_i = i * 11;

        float fdt = (float)dt;
        age += fdt;

        position(0) += fdt * velocity(0);
        position(1) += fdt * velocity(1);
        position(2) += fdt * velocity(2);
        size = (sin(age) + 1.0) * 5.0 + 5.0;

        //float3 gravityCenter = (float3)(0, 1, 3);
        float3 f3position;
        f3position.x = position(0);
        f3position.y = position(1);
        f3position.z = position(2);
        float distance = length(gravityCenter - f3position);
        float squaredDistance = distance * distance;
        if (squaredDistance < 1)
            squaredDistance = 1;
        if (squaredDistance != 0) {
            velocity(0) += fdt * (gravityCenter.x - position(0)) / squaredDistance;
            velocity(1) += fdt * (gravityCenter.y - position(1)) / squaredDistance;
            velocity(2) += fdt * (gravityCenter.z - position(2)) / squaredDistance;
        }

        //color(0) = distance;
        //color(1) = 0;
        //color(2) = distance / 2;

    }
}
