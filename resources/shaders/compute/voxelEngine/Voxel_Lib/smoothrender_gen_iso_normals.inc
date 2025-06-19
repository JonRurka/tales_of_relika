#ifndef _SMOOTHRENDER_CONSTRUCT_INC_
#define _SMOOTHRENDER_CONSTRUCT_INC_

#define USE_LIBNOISE
#include "voxel_lib_core.inc"

#include "block_types.inc"

#define USE_HEIGHTMAP 1
#define USE_MATERIALS 0
#define GEN_CRATER_MAP 0

#include "../../Samplers/sampler.inc"

#define SURFACE_TYPE_FLOOR 1
#define SURFACE_TYPE_WALL 2
#define SURFACE_TYPE_CEILING 3
#define Surface_Type int

#define HEIGHTMAP_PADDING 4
#define HEIGHTMAP_OFFSET 0

#define GRID_PADDING HEIGHTMAP_PADDING
#define GRID_OFFSET 2


read_buffer_struct(Static_Settings, 0, in_static_settings)
readwrite_buffer_struct(Run_Settings, 1, in_run_settings)

read_buffer_float(2, in_heightmap_data)
readwrite_buffer(fvec4, 3, in_height_neighboor_data)

readwrite_buffer(fvec4, 4, out_iso_type)
readwrite_buffer(fvec4, 5, out_normal)


struct IsoSample{
	float iso;
	int type_id;
};

struct IsoNormalSample{
	STRUCT IsoSample iso_type;
	fvec3 normal;
};

float iso_sample_3D_4(float x, float y, float z, STRUCT Settings s){

    return Sample_3D(s, x, y, z);

    //float radius = 10;
    //return sphere_sample_4(radius, x, y, z);
}

float iso_sample_3D_2(fvec3 p, STRUCT Settings s){
    return iso_sample_3D_4(p.x, p.y, p.z, s);
}

float height_sample_5(int x, int y, int column_idx, STRUCT Settings s, int n_index)
{
	int size_x = INT(s.ChunkSizeX) + HEIGHTMAP_PADDING;
	int size_z = INT(s.ChunkSizeZ) + HEIGHTMAP_PADDING;
	int col_size = size_x * size_z;
	int col_offset_start = column_idx * col_size;
	int local_idx = INT(C_2D_to_1D(x + HEIGHTMAP_OFFSET, y + HEIGHTMAP_OFFSET, size_z));
	int data_index = col_offset_start + local_idx;
	float val = 0;
	if (n_index > 3){
		val = get_buffer_scalar(in_heightmap_data, data_index);
	}
	else{
		val = get_buffer_vector(in_height_neighboor_data, data_index)[n_index];
	}
	
	return val;
}

STRUCT IsoSample Combine_ISO(STRUCT Settings s, ivec3 voxel_pos, fvec3 world_pos, float iso_3D, float height, float edit_iso){
	
	float caveDensity = 5.0f;
	
	float iso_result = 0;
	int type_res = grass(); //grass
	
	float raw_height_iso = height - world_pos.y;
	float height_iso = clamp(raw_height_iso, -2.0f, 2.0f);
	//height_iso = iso_invert(height_iso);
	
	float raw_cave_iso = iso_3D;//;sphere_sample_5(10, FVEC3(5, 17, 0), world_pos.x, world_pos.y, world_pos.z);
	float cave_iso = clamp(raw_cave_iso, -5.0f, 5.0f);
	//cave_iso = iso_invert(cave_iso);
	
	if (raw_cave_iso > 0.0 && world_pos.y < (height - 0.0f)){
		//iso_result = clamp(clamp(height_iso, -5.0f, 5.0f) - cave_iso, -5.0f, 5.0f);
	}
	else{
		//iso_result = height_iso;
	}
	iso_result = height_iso;
	
	if (world_pos.y < (height - 5)){
		type_res = stone(); // Stone
	}
	else if (world_pos.y < (height - 2)){
		type_res = dirt(); // dirt
	}
	
	STRUCT IsoSample sample_res;
	sample_res.iso = iso_result;
	sample_res.type_id = type_res;
	return sample_res;

}

STRUCT IsoSample iso_sample_4(STRUCT Settings s, ivec3 p, fvec3 world_pos, int n_index){
    float iso = iso_sample_3D_2(world_pos, s);
    //float height = height_sample_3(world_pos.x, world_pos.z, s);
	float height = height_sample_5(p.x, p.z, s.column_index, s, n_index);

    return Combine_ISO(s, p, world_pos, iso, height, 0);
}

