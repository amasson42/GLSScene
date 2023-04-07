
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
        float fi = (float)i / count;
        velocity(0) = cos(3.141592 * fi);
        velocity(1) = sin(3.141592 * fi);
        velocity(2) = 1;

        // color
        color(0) = (cubeWidth / 2 + position(0)) / cubeWidth;
        color(1) = (cubeWidth / 2 + position(1)) / cubeWidth;
        color(2) = (cubeWidth / 2 + position(2)) / cubeWidth;

        // size
        size = 1;
        age = -1 * i / (float)count;
    }
}

kernel void particlesystem_update(global float *buffer, const unsigned int count, float dt, float3 gravityCenter) {
    unsigned int i = get_global_id(0);
    if (i < count) {
        int p_i = i * 11;

        // if (age < 0) {
        //     position(0) = gravityCenter.x;
        //     position(1) = gravityCenter.y;
        //     position(2) = gravityCenter.z;
        // }
        age += dt;

        position(0) += dt * velocity(0);
        position(1) += dt * velocity(1);
        position(2) += dt * velocity(2);

        float3 f3position;
        f3position.x = position(0);
        f3position.y = position(1);
        f3position.z = position(2);
        float distance = length(gravityCenter - f3position);
        float squaredDistance = distance * distance;
        // if (squaredDistance < 1)
        //     squaredDistance = 1;
        if (squaredDistance != 0) {
            float G = 8;
            velocity(0) += G * (gravityCenter.x - position(0)) / squaredDistance;
            velocity(1) += G * (gravityCenter.y - position(1)) / squaredDistance;
            velocity(2) += G * (gravityCenter.z - position(2)) / squaredDistance;
        }

        // color(0) = 0.4 + 0.2 * fabs(cos(age * 1.8));
        // color(1) = 0.1 + 0.1 * fabs(sin(age * 1.2));
        // color(2) = 0.7 + 0.3 * fabs(cos(age * 1.4)) - distance / 10;

        size = (sin(age) + 2.0) * 0.4 + 1.3;
        // size = age > 0.1 ? (3.0 / sqrt(age)) : 0;

        // if (age >= 10) {
        //     age -= 10.1;
        // }
    }
}
