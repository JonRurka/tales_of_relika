#ifndef _HEIGHTMAP_INC_
#define _HEIGHTMAP_INC_

#include "../../libnoise/libnoise.inc"
#include "../../Samplers/sampler.inc"

struct Static_Settings{
    fvec4 VoxelsPerMeter;
    uvec4 ChunkMeterSize;
    uvec4 ChunkSize;
    uvec4 FullChunkSize;
    fvec4 skipDist;
    fvec4 half_;
    fvec4 SideLength;
    uvec4 batches;
};

struct Run_Settings{
    ivec4 Location;
};

read_buffer_struct(Static_Settings, 0, in_static_settings)
read_buffer_struct(Run_Settings, 1, in_run_settings)

// x: start
// y: size
read_buffer(ivec4, 2, in_column_offsets)

readwrite_buffer_float(3, out_height_data)
readwrite_buffer_int(4, out_biome_data)

#define GET_STATIC_SETTINGS() get_buffer_object(in_static_settings, 0)
#define GET_RUN_SETTINGS() get_buffer_object(in_run_settings, (0))

uint C_2D_to_1D(int x, int y, uint width) {
    return UINT(y * width + x);
}

ivec2 C_1D_to_2D(uint i, uint width) {
    int y = INT(i / width);
    int x = INT(i % width);
    return IVEC2(x, y);
}

float height_sample_3(float x, float y, STRUCT Settings s){
    return Sample_2D(s, x, y);
}

float height_sample_2(fvec2 p, STRUCT Settings s){
    return height_sample_3(p.x, p.y, s);
}

void heightmap_gen_main(uint index) 
{
	STRUCT Static_Settings static_settings = GET_STATIC_SETTINGS();
	uint ChunkSizeX = static_settings.ChunkSize.x + 1;
    uint ChunkSizeZ = static_settings.ChunkSize.z + 1;
	
	STRUCT Run_Settings run_settings = GET_RUN_SETTINGS();
	
	ivec2 loc = C_1D_to_2D(index, ChunkSizeZ);
	
	float xSideLength = static_settings.SideLength.x;
    float zSideLength = static_settings.SideLength.z;
	
	// Coordinate of Chunk
    int cx = run_settings.Location.x;
    int cz = run_settings.Location.y;
	int chunk_data_index = run_settings.Location.w;
	
	// Global coordinate of first voxel in chunk.
    int xStart = cx * INT(ChunkSizeX);
    int zStart = cz * INT(ChunkSizeZ);
	
	// World position of voxel relative to chunk
    float xLocalWorld = loc.x * xSideLength;
    float zLocalWorld = loc.z * zSideLength;
    fvec2 pos_localWorld = FVEC2(xLocalWorld, zLocalWorld);
	
	// World position of voxel relative to world origin
    float xWorldOrigin = (xStart * xSideLength) + xLocalWorld;
    float zWorldOrigin = (zStart * zSideLength) + zLocalWorld;
    fvec2 pos_originWorld = FVEC2(xWorldOrigin, zWorldOrigin);
	
	STRUCT Settings s = New_Settings();
    s.ChunkSizeX = ChunkSizeX + 1;
    s.ChunkSizeZ = ChunkSizeZ + 1;
    s.VoxelsPerMeter = static_settings.VoxelsPerMeter[0];
	
	float height_sample_val = height_sample_3(pos_originWorld.x, pos_originWorld.y, s);
	
	int start_offset = chunk_data_index * ChunkSizeX * ChunkSizeZ;
	int out_index = start_offset + index;
	
	set_buffer_scalar(out_height_data, out_index, height_sample_val);
	set_buffer_scalar(out_biome_data, out_index, 0);
}


#endif //_HEIGHTMAP_INC_