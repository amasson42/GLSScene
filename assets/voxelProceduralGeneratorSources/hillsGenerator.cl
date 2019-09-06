
// hillsGenerator.cl

#define BLOCK_AIR 0x00000000
#define BLOCK_BEDROCK 0x00010000
#define BLOCK_STONE 0x01010000
#define BLOCK_DIRT 0x02010000
#define BLOCK_GRASS 0x03010000
#define BLOCK_SAND 0x04010000
#define BLOCK_GRAVEL 0x05010000
#define BLOCK_CLAY 0x08010000
#define BLOCK_WATER 0x07010000
#define BLOCK_WATER_SURFACE 0x07020000
#define BLOCK_GRASS_BROWN 0x10010000
#define BLOCK_WOOD 0x11010000
#define BLOCK_LEAFS 0x12010000
#define BLOCK_CACTUS 0x12080000 // FIXME
#define BLOCK_FLOWER_YELLOW 0x12080000 // FIXME
#define BLOCK_WOOD_PLANKS 0x13010000
#define BLOCK_BRICKS 0x14010000
#define BLOCK_COBBLESTONE 0x15010000
#define BLOCK_TERRACOTA_ORANGE 0x16010000 // FIXME
#define BLOCK_TERRACOTA_YELLOW 0x17010000 // FIXME
#define BLOCK_TERRACOTA_BROWN 0x18010000 // FIXME
#define BLOCK_ICE 0x20010000
#define BLOCK_ICE_BROKEN 0x21010000
#define BLOCK_SNOW 0x22010000 // FIXME
#define BLOCK_OBSIDIAN 0x24010000
#define BLOCK_GRASS_PURPLE 0x30010000
#define BLOCK_CACTUS 0x31010000
#define BLOCK_GOLD 0x34010000
#define BLOCK_TNT 0x35010000
#define BLOCK_WOOD_FENCE 0x13080000
#define BLOCK_TREE_SAPLING 0x11080000

#define WATER_LEVEL 64

double grad(int hash, double x, double y, double z);
double noise(__global int* p, double x, double y, double z);
double lerp(double t, double a, double b);
float ridge(float h, float offset);
float ridgedMF(__global int* ppm, float3 p, float freq);

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
        float n = ridge(noise(ppm, p.x * freq, 0.42, p.z * freq), offset);
        sum += n * amp;
        sum += n * amp * prev;
        prev = n;
        freq *= lacunarity;
        amp *= gain;
    }
    return sum;
}

int3 getLocalPosition(int i, const int chunkSize, const int bigChunkWidth);
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

int3 getWorldPosition(const int3 localPosition, const int2 bigChunkPos, const int bigChunkFullWidth);
int3 getWorldPosition(const int3 localPosition, const int2 bigChunkPos, const int bigChunkFullWidth) {
	return (int3)(localPosition.x + bigChunkPos.x * bigChunkFullWidth,
				localPosition.y,
				localPosition.z + bigChunkPos.y * bigChunkFullWidth);
}

int calculBlockAt(__global int* ppm, int3 wpos);

/* MARK: Biome intensities */

float biomesIntensity_grass(__global int* ppm, float3 wpos);
float biomesIntensity_forest(__global int* ppm, float3 wpos);
float biomesIntensity_mountains(__global int* ppm, float3 wpos);
float biomesIntensity_canyons(__global int* ppm, float3 wpos);
float biomesIntensity_desert(__global int* ppm, float3 wpos);
float biomesIntensity_ocean(__global int* ppm, float3 wpos);

float biomesIntensity_grass(__global int* ppm, float3 wpos) {
	return 1.0 + noise(ppm, wpos.x * 0.01 + 38145.42, 0.75, wpos.z * 0.01 + 23.41);
}

float biomesIntensity_forest(__global int* ppm, float3 wpos) {
	return 1.0 + noise(ppm, wpos.x * 0.01 + 7469.1337, 0.55, wpos.z * 0.01 + 23.41);
}

