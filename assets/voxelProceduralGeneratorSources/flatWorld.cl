// flatWorld.cl

#define BLOCK_AIR 0x00000000
#define BLOCK_GRASS 0x03010000

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

kernel void generateBigChunk(__global int* ppm, __global int* blocks, const int2 bigChunkPos, const int chunkSize, const int bigChunkWidth) {
    size_t i = get_global_id(0);
    int3 localPosition = getLocalPosition(i, chunkSize, bigChunkWidth);
    float3 wpos = getWorldPosition(localPosition, bigChunkPos, chunkSize * bigChunkWidth);
    blocks[i] = wpos.y == 0 ? BLOCK_GRASS : BLOCK_AIR;
}