fvec3 normal_sample(STRUCT Settings s, ivec3 p, fvec3 world_pos)
{
	float epsilon = 0.01;
	
	// sample from extended heightmap data
	float x1 = iso_sample_4(s, p, world_pos - FVEC3(epsilon, 0, 0), 0).iso;
	float x2 = iso_sample_4(s, p, world_pos + FVEC3(epsilon, 0, 0), 1).iso;
	
	float z1 = iso_sample_4(s, p, world_pos - FVEC3(0, 0, epsilon), 2).iso;
	float z2 = iso_sample_4(s, p, world_pos + FVEC3(0, 0, epsilon), 3).iso;

	// sample using the epsilon above and below
	float y1 = iso_sample_4(s, p, world_pos - FVEC3(0, epsilon, 0), 4).iso;
	float y2 = iso_sample_4(s, p, world_pos + FVEC3(0, epsilon, 0), 4).iso;

	//Set_Debug_Data(g_index, FVEC4(y1, y2, y1 - y2, 1));

	fvec4 res = FVEC4((x2 - x1), (y2 - y1), (z2 - z1), 0);
	//Set_Debug_Data(g_index, res);
	
	res = normalize(res);
	//Set_Debug_Data(g_index, res);
	
	return -res.xyz;
}


STRUCT IsoNormalSample Gen_Grid_Point(ivec4 batch, uint g_index, uint v_index, STRUCT Static_Settings static_settings, STRUCT Run_Settings run_settings)
{
	uint ChunkSizeX = static_settings.ChunkSize.x;
    uint ChunkSizeY = static_settings.ChunkSize.y;
    uint ChunkSizeZ = static_settings.ChunkSize.z;

    float xSideLength = static_settings.SideLength.x;
    float ySideLength = static_settings.SideLength.y;
    float zSideLength = static_settings.SideLength.z;

    uint fullChunkSize = static_settings.FullChunkSize[0];

    //int v_index = GetVoxelIndex(batch[0], g_index, fullChunkSize);
	ivec3 index_pos_tmp = C_1D_to_3D(v_index, ChunkSizeX, ChunkSizeZ);
    ivec4 index_pos = IVEC4(index_pos_tmp.x, index_pos_tmp.y, index_pos_tmp.z, 0);

    // Coordinate of voxel in chunk
    int x = index_pos.x;
    int y = index_pos.y;
    int z = index_pos.z;

    // Coordinate of Chunk
    int cx = run_settings.Location.x;
    int cy = run_settings.Location.y;
    int cz = run_settings.Location.z;
	
	int column_idx = run_settings.int_data_1.x; // chunk column index

    // Global coordinate of first voxel in chunk.
    int xStart = cx * INT(ChunkSizeX);
    int yStart = cy * INT(ChunkSizeY);
    int zStart = cz * INT(ChunkSizeZ);

    // World position of voxel relative to chunk
    float xLocalWorld = x * xSideLength;
    float yLocalWorld = y * ySideLength;
    float zLocalWorld = z * zSideLength;
    fvec4 pos_localWorld = FVEC4(xLocalWorld, yLocalWorld, zLocalWorld, 0);

    // World position of voxel relative to world origin
    float xWorldOrigin = (xStart * xSideLength) + xLocalWorld;
    float yWorldOrigin = (yStart * ySideLength) + yLocalWorld;
    float zWorldOrigin = (zStart * zSideLength) + zLocalWorld;
    fvec4 pos_originWorld = FVEC4(xWorldOrigin, yWorldOrigin, zWorldOrigin, 0);

    uvec4 batch_settings = static_settings.batches;
    //STRUCT GridPoint grid[8];
	NEW_GRIDPOINT_ARRAY(grid, 8)

	STRUCT Settings s = New_Settings();
	s.column_index = column_idx;
    s.ChunkSizeX = ChunkSizeX;
    s.ChunkSizeY = ChunkSizeY;
    s.ChunkSizeZ = ChunkSizeZ;
    s.VoxelsPerMeter = static_settings.VoxelsPerMeter[0];
    s.seed = 0;
    s.quality = QUALITY_STD;
	
	ivec4 p_index_pos = index_pos;
	fvec4 globalWorldPos = pos_originWorld;
	
	uint index_exp = C_3D_to_1D(p_index_pos.x, p_index_pos.y, p_index_pos.z, ChunkSizeX + 1, ChunkSizeZ + 1);
	uint index2 = C_3D_to_1D(p_index_pos.x, p_index_pos.y, p_index_pos.z, ChunkSizeX, ChunkSizeZ);
	uint batch_index = GetBatchIndex((UINT(batch[1]) % UINT(batch_settings[0])), index_exp, batch[3]);
	
	STRUCT IsoSample iso_sample = iso_sample_4(s, p_index_pos.xyz, globalWorldPos.xyz, 4);
	fvec3 normal = normal_sample(s, p_index_pos.xyz, globalWorldPos.xyz);
	
	
	//fvec4 iso_type_res = FVEC4(iso_sample.iso, iso_sample.type_id);
	//fvec4 normal_res = FVEC4(normal.x, normal.y, normal.z, 0.0);
	
	STRUCT IsoNormalSample sample;
	sample.iso_type = iso_sample;
	sample.normal = normal;
	return sample;
	
	//set_buffer_object(out_iso_type, batch_index, iso_type_res);
	//set_buffer_object(out_normal, batch_index, normal_res);
}

