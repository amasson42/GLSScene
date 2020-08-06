
// default.cl

#define	BLOCK_AIR				0x00000000
#define	BLOCK_BEDROCK			0x00010000
#define	BLOCK_STONE				0x01010000
#define	BLOCK_DIRT				0x02010000
#define	BLOCK_GRASS				0x03010000
#define	BLOCK_SAND				0x04010000
#define	BLOCK_GRAVEL			0x05010000
#define	BLOCK_WATER				0x07010000
#define	BLOCK_WATER_SURFACE		0x07020000
#define	BLOCK_GRASS_BROWN		0x10010000
#define	BLOCK_WOOD				0x11010000
#define	BLOCK_GOLD				0x12010000
#define	BLOCK_CACTUS			0x31070000
#define	BLOCK_CACTUS_THIN		0x31060000
#define	BLOCK_BUSH				0x32080000
#define	BLOCK_BUSH_LEAFS		0x34030000
#define	BLOCK_FLOWER_YELLOW		0x33080000
#define	BLOCK_LEAFS_TREE		0x34010000
#define	BLOCK_LEAFS_MOUNTAIN	0x35010000
#define	BLOCK_BUSH_DEAD			0x36080000
#define	BLOCK_MUSHROOM_RED		0x37080000
#define	BLOCK_MUSHROOM_BROWN	0x38080000
#define	BLOCK_WOOD_PLANKS		0x13010000
#define	BLOCK_BRICKS			0x14010000
#define	BLOCK_COBBLESTONE		0x15010000
#define	BLOCK_COBBLESTONE_FENCE	0x15060000
#define	BLOCK_COBBLESTONE_TOP	0x15040000
#define	BLOCK_SANDSTONE			0x16010000
#define	BLOCK_ICE				0x20010000
#define	BLOCK_ICE_BROKEN		0x21010000
#define	BLOCK_SNOW				0x22010000
#define	BLOCK_SNOW_SLAB			0x22030000
#define	BLOCK_OBSIDIAN			0x24010000
#define	BLOCK_GRASS_PURPLE		0x30010000
#define	BLOCK_WOOD_FENCE		0x13060000
#define	BLOCK_SAPLING_TREE		0x11060000
#define	BLOCK_SAPLING_MOUNTAIN	0x11080000
#define	BLOCK_TERRACOTA_ORANGE	0x40010000
#define	BLOCK_TERRACOTA_BROWN	0x41010000
#define	BLOCK_TERRACOTA_YELLOW	0x42010000
#define	BLOCK_CLAY				0x43010000
#define	BLOCK_TNT				0x44010000

#define CAVEMAZE_NONE 0
#define CAVEMAZE_UP 1
#define CAVEMAZE_DOWN 2
#define CAVEMAZE_LEFT 4
#define CAVEMAZE_RIGHT 8

float grad(int hash, float x, float y, float z);
float noise(__global int* p, float x, float y, float z);
float lerp(float t, float a, float b);
float ridge(float h, float offset);
float ridgedMF(__global int* ppm, float3 p, float freq);

float grad(int hash, float x, float y, float z) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float noise(__global int* p, float x, float y, float z) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    float u = x;
    float v = y;
    float w = z;

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

float ridge(float h, float offset) {
    h = fabs(h);     // create creases
    h = offset - h; // invert so creases are at top
    h = h * h;      // sharpen creases
    return h;
}

float ridgedMF(__global int* ppm, float3 p, float freq) {
    float lacunarity = 2.0;
    float gain = 0.5;
    float offset = 0.9;

    float sum = 0.0;
    float amp = 0.5;
    float prev = 1.0;
    for (int i = 0; i < 4; i++) {
        float n = ridge(noise(ppm, p.x * freq, 0.42f, p.z * freq), offset);
        sum += n * amp;
        sum += n * amp * prev;
        prev = n;
        freq *= lacunarity;
        amp *= gain;
    }
    return sum;
}

int3 getLocalPosition(int i, const int chunkSize, const int bigChunkWidth);
int3 getWorldPosition(const int3 localPosition, const int2 bigChunkPos, const int bigChunkFullWidth);

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

int calculBlockAt(__global int* ppm, int3 wpos, int3 lpos, int paths);

kernel void generateRoom(
    __global int* ppm,
    __global int* blocks,
    const int2 bigChunkPos,
    const int chunkSize,
    const int bigChunkWidth,
    const int paths
    ) {
	size_t i = get_global_id(0);
	int3 localPosition = getLocalPosition(i, chunkSize, bigChunkWidth);
    localPosition.x -= chunkSize * bigChunkWidth / 2;
    localPosition.z -= chunkSize * bigChunkWidth / 2;
	int3 wpos = getWorldPosition(localPosition, bigChunkPos, chunkSize * bigChunkWidth);
	blocks[i] = calculBlockAt(ppm, wpos, localPosition, paths);
}

int calculBlockAt(__global int* ppm, int3 wpos, int3 lpos, int paths) {
    if (wpos.y == 15) {
        if (abs(lpos.x) < 3) {
            if (lpos.z > 0) {
                return paths & CAVEMAZE_UP ? BLOCK_STONE : BLOCK_WOOD_PLANKS;
            } else {

            }
        }
        if (abs(lpos.z) < 2)
            return BLOCK_AIR;
        return BLOCK_WOOD_PLANKS;
    } else
        return BLOCK_AIR;
}