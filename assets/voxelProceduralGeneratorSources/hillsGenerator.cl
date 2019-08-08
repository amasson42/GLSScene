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
double linearNoise(__global int* p, double x, double y, double z);
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

double linearNoise(__global int* p, double x, double y, double z) {
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

kernel void generateBigChunk(__global int* perlinPermutations, __global int* blocks, const int2 bigChunkPos, const int chunkSize, const int bigChunkWidth) {
	size_t i = get_global_id(0);
	
	int3 localPosition = getLocalPosition(i, chunkSize, bigChunkWidth);
	
	float3 wpos = (float3)(localPosition.x + bigChunkPos.x * chunkSize * bigChunkWidth,
						localPosition.y,
						localPosition.z + bigChunkPos.y * chunkSize * bigChunkWidth);

	int groundHeight = 15 * linearNoise(perlinPermutations, wpos.x * 0.046, 0, wpos.z * 0.073) + 50;

	int snowHeight = 0;
	{
		float3 height = (float3)wpos * (float3)(0.035, 0, 0.33);
		float nValue = linearNoise(perlinPermutations, height.x, height.y, height.z);
		snowHeight = nValue * 8 + 110;
	}

	if (localPosition.y == 0) {
		blocks[i] = BLOCK_BEDROCK;
	} else if (localPosition.y < 3) {
		float nValue = linearNoise(perlinPermutations, wpos.x * 1.2454, wpos.y + 5.24, wpos.z * 4.6378);
		blocks[i] = nValue >= 0.1 * localPosition.y ? BLOCK_BEDROCK : BLOCK_STONE;
	} else if (localPosition.y <= groundHeight) {
		float3 heightSeed = (float3)(0.03, 0.12, 0.04) * (float3)(0.03, 0.12, 0.04) * (wpos + (float3)(0.0, localPosition.y, 0.0));
		float nValue = linearNoise(perlinPermutations, heightSeed.x, heightSeed.y, heightSeed.z);

		int caveBlockValue = nValue > -0.40 ? BLOCK_STONE :
							nValue > -0.41 ? BLOCK_COBBLESTONE :
							BLOCK_AIR;

		int blockValue = 0;
		if (localPosition.y > snowHeight) {
			blockValue = BLOCK_ICE;
		} else if (localPosition.y == groundHeight) {
			blockValue = BLOCK_GRASS;
		} else if (localPosition.y > groundHeight - 3) {
			blockValue = BLOCK_DIRT;
		} else {
			blockValue = BLOCK_STONE;
		}

		blockValue = caveBlockValue == BLOCK_AIR ? BLOCK_AIR :
							localPosition.y > groundHeight - 3 ? blockValue :
							caveBlockValue;
		blocks[i] = blockValue;
	}
}