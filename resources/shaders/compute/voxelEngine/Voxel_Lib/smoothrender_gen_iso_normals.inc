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

#define HEIGHTMAP_PADDING 2
#define HEIGHTMAP_OFFSET 0

#define GRID_PADDING HEIGHTMAP_PADDING
#define GRID_OFFSET 1


read_buffer_struct(Static_Settings, 0, in_static_settings)
readwrite_buffer_struct(Run_Settings, 1, in_run_settings)

read_buffer_float(2, in_heightmap_data)
readwrite_buffer(fvec4, 3, in_height_neighboor_data)

readwrite_buffer(fvec4, 4, in_modification_data);

readwrite_buffer(fvec4, 5, out_iso_type)
readwrite_buffer(fvec4, 6, out_normal)


#include "sampler_data_funcs.inc"

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

    uint fullChunkSize = static_settings.FullChunkSize.x;

    //int v_index = GetVoxelIndex(BATCH_0(batch), g_index, fullChunkSize);
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
    s.VoxelsPerMeter = static_settings.VoxelsPerMeter.x;
    s.seed = 0;
    s.quality = QUALITY_STD;
	
	ivec4 p_index_pos = index_pos;
	fvec4 globalWorldPos = pos_originWorld;
	
	uint index_exp = C_3D_to_1D(p_index_pos.x, p_index_pos.y, p_index_pos.z, ChunkSizeX + 1, ChunkSizeZ + 1);
	uint index2 = C_3D_to_1D(p_index_pos.x, p_index_pos.y, p_index_pos.z, ChunkSizeX, ChunkSizeZ);
	uint batch_index = GetBatchIndex((UINT(BATCH_1(batch)) % UINT(BATCH_0(batch_settings))), index_exp, BATCH_3(batch));
	
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
	
	uint numBatchesPerGroup = static_settings.batches.x;
	uint numBatchGroups = static_settings.batches.y;
	
	STRUCT Run_Settings run_settings_ctrl = GET_RUN_SETTINGS_CTRL();
	uint batch_group = run_settings_ctrl.Location.x;
	uint start_index = (batch_group * numBatchesPerGroup);
	
	uint inst_index = start_index + (INT(index) / static_settings.FullChunkSize.y);
	STRUCT Run_Settings run_settings = GET_RUN_SETTINGS(inst_index);
	
	
	if (run_settings.Location.w == -1){
		return;
	}
	
	int batchIndex = run_settings.Location.w;
    int batchIndexExpanded = run_settings.Location.w;
    ivec4 batch = IVEC4(batchIndex, batchIndexExpanded, static_settings.FullChunkSize.x, static_settings.FullChunkSize.y);

	uint voxel_index = GetVoxelIndex(batchIndex, index, BATCH_3(batch));
	
	uint ChunkSizeX = static_settings.ChunkSize.x + GRID_PADDING;
    uint ChunkSizeY = static_settings.ChunkSize.y + GRID_PADDING;
    uint ChunkSizeZ = static_settings.ChunkSize.z + GRID_PADDING;
	
	float xSideLength = static_settings.SideLength.x;
    float ySideLength = static_settings.SideLength.y;
    float zSideLength = static_settings.SideLength.z;
	
	uint fullChunkSize = static_settings.FullChunkSize.x;
	
	//int v_index = GetVoxelIndex(BATCH_0(batch), g_index, fullChunkSize);
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
	int modifications_idx = run_settings.int_data_1.y; // chunk column index

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
	s.modification_index = modifications_idx;
    s.ChunkSizeX = ChunkSizeX - GRID_PADDING;
    s.ChunkSizeY = ChunkSizeY - GRID_PADDING;
    s.ChunkSizeZ = ChunkSizeZ - GRID_PADDING;
    s.VoxelsPerMeter = static_settings.VoxelsPerMeter.x;
    s.seed = 0;
    s.quality = QUALITY_STD;
	
	ivec4 p_index_pos = IVEC4(index_pos.x, index_pos.y, index_pos.z, 0);
	fvec4 globalWorldPos = pos_originWorld;
	fvec4 localWorldPos = pos_localWorld;
	
	//uint index_exp = C_3D_to_1D(p_index_pos.x, p_index_pos.y, p_index_pos.z, ChunkSizeX + 1, ChunkSizeZ + 1);
	//uint index2 = C_3D_to_1D(p_index_pos.x, p_index_pos.y, p_index_pos.z, ChunkSizeX, ChunkSizeZ);
	//uint batch_index = GetBatchIndex((UINT(BATCH_1(batch)) % UINT(batch_settings[0])), index_exp, BATCH_3(batch));
	
	STRUCT IsoSample iso_sample = iso_sample_4(s, p_index_pos.xyz, globalWorldPos.xyz, 4);
	fvec3 normal = normal_sample(s, p_index_pos.xyz, globalWorldPos.xyz);
	
	
	//uint batch_ind = GetBatchIndex((batchIndexExpanded % static_settings.batches[0]), voxel_index, BATCH_3(batch));
	
	//STRUCT IsoNormalSample iso_normal_sample = Gen_Grid_Point(batch, index, voxel_index, static_settings, run_settings);
	//STRUCT IsoSample iso_sample = iso_normal_sample.iso_type;
	//fvec3 normal = iso_normal_sample.normal;
	
	fvec4 iso_type_res = FVEC4(iso_sample.iso, iso_sample.type_id, 0.0, 0.0);
	fvec4 normal_res = FVEC4(normal.x, normal.y, normal.z, 0.0);
	
	
	
	if(x == 31 && y == 3 && z == 2)
	{
		//Print_2("%i: %f \n", y, iso_sample.iso);
		//Print_2("%i: %f \n", y, 0);
		//Print_4("Gen ISO: (%i, %i, %i) %f \n", x, y, z, iso_sample.iso);
	}
	
	if (index_pos.z == 28 && FABS(iso_type_res.x) < 2.0f /*&& (p_index_pos.y > 3 && p_index_pos.y < 10)*/) {
		//Print_4("Gen Block (%i, %i, %i): %f\n", index_pos.x, index_pos.y, index_pos.z, iso_type_res.x);
	}
	
	
	// dst_voxel_index
	uint batch_voxel_index = GetBatchIndex((batchIndexExpanded % static_settings.batches.x), voxel_index, BATCH_3(batch));
	set_buffer_object(out_iso_type, batch_voxel_index, iso_type_res);
	set_buffer_object(out_normal, batch_voxel_index, normal_res);
	
    //set_buffer_object(out_data, batch_index, mat_point);
}





#endif //_SMOOTHRENDER_GEN_ISO_NORMALS_INC_