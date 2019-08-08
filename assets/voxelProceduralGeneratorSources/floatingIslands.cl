
// floatingIslands.cl

#define BLOCK_AIR 0
#define BLOCK_BEDROCK 1
#define BLOCK_STONE 2
#define BLOCK_DIRT 3
#define BLOCK_GRASS 4
#define BLOCK_SAND 5
#define BLOCK_GRAVEL 6
#define BLOCK_GRASS_BROWN 11
#define BLOCK_WOOD 12
#define BLOCK_LEAFS 13
#define BLOCK_WOOD_PLANKS 14
#define BLOCK_BRICKS 15
#define BLOCK_COBBLESTONE 16
#define BLOCK_ICE 21
#define BLOCK_ICE_BROKEN 22
#define BLOCK_OBSIDIAN 25
#define BLOCK_GRASS_PURPLE 31
#define BLOCK_GOLD 35
#define BLOCK_TNT 92

double grad(int hash, double x, double y, double z);
double lerp(double t, double a, double b);
double fade(double t);

double grad(int hash, double x, double y, double z) {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

double lerp(double t, double a, double b) {
    return a + t * (b - a);
}

double fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double noise(__global int* p, double x, double y, double z) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

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

int3 getWorldPosition(const int3 localPosition, const int2 bigChunkPos, const int bigChunkFullWidth) {
	return (int3)(localPosition.x + bigChunkPos.x * bigChunkFullWidth,
				localPosition.y,
				localPosition.z + bigChunkPos.y * bigChunkFullWidth);
}

double noise(__global int* p, double x, double y, double z);

kernel void generateBigChunk(__global int* ppm, __global int* blocks, const int2 bigChunkPos, const int chunkSize, const int bigChunkWidth) {
	size_t i = get_global_id(0);
	int3 localPosition = getLocalPosition(i, chunkSize, bigChunkWidth);
	int3 wpos = getWorldPosition(localPosition, bigChunkPos, chunkSize * bigChunkWidth);
    int block = BLOCK_AIR;

    float islandIntensity = pow(noise(ppm, wpos.x * 0.02, 0.5, wpos.z * 0.02) + noise(ppm, wpos.x * 0.02, 0.5, wpos.z * 0.02) + 1.0, 2.0) - 1.0;

    int height = islandIntensity * 16 + 16;
    if (wpos.y < height) {
        block = height > 16 ? BLOCK_STONE : BLOCK_WOOD;
    }

    blocks[i] = block;
}