void smoothrender_gen_iso_normal_main(uint index)
{
	
	STRUCT Static_Settings static_settings = GET_STATIC_SETTINGS();
	
	uint numBatchesPerGroup = static_settings.batches[0];
	uint numBatchGroups = static_settings.batches[1];
	
	STRUCT Run_Settings run_settings_ctrl = GET_RUN_SETTINGS_CTRL();
	uint batch_group = run_settings_ctrl.Location.x;
	uint start_index = (batch_group * numBatchesPerGroup);
	
	uint inst_index = start_index + (INT(index) / static_settings.FullChunkSize[1]);
	STRUCT Run_Settings run_settings = GET_RUN_SETTINGS(inst_index);
	
	if (run_settings.Location.w == -1){
		return;
	}
	
	int batchIndex = run_settings.Location.w;
    int batchIndexExpanded = run_settings.Location.w;
    ivec4 batch = IVEC4(batchIndex, batchIndexExpanded, static_settings.FullChunkSize[0], static_settings.FullChunkSize[1]);

	uint voxel_index = GetVoxelIndex(batchIndex, index, batch[3]);
	
	uint ChunkSizeX = static_settings.ChunkSize.x + GRID_PADDING;
    uint ChunkSizeY = static_settings.ChunkSize.y + GRID_PADDING;
    uint ChunkSizeZ = static_settings.ChunkSize.z + GRID_PADDING;
	
	float xSideLength = static_settings.SideLength.x;
    float ySideLength = static_settings.SideLength.y;
    float zSideLength = static_settings.SideLength.z;
	
	uint fullChunkSize = static_settings.FullChunkSize[0];
	
	//int v_index = GetVoxelIndex(batch[0], g_index, fullChunkSize);
	ivec3 index_pos = C_1D_to_3D(voxel_index, ChunkSizeX, ChunkSizeZ);
    //ivec4 index_pos = IVEC4(index_pos_tmp.x, index_pos_tmp.y, index_pos_tmp.z, 0);

    // Coordinate of voxel in chunk
    int x = index_pos.x;
    int y = index_pos.y;
    int z = index_pos.z;

    // Coordinate of Chunk
    int cx = run_settings.Location.x;
    int cy = run_settings.Location.y;
    int cz = run_settings.Location.z;
	
	int column_idx = run_settings.int_data_1.x; // chunk column index

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

    uvec4 batch_settings = static_settings.batches;
    //STRUCT GridPoint grid[8];
	NEW_GRIDPOINT_ARRAY(grid, 8)

	STRUCT Settings s = New_Settings();
	s.column_index = column_idx;
    s.ChunkSizeX = ChunkSizeX - GRID_PADDING;
    s.ChunkSizeY = ChunkSizeY - GRID_PADDING;
    s.ChunkSizeZ = ChunkSizeZ - GRID_PADDING;
    s.VoxelsPerMeter = static_settings.VoxelsPerMeter[0];
    s.seed = 0;
    s.quality = QUALITY_STD;
	
	ivec4 p_index_pos = IVEC4(index_pos.x, index_pos.y, index_pos.z, 0);
	fvec4 globalWorldPos = pos_originWorld;
	fvec4 localWorldPos = pos_localWorld;
	
	//uint index_exp = C_3D_to_1D(p_index_pos.x, p_index_pos.y, p_index_pos.z, ChunkSizeX + 1, ChunkSizeZ + 1);
	//uint index2 = C_3D_to_1D(p_index_pos.x, p_index_pos.y, p_index_pos.z, ChunkSizeX, ChunkSizeZ);
	//uint batch_index = GetBatchIndex((UINT(batch[1]) % UINT(batch_settings[0])), index_exp, batch[3]);
	
	STRUCT IsoSample iso_sample = iso_sample_4(s, p_index_pos.xyz, localWorldPos.xyz, 4);
	fvec3 normal = normal_sample(s, p_index_pos.xyz, localWorldPos.xyz);
	
	
	//uint batch_ind = GetBatchIndex((batchIndexExpanded % static_settings.batches[0]), voxel_index, batch[3]);
	
	//STRUCT IsoNormalSample iso_normal_sample = Gen_Grid_Point(batch, index, voxel_index, static_settings, run_settings);
	//STRUCT IsoSample iso_sample = iso_normal_sample.iso_type;
	//fvec3 normal = iso_normal_sample.normal;
	
	fvec4 iso_type_res = FVEC4(iso_sample.iso, iso_sample.type_id, 0.0, 0.0);
	fvec4 normal_res = FVEC4(normal.x, normal.y, normal.z, 0.0);
	
	if(x == 5 && z == 5 && (y >= 3 && y <= 10))
	{
		//Print_2("%i: %f \n", y, iso_sample.iso);
	}
	
	
	// dst_voxel_index
	uint batch_voxel_index = GetBatchIndex((batchIndexExpanded % static_settings.batches[0]), voxel_index, batch[3]);
	set_buffer_object(out_iso_type, batch_voxel_index, iso_type_res);
	set_buffer_object(out_normal, batch_voxel_index, normal_res);
	
    //set_buffer_object(out_data, batch_index, mat_point);
}





#endif //_SMOOTHRENDER_GEN_ISO_NORMALS_INC_