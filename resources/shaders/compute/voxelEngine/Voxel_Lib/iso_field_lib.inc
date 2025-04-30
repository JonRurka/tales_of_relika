#ifndef _ISO_FIELD_INC_
#define _ISO_FIELD_INC_

#include "voxel_lib_core.inc"
#include "sampler.inc"

read_buffer_struct(Static_Settings, 0, in_static_settings)
read_buffer_struct(Run_Settings, 1, in_run_settings)

readwrite_buffer(fvec4, 2, out_data)

readwrite_buffer_float(3, out_debug_data)

float sphere_sample_4(float radius, float x, float y, float z){

    fvec3 sphere_center = FVEC3(16, 16, 16);

    return radius - sqrt(pow(x - sphere_center.x, 2) + pow(y - sphere_center.y, 2) + pow(z - sphere_center.z, 2));
}

float iso_sample_4(float x, float y, float z, STRUCT Settings s){

    return Sample_3D(s, x, y, z);

    //float radius = 10;

    //return sphere_sample_3(radius, x, y, z);
}

float iso_sample_2(fvec3 p, STRUCT Settings s){
    return iso_sample_4(p.x, p.y, p.z,  s);
}

fvec3 normal_sample(float x, float y, float z, float centerDistance, STRUCT Settings s){
    fvec3 p = FVEC3(x, y, z);
    float epsilon = 0.001; // arbitrary — should be smaller than any surface detail in your distance function, but not so small as to get lost in float precision
    //float centerDistance = iso_sample_1(p);
    float xDistance = iso_sample_2(p + FVEC3(epsilon, 0, 0), s);
    float yDistance = iso_sample_2(p + FVEC3(0, epsilon, 0), s);
    float zDistance = iso_sample_2(p + FVEC3(0, 0, epsilon), s);
    fvec3 normal = (FVEC3(xDistance - centerDistance, yDistance - centerDistance, zDistance - centerDistance)) / epsilon;
    return normal;
}

float getISOval(uint index){
    //uint buffer_index = index / 4;
	//uint access_index = index % 4;

	
    float val = 0;//in_data[buffer_index].values[access_index];

    return val;
}

void setISOval(uint index, float iso, fvec3 normal){
	set_buffer_vector(out_data, index, FVEC4(normal.x, normal.y, normal.z, iso));
	
    //out_data[index].values = vec4(normal.x, normal.y, normal.z, iso);

    //uint buffer_index = index / 4;
	//uint access_index = index % 4;
	//out_data[buffer_index].values[access_index] = value;
}

void iso_field_gen_main(uint index) {
	
	STRUCT Static_Settings static_settings = GET_STATIC_SETTINGS();
	
	uint inst_index = index / static_settings.FullChunkSize[1];
    STRUCT Run_Settings run_settings = GET_RUN_SETTINGS(inst_index);
	
	uint ChunkSizeX = static_settings.ChunkSize.x;
    uint ChunkSizeY = static_settings.ChunkSize.y;
    uint ChunkSizeZ = static_settings.ChunkSize.z;
	
	uint FullChunkSize = (ChunkSizeX) * (ChunkSizeY) * (ChunkSizeZ);
    uint FullChunkSize_exp = (ChunkSizeX + 2) * (ChunkSizeY + 2) * (ChunkSizeZ + 2);
	
	uint numBatchesPerGroup = static_settings.batches[0];
	
	if (run_settings.Location.w == -1 || index >= (FullChunkSize_exp * numBatchesPerGroup)){
        return;
    }
	
	uint batchIndex = UINT(run_settings.Location.w);
    uint batchIndexExpanded = UINT(run_settings.Location.w);
	ivec4 batch = IVEC4(batchIndex, batchIndexExpanded, static_settings.FullChunkSize[0], FullChunkSize_exp);
	
	uint voxel_index = GetVoxelIndex(batchIndexExpanded, index, batch[3]);
	
	float xSideLength = static_settings.SideLength.x;
    float ySideLength = static_settings.SideLength.y;
    float zSideLength = static_settings.SideLength.z;
	
	ivec3 index_pos = C_1D_to_3D(voxel_index, ChunkSizeX + 2, ChunkSizeZ + 2);
	
	// Coordinate of voxel in chunk
    int x = index_pos.x;
    int y = index_pos.y;
    int z = index_pos.z;
	
	// Coordinate of Chunk
    int cx = run_settings.Location.x;
    int cy = run_settings.Location.y;
    int cz = run_settings.Location.z;
	
	// Global coordinate of first voxel in chunk.
    int xStart = cx * INT(ChunkSizeX);
    int yStart = cy * INT(ChunkSizeY);
    int zStart = cz * INT(ChunkSizeZ);
	
	// World position of voxel relative to chunk
    float xLocalWorld = x * xSideLength;
    float yLocalWorld = y * ySideLength;
    float zLocalWorld = z * zSideLength;
    fvec3 pos_localWorld = FVEC3(xLocalWorld, yLocalWorld, zLocalWorld);
	
	// World position of voxel relative to world origin
    float xWorldOrigin = (xStart * xSideLength) + xLocalWorld;
    float yWorldOrigin = (yStart * ySideLength) + yLocalWorld;
    float zWorldOrigin = (zStart * zSideLength) + zLocalWorld;
    fvec3 pos_originWorld = FVEC3(xWorldOrigin, yWorldOrigin, zWorldOrigin);
	
	STRUCT Settings s = New_Settings();
    s.ChunkSizeX = ChunkSizeX + 2;
    s.ChunkSizeY = ChunkSizeY + 2;
    s.ChunkSizeZ = ChunkSizeZ + 2;
    s.VoxelsPerMeter = static_settings.VoxelsPerMeter[0];
	
	float iso = iso_sample_4(pos_originWorld.x, pos_originWorld.y, pos_originWorld.z, s);
    fvec3 normal = FVEC3(0,0,0);//-normal_sample(pos_originWorld.x, pos_originWorld.y, pos_originWorld.z, iso);
	
	setISOval(GetBatchIndex((batchIndexExpanded % static_settings.batches[0]), voxel_index, batch[3]), iso, normal);
}


#endif // _ISO_FIELD_INC_