float biomesIntensity_mountains(__global int* ppm, float3 wpos) {
	return 1.0 + 1.5 * noise(ppm, wpos.x * 0.003 + 93.542, 0.95, wpos.z * 0.002 + 23.41);
}

float biomesIntensity_canyons(__global int* ppm, float3 wpos) {
	return 0.8 + 1.5 * noise(ppm, wpos.x * 0.001 + 87426.7456, 0.85, wpos.z * 0.001 + 23.41);
}

float biomesIntensity_desert(__global int* ppm, float3 wpos) {
	return 1.0 + noise(ppm, wpos.x * 0.01 + 841.943, 0.15, wpos.z * 0.01 + 23.41);
}

float biomesIntensity_ocean(__global int* ppm, float3 wpos) {
	return 1.0 + noise(ppm, wpos.x * 0.005 + 8415.871, 0.45, wpos.z * 0.005 + 23.41) + 0.3;
}

/* MARK: Biome height */

float biomeHeigh_grass(__global int* ppm, float3 wpos);
float biomeHeigh_forest(__global int* ppm, float3 wpos);
float biomeHeigh_mountains(__global int* ppm, float3 wpos);
float biomeHeigh_canyons(__global int* ppm, float3 wpos);
float biomeHeigh_desert(__global int* ppm, float3 wpos);
float biomeHeigh_ocean(__global int* ppm, float3 wpos);

float biomeHeigh_grass(__global int* ppm, float3 wpos) {
	return WATER_LEVEL + 6
		+ 7 * noise(ppm, wpos.x * 0.01 + 2654.456, 0.24, wpos.z * 0.01 + 651.45);
}

float biomeHeigh_forest(__global int* ppm, float3 wpos) {
	return WATER_LEVEL + 9
		+ 12 * noise(ppm, wpos.x * 0.02 + 2654.456, 0.24, wpos.z * 0.02 + 947.137);
}

float biomeHeigh_mountains(__global int* ppm, float3 wpos) {
	return WATER_LEVEL + 12
		+ 45 * ridgedMF(ppm, wpos, 0.01);
}

float biomeHeigh_canyons(__global int* ppm, float3 wpos) {
	return WATER_LEVEL + 10
		+ clamp(90 * (noise(ppm, wpos.x * 0.05 + 2654.456, 0.24, wpos.z * 0.015 + 1713.1) + noise(ppm, wpos.x * 0.005 + 843.2, 0.167, wpos.z * 0.0015)),
			-10.0 + 5 * noise(ppm, wpos.x * 0.05, 0.84, wpos.z * 0.05),
			20.0 + 3 * noise(ppm, wpos.x * 0.03, 0.94, wpos.z * 0.03));
}

float biomeHeigh_desert(__global int* ppm, float3 wpos) {
	return WATER_LEVEL + 6
		+ clamp(40 * (noise(ppm, wpos.x * 0.05 + 6841.456, 0.67, wpos.z * 0.015 + 134.47) + noise(ppm, wpos.x * 0.005 + 346.1, 0.944, wpos.z * 0.0015) * 3),
			-5.0 + 5 * noise(ppm, wpos.x * 0.02, 0.84, wpos.z * 0.02),
			5.0 + 3 * noise(ppm, wpos.x * 0.03, 0.94, wpos.z * 0.03));
}

float biomeHeigh_ocean(__global int* ppm, float3 wpos) {
	return WATER_LEVEL - 50
		+ 20 * (noise(ppm, wpos.x * 0.1 + 84.1, wpos.y * 0.1 + 0.894, wpos.z * 0.1 + 94.05) + 0.2)
		+ 45 * (noise(ppm, wpos.x * 0.01 + 684.4, 0.94, wpos.z * 0.01 + 684.15) + 0.5);
}

/* MARK: Biome create block */

