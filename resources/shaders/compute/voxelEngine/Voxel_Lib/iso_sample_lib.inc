#ifndef _ISO_SAMPLE_LIB_
#define _ISO_SAMPLE_LIB_

#define USE_LIBNOISE
#include "voxel_lib_core.inc"

#include "block_types.inc"

#define USE_HEIGHTMAP 1
#define USE_MATERIALS 0
#define GEN_CRATER_MAP 0

#include "../../Samplers/sampler.inc"

#define HEIGHTMAP_PADDING 2
#define HEIGHTMAP_OFFSET 0

#define GRID_PADDING HEIGHTMAP_PADDING
#define GRID_OFFSET 1

struct Voxel_Location {
	ivec4 chunk;
	ivec4 voxel;
	ivec4 int_data_1;
};

read_buffer_struct(Static_Settings, 0, in_static_settings)

read_buffer_struct(Voxel_Location, 1, in_voxel_locations);

read_buffer_float(2, in_heightmap_data)
read_buffer(fvec4, 3, in_height_neighboor_data)
read_buffer(fvec4, 4, in_modification_data);

readwrite_buffer(fvec4, 5, out_iso_data);


#include "sampler_data_funcs.inc"


void iso_sample_main(uint index)
{
	STRUCT Static_Settings static_settings = GET_STATIC_SETTINGS();
	
	STRUCT Voxel_Location voxel_loc = get_buffer_object(in_voxel_locations, index);
	ivec4 chunk = voxel_loc.chunk;
	ivec4 voxel = voxel_loc.voxel;
	ivec4 int_data_1 = voxel_loc.int_data_1;
	
	uint ChunkSizeX = static_settings.ChunkSize.x + GRID_PADDING;
    uint ChunkSizeY = static_settings.ChunkSize.y + GRID_PADDING;
    uint ChunkSizeZ = static_settings.ChunkSize.z + GRID_PADDING;
	
	float xSideLength = static_settings.SideLength.x;
    float ySideLength = static_settings.SideLength.y;
    float zSideLength = static_settings.SideLength.z;
	
	uint fullChunkSize = static_settings.FullChunkSize.x;
	
	int x = voxel.x;
    int y = voxel.y;
    int z = voxel.z;
	
	int cx = chunk.x;
    int cy = chunk.y;
    int cz = chunk.z;
	
	int column_idx = int_data_1.x; // chunk column index
	int modifications_idx = int_data_1.y; // chunk column index
	
	// Global coordinate of first voxel in chunk.
    int xStart = cx * INT(ChunkSizeX - GRID_PADDING);
    int yStart = cy * INT(ChunkSizeY - GRID_PADDING);
    int zStart = cz * INT(ChunkSizeZ - GRID_PADDING);

    // World position of voxel relative to chunk
    float xLocalWorld = (x - GRID_OFFSET) * xSideLength;
    float yLocalWorld = (y - GRID_OFFSET) * ySideLength;
    float zLocalWorld = (z - GRID_OFFSET) * zSideLength;
    fvec4 pos_localWorld = FVEC4(xLocalWorld, yLocalWorld, zLocalWorld, 0);

    // World position of voxel relative to world origin
    float xWorldOrigin = (xStart * xSideLength) + xLocalWorld;
    float yWorldOrigin = (yStart * ySideLength) + yLocalWorld;
    float zWorldOrigin = (zStart * zSideLength) + zLocalWorld;
    fvec4 pos_originWorld = FVEC4(xWorldOrigin, yWorldOrigin, zWorldOrigin, 0);
	
	STRUCT Settings s = New_Settings();
	s.column_index = column_idx;
	s.modification_index = modifications_idx;
    s.ChunkSizeX = ChunkSizeX - GRID_PADDING;
    s.ChunkSizeY = ChunkSizeY - GRID_PADDING;
    s.ChunkSizeZ = ChunkSizeZ - GRID_PADDING;
    s.VoxelsPerMeter = static_settings.VoxelsPerMeter.x;
    s.seed = 0;
    s.quality = QUALITY_STD;
	
	ivec4 p_index_pos = IVEC4(voxel.x, voxel.y, voxel.z, 0);
	fvec4 globalWorldPos = pos_originWorld;
	fvec4 localWorldPos = pos_localWorld;
	
	STRUCT IsoSample iso_sample = iso_sample_4(s, p_index_pos.xyz, globalWorldPos.xyz, 4);
	
	fvec4 iso_type_res = FVEC4(iso_sample.iso, iso_sample.type_id, 0.0, 0.0);
	
	//printf("Sample ISO: (%i, %i, %i): %f \n", x, y, z, iso_sample.iso);
	
	set_buffer_object(out_iso_data, index, iso_type_res);
}



#endif