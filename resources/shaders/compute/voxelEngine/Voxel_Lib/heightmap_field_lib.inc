#ifndef _HEIGHTMAP_FIELD_INC_
#define _HEIGHTMAP_FIELD_INC_

#include "voxel_lib_core.inc"
#include "sampler.inc"

read_buffer_struct(Static_Settings, 0, in_static_settings)
read_buffer_struct(Run_Settings, 1, in_run_settings)

readwrite_buffer_float(2, out_data)

readwrite_buffer_float(3, out_debug_data)


float height_sample(float x, float y, STRUCT Settings s){

    return Sample_2D(s, x, y);
}

float height_sample(fvec2 p, STRUCT Settings s){
    return height_sample(p.x, p.y, s);
}

void setHeightVal(uint index, float height){

    //uint buffer_index = index / 4;
	//uint access_index = index % 4;
	//out_data[buffer_index].values[access_index] = height;
	
	set_buffer_scalar(out_data, index, height);
	
}

void heightmap_gen_main(uint index) {
	
	
	STRUCT Static_Settings static_settings = GET_STATIC_SETTINGS();
	
	uint ChunkSizeX = static_settings.ChunkSize.x;
    uint ChunkSizeY = static_settings.ChunkSize.y;
	
	uint fullSize = (ChunkSizeX + 2) * (ChunkSizeY + 2);
	
	uint inst_index = int(index) / fullSize;
    STRUCT Run_Settings run_settings = GET_RUN_SETTINGS(inst_index);
	
	uint numBatchesPerGroup = static_settings.batches[0];
	
	if (run_settings.Location.w == -1 || index >= (fullSize * numBatchesPerGroup)){
        return;
    }
	
	uint batchIndex = UINT(run_settings.Location.w);
    uint batchIndexExpanded = UINT(run_settings.Location.w);
	
	ivec4 batch = IVEC4(0, batchIndexExpanded, 0, fullSize);
	
	uint voxel_index = GetVoxelIndex(batchIndexExpanded, index, fullSize);
	
	ivec2 index_pos = C_1D_to_2D(voxel_index, ChunkSizeY + 2);
	
	float xSideLength = static_settings.SideLength.x;
    float ySideLength = static_settings.SideLength.y;
	
	// Coordinate of voxel
    int x = index_pos.x;
    int y = index_pos.y;
	
	// Coordinate of Chunk
    int cx = run_settings.Location.x;
    int cy = run_settings.Location.y;
	
	// Global coordinate of first voxel in chunk.
    int xStart = cx * INT(ChunkSizeX);
    int yStart = cy * INT(ChunkSizeY);
	
	// World position of voxel relative to chunk
    float xLocalWorld = x * xSideLength;
    float yLocalWorld = y * ySideLength;
    fvec2 pos_localWorld = FVEC2(xLocalWorld, yLocalWorld);
	
	// World position of voxel relative to world origin
    float xWorldOrigin = (xStart * xSideLength) + xLocalWorld;
    float yWorldOrigin = (yStart * ySideLength) + yLocalWorld;
    fvec2 pos_originWorld = FVEC2(xWorldOrigin, yWorldOrigin);
	
	STRUCT Settings s = New_Settings();
    s.ChunkSizeX = ChunkSizeX + 1;
    s.ChunkSizeZ = ChunkSizeY + 1;
    s.VoxelsPerMeter = static_settings.VoxelsPerMeter[0];
	
	float height_sample_val = height_sample(pos_originWorld.x, pos_originWorld.y, s);
	
	setHeightVal(GetBatchIndex((batchIndexExpanded % UINT(static_settings.batches[0])), voxel_index, fullSize), height_sample_val);
	
}



#endif //_HEIGHTMAP_FIELD_INC_