int biomeBlockAt_grass(__global int* ppm, float3 wpos, int groundHeight, float intensity);
int biomeBlockAt_forest(__global int* ppm, float3 wpos, int groundHeight, float intensity);
int biomeBlockAt_mountains(__global int* ppm, float3 wpos, int groundHeight, float intensity);
int biomeBlockAt_canyons(__global int* ppm, float3 wpos, int groundHeight, float intensity);
int biomeBlockAt_desert(__global int* ppm, float3 wpos, int groundHeight, float intensity);
int biomeBlockAt_ocean(__global int* ppm, float3 wpos, int groundHeight, float intensity);

int biomeBlockAt_grass(__global int* ppm, float3 wpos, int groundHeight, float intensity) {
	if (wpos.y < (float)groundHeight - 2)
		return BLOCK_STONE;
	if (wpos.y < groundHeight)
		return BLOCK_DIRT;
	if (wpos.y == groundHeight)
		return BLOCK_GRASS;
	return BLOCK_AIR;
}

int biomeBlockAt_forest(__global int* ppm, float3 wpos, int groundHeight, float intensity) {
	if (wpos.y < (float)groundHeight - 2)
		return BLOCK_STONE;
	if (wpos.y < groundHeight)
		return BLOCK_DIRT;
	float attenued = -log(1 - intensity);
	if (wpos.y == groundHeight) {
		if ((noise(ppm, wpos.x * 0.6, wpos.y * 0.6, wpos.z * 0.6) + 0.0) + attenued / 3 > 0.3)
			return BLOCK_GRASS_BROWN;
		else
			return BLOCK_GRASS;
	}
	if (wpos.y == groundHeight + 1) {
		float clamped = clamp(attenued / 8.0f, 0.1f, 1.0f);
		if (pow(noise(ppm, wpos.x * 1.85, wpos.y * 1.85, wpos.z * 1.85) + 0.7, 5.0) + clamped > 2)
			return BLOCK_TREE_SAPLING;
	}
	return BLOCK_AIR;
}

int biomeBlockAt_mountains(__global int* ppm, float3 wpos, int groundHeight, float intensity) {
	if (wpos.y < (float)groundHeight - 2)
		return BLOCK_STONE;
	if (wpos.y < groundHeight)
		return BLOCK_STONE;
	if (wpos.y == groundHeight)
		return BLOCK_STONE;
	return BLOCK_AIR;
}

int biomeBlockAt_canyons(__global int* ppm, float3 wpos, int groundHeight, float intensity) {
	if (wpos.y < (float)min(groundHeight - 6, WATER_LEVEL + 10))
		return BLOCK_STONE;
	if (wpos.y <= groundHeight) {
		if ((int)wpos.y % 6 < 2)
			return BLOCK_TERRACOTA_YELLOW;
		else if ((int)wpos.y % 6 < 4)
			return BLOCK_TERRACOTA_ORANGE;
		else
			return BLOCK_TERRACOTA_BROWN;
	}
	if (wpos.y == groundHeight)
		return BLOCK_GRASS;
	return BLOCK_AIR;
}

int biomeBlockAt_desert(__global int* ppm, float3 wpos, int groundHeight, float intensity) {
	if (wpos.y < (float)groundHeight - 2)
		return BLOCK_STONE;
	if (wpos.y < groundHeight)
		return BLOCK_DIRT;
	if (wpos.y == groundHeight)
		return BLOCK_GRASS;
	return BLOCK_AIR;
}

int biomeBlockAt_ocean(__global int* ppm, float3 wpos, int groundHeight, float intensity) {
	if (wpos.y < (float)groundHeight - 2)
		return BLOCK_STONE;
	if (wpos.y < groundHeight)
		return BLOCK_DIRT;
	if (wpos.y == groundHeight)
		return BLOCK_WATER;
	return BLOCK_AIR;
}

#define BIOMES_COUNT 6

