// canyon.cl

#define BLOCK_AIR 0x00000000
#define BLOCK_BEDROCK 0x00010000
#define BLOCK_STONE 0x01010000
#define BLOCK_DIRT 0x02010000
#define BLOCK_GRASS 0x03010000
#define BLOCK_SAND 0x04010000
#define BLOCK_GRAVEL 0x05010000
#define BLOCK_WATER 0x07010000
#define BLOCK_WATER_SURFACE 0x07020000
#define BLOCK_GRASS_BROWN 0x10010000
#define BLOCK_WOOD 0x11010000
#define BLOCK_LEAFS 0x12010000
#define BLOCK_WOOD_PLANKS 0x13010000
#define BLOCK_BRICKS 0x14010000
#define BLOCK_COBBLESTONE 0x15010000
#define BLOCK_ICE 0x20010000
#define BLOCK_ICE_BROKEN 0x21010000
#define BLOCK_OBSIDIAN 0x24010000
#define BLOCK_GRASS_PURPLE 0x30010000
#define BLOCK_GOLD 0x34010000
#define BLOCK_TNT 0x35010000
#define BLOCK_WOOD_FENCE 0x11080000
#define WATER_LEVEL 64

double grad(int hash, double x, double y, double z);
double noise(__global int* p, double x, double y, double z);
double lerp(double t, double a, double b);

double grad(int hash, double x, double y, double z) {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

double lerp(double t, double a, double b) {
    return a + t * (b - a);
}

double noise(__global int* p, double x, double y, double z) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    double u = x;
    double v = y;
    double w = z;

    int A = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;

    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;

    return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),
                                     grad(p[BA  ], x-1, y  , z   )),
                             lerp(u, grad(p[AB  ], x  , y-1, z   ),
                                     grad(p[BB  ], x-1, y-1, z   ))),
                     lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),
                                     grad(p[BA+1], x-1, y  , z-1 )),
                             lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                     grad(p[BB+1], x-1, y-1, z-1 ))));
}

int3 getLocalPosition(int i, const int chunkSize, const int bigChunkWidth) {
    const int chunkSize_2 = chunkSize * chunkSize;
    const int chunkSize_3 = chunkSize_2 * chunkSize;
    const int bigChunkWidth_2 = bigChunkWidth * bigChunkWidth;
    const int iByChunkSize_3 = i / chunkSize_3;
    const int iModChunkSize_3 = i % chunkSize_3;
    return (int3)(
        ((iByChunkSize_3 % bigChunkWidth_2) / bigChunkWidth) * chunkSize + (iModChunkSize_3 % chunkSize),
        (iByChunkSize_3 / bigChunkWidth_2) * chunkSize + ((iModChunkSize_3 % chunkSize_2) / chunkSize),
        ((iByChunkSize_3 % bigChunkWidth_2) % bigChunkWidth) * chunkSize + (iModChunkSize_3 / chunkSize_2)
    );
}

float3 getWorldPosition(const int3 localPosition, const int2 bigChunkPos, const int bigChunkFullWidth) {
    return (float3)(localPosition.x + bigChunkPos.x * bigChunkFullWidth,
                localPosition.y,
                localPosition.z + bigChunkPos.y * bigChunkFullWidth);
}

int calculBlockAt(__global int* ppm, float3 wpos);

float biomesIntensity_grass(__global int* ppm, float3 wpos) {
    return noise(ppm, wpos.x * 0.01 + 265.42, 0.75, wpos.z * 0.01 + 23.41);
}

#define BIOMES_COUNT 6

int calculBlockAt(__global int* ppm, float3 wpos) {
    if (wpos.y > 1)
        return BLOCK_AIR;
    if (biomesIntensity_grass(ppm, wpos) > 0)
        return BLOCK_GRASS;
    else
        return BLOCK_DIRT;
    float biomesIntensity[6];

}

kernel void generateBigChunk(__global int* ppm, __global int* blocks, const int2 bigChunkPos, const int chunkSize, const int bigChunkWidth) {
    size_t i = get_global_id(0);
    int3 localPosition = getLocalPosition(i, chunkSize, bigChunkWidth);
    float3 wpos = getWorldPosition(localPosition, bigChunkPos, chunkSize * bigChunkWidth);
    int block = BLOCK_AIR;

	float height = noise(ppm, wpos.x * 0.01, 0, wpos.z * 0.01) * 400;


	if (height >= wpos.y) {
		// float surface = noise(ppm, wpos.x * 0.02, 0, wpos.z * 0.02) * 100;

    	height = clamp(height, 0.0f, 30.0f);
    	// height = clamp(height, 0.0f, surface);
	}


    if (wpos.y < 3) {
        block = BLOCK_SAND;
        block = BLOCK_BEDROCK;
        block = BLOCK_WATER;
    } 
    else if (height >= wpos.y) {
        block = BLOCK_TNT;
        block = BLOCK_SAND;
        block = BLOCK_COBBLESTONE;
        block = BLOCK_DIRT;
    }
    // else {
    //     block = BLOCK_STONE;
    // }

    // block = calculBlockAt(ppm, wpos);

    blocks[i] = block;
}