void normalizeBiomes(float biomes[BIOMES_COUNT]) {
	float total = 0;
	for (int i = 0; i < BIOMES_COUNT; i++)
		total += biomes[i];
	for (int i = 0; i < BIOMES_COUNT; i++)
		biomes[i] /= total;
}

int calculBlockAt(__global int* ppm, int3 wposi) {
	float3 wpos = (float3)(wposi.x, wposi.y, wposi.z);
	float biomesIntensity[BIOMES_COUNT];
	// Calcul all biomes intensities
	biomesIntensity[0] = biomesIntensity_grass(ppm, wpos);
	biomesIntensity[1] = biomesIntensity_forest(ppm, wpos);
	biomesIntensity[2] = biomesIntensity_mountains(ppm, wpos);
	biomesIntensity[3] = biomesIntensity_canyons(ppm, wpos);
	biomesIntensity[4] = biomesIntensity_desert(ppm, wpos);
	biomesIntensity[5] = biomesIntensity_ocean(ppm, wpos);
	normalizeBiomes(biomesIntensity);

	// Find the most present
	int biomeIndex = 0;
	for (int i = 1; i < BIOMES_COUNT; i++) {
		if (biomesIntensity[i] > biomesIntensity[biomeIndex])
			biomeIndex = i;
	}

	for (int i = 0; i < BIOMES_COUNT; i++) {
		biomesIntensity[i] = pow(biomesIntensity[i], 18);
	}
	normalizeBiomes(biomesIntensity);

	int groundHeight = (int)(0
		+ biomeHeigh_grass(ppm, wpos) * biomesIntensity[0]
		+ biomeHeigh_forest(ppm, wpos) * biomesIntensity[1]
		+ biomeHeigh_mountains(ppm, wpos) * biomesIntensity[2]
		+ biomeHeigh_canyons(ppm, wpos) * biomesIntensity[3]
		+ biomeHeigh_desert(ppm, wpos) * biomesIntensity[4]
		+ biomeHeigh_ocean(ppm, wpos) * biomesIntensity[5]
	);

	if (biomeIndex == 5 && groundHeight >= WATER_LEVEL) {
		biomeIndex = 0;
		for (int i = 1; i < BIOMES_COUNT; i++) {
			if (i == 5)
				continue;
			if (biomesIntensity[i] > biomesIntensity[biomeIndex])
				biomeIndex = i;
		}
	}

	int block;
	switch (biomeIndex) {
		case 0: block = biomeBlockAt_grass(ppm, wpos, groundHeight, biomesIntensity[biomeIndex]); break;
		case 1: block = biomeBlockAt_forest(ppm, wpos, groundHeight, biomesIntensity[biomeIndex]); break;
		case 2: block = biomeBlockAt_mountains(ppm, wpos, groundHeight, biomesIntensity[biomeIndex]); break;
		case 3: block = biomeBlockAt_canyons(ppm, wpos, groundHeight, biomesIntensity[biomeIndex]); break;
		case 4: block = biomeBlockAt_desert(ppm, wpos, groundHeight, biomesIntensity[biomeIndex]); break;
		case 5: block = biomeBlockAt_ocean(ppm, wpos, groundHeight, biomesIntensity[biomeIndex]); break;
		default: block = BLOCK_AIR; break;
	}

	if (wposi.y == WATER_LEVEL && block == BLOCK_AIR)
		block = BLOCK_WATER_SURFACE;

	return block;
}

kernel void generateBigChunk(__global int* ppm, __global int* blocks, const int2 bigChunkPos, const int chunkSize, const int bigChunkWidth) {
	size_t i = get_global_id(0);
	int3 localPosition = getLocalPosition(i, chunkSize, bigChunkWidth);
	int3 wpos = getWorldPosition(localPosition, bigChunkPos, chunkSize * bigChunkWidth);
    int block = BLOCK_AIR;

	block = calculBlockAt(ppm, wpos);

	blocks[i] = block;
}
