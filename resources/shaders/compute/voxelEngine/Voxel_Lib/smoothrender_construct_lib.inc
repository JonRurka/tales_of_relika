#ifndef _SMOOTHRENDER_CONSTRUCT_INC_
#define _SMOOTHRENDER_CONSTRUCT_INC_

#define USE_LIBNOISE
#include "voxel_lib_core.inc"


#define USE_HEIGHTMAP 1
#define USE_MATERIALS 0
#define GEN_CRATER_MAP 0

#include "../../Samplers/sampler.inc"

#define SURFACE_TYPE_FLOOR 1
#define SURFACE_TYPE_WALL 2
#define SURFACE_TYPE_CEILING 3
#define Surface_Type int

#define HEIGHTMAP_PADDING 4
#define HEIGHTMAP_OFFSET 2

read_buffer_struct(Static_Settings, 0, in_static_settings)
readwrite_buffer_struct(Run_Settings, 1, in_run_settings)

#if USL_OPENCL == 1
__constant fvec4* in_locOffset;
__constant fvec4* in_globalOffsets;
__constant ivec4* in_directionOffsets;

__constant int* in_edgeTable_data;
__constant int* in_TriTable_data;
#else
read_buffer(fvec4, 2, in_locOffset)
read_buffer(fvec4, 3, in_globalOffsets)	
read_buffer(ivec4, 4, in_directionOffsets)

read_buffer_int(5, in_edgeTable_data)
read_buffer_int(6, in_TriTable_data)
#endif



read_buffer_float(7, in_heightmap_data)
readwrite_buffer(fvec4, 8, in_height_neighboor_data)

readwrite_buffer(fvec4, 9, out_vertex_data)
readwrite_buffer(fvec4, 10, out_normal_data)
readwrite_buffer_int(11, out_triangles_data)
readwrite_buffer_int(12, out_counts_data)
readwrite_buffer_int(13, out_stitch_map_data)

readwrite_buffer(fvec4, 14, out_debug_data)

struct IsoSample{
	float iso;
	int type_id;
};


#define VERTEX_SELECT(vert_list, res_idx, edge_ent, test, p1_idx, p2_idx, invalid)         \
do {                                                                                       \
    if ((edge_ent & test) == 0 || invalid){                                                \
		vert_list[res_idx] = IVEC2(-1, -1);                                                \
        break;                                                                             \
    }                                                                                      \
	vert_list[res_idx] = IVEC2(p1_idx, p2_idx);                                            \
}while(false);

//GRIDPOINT_ARRAY_PACK_MATERIAL(grid, p1_idx);
//GRIDPOINT_ARRAY_PACK_MATERIAL(grid, p2_idx);
#define VERTEX_INTERP(grid_vert, isoLevel, grid, p1_idx, p2_idx)                  \
do{                                                                               \
	GRIDPOINT_ARRAY_PACK_MATERIAL(grid, p1_idx);								  \
	GRIDPOINT_ARRAY_PACK_MATERIAL(grid, p2_idx);								  \
																				  \
    float iso_1 = GRIDPOINT_ISO(grid, p1_idx);                                    \
    fvec4 pos_1_local = GRIDPOINT_LOCAL_POS(grid, p1_idx);                        \
	fvec4 normal_1 = GRIDPOINT_NORMAL(grid, p1_idx);                              \
																				  \
	float iso_2 = GRIDPOINT_ISO(grid, p2_idx);                                    \
    fvec4 pos_2_local = GRIDPOINT_LOCAL_POS(grid, p2_idx);                        \
    fvec4 normal_2 = GRIDPOINT_NORMAL(grid, p2_idx);                              \
																				  \
    if (FABS(isoLevel - iso_1) < 0.00001f){                                       \
		GRID_VERTEX_LOCAL_POS(grid_vert, 0) = GRIDPOINT_LOCAL_POS(grid, p1_idx);  \
		GRID_VERTEX_NORMAL(grid_vert, 0) = GRIDPOINT_NORMAL(grid, p1_idx);        \
        break;                                                                    \
	}																			  \
    if (FABS(isoLevel - iso_2) < 0.00001f) {                                      \
		GRID_VERTEX_LOCAL_POS(grid_vert, 0) = GRIDPOINT_LOCAL_POS(grid, p2_idx);  \
		GRID_VERTEX_NORMAL(grid_vert, 0) = GRIDPOINT_NORMAL(grid, p2_idx);        \
        break;                                                                    \
	}                                                                             \
    if (FABS(iso_1 - iso_2) < 0.00001f){                                          \
		GRID_VERTEX_LOCAL_POS(grid_vert, 0) = GRIDPOINT_LOCAL_POS(grid, p1_idx);  \
		GRID_VERTEX_NORMAL(grid_vert, 0) = GRIDPOINT_NORMAL(grid, p1_idx);        \
        break;                                                                    \
	}																			  \
    float mu = (isoLevel - iso_1) / (iso_2 - iso_1);                              \
																				  \
    fvec4 res_pos_local = FVEC4_ZERO;                                             \
	fvec4 res_normal = FVEC4_ZERO;                                                \
    ivec4 type = IVEC4_ZERO;                                                      \
    fvec4 type_ratios = FVEC4_ZERO;                                               \
																				  \
    res_pos_local.x = pos_1_local.x + mu * (pos_2_local.x - pos_1_local.x);       \
    res_pos_local.y = pos_1_local.y + mu * (pos_2_local.y - pos_1_local.y);       \
    res_pos_local.z = pos_1_local.z + mu * (pos_2_local.z - pos_1_local.z);       \
	res_normal.x = normal_1.x + mu * (normal_2.x - normal_1.x);                   \
    res_normal.y = normal_1.y + mu * (normal_2.y - normal_1.y);                   \
    res_normal.z = normal_1.z + mu * (normal_2.z - normal_1.z);                   \
																				  \
	res_pos_local.w = (mu < 0.5) ? pos_1_local.w : pos_2_local.w;				  \
																				  \
	GRID_VERTEX_LOCAL_POS(grid_vert, 0) = res_pos_local;                          \
	GRID_VERTEX_NORMAL(grid_vert, 0) = res_normal;                                \
}while(false);

// res_pos_local.w = (mu < 0.5) ? pos_1_local.w : pos_2_local.w;


//type = mu < 0.5 ? p1.type : p2.type;
//type_ratios = mu < 0.5 ? p1.type_ratio : p2.type_ratio;

/*float sphere_sample_5(float radius, fvec3 center, float x, float y, float z){

    return radius - sqrt(pow(x - center.x, 2) + pow(y - center.y, 2) + pow(z - center.z, 2));
}*/


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

float height_sample_3(float x, float y, STRUCT Settings s){

    return Sample_2D(s, x, y);
}

float height_sample_2(fvec2 p, STRUCT Settings s){
    return height_sample_3(p.x, p.y, s);
}

float iso_invert(float iso){
	if (iso > 0)
	{
		return 1 - iso;
	}
	else{
		return -(1 - FABS(iso));
	}
}

STRUCT IsoSample Combine_ISO(STRUCT Settings s, ivec3 voxel_pos, fvec3 world_pos, float iso_3D, float height, float edit_iso){
	
	float caveDensity = 5.0f;
	
	float iso_result = 0;
	int type_res = 1; //grass
	
	float raw_height_iso = height - world_pos.y;
	float height_iso = clamp(raw_height_iso, -2.0f, 2.0f);
	//height_iso = iso_invert(height_iso);
	
	float raw_cave_iso = iso_3D;//;sphere_sample_5(10, FVEC3(5, 17, 0), world_pos.x, world_pos.y, world_pos.z);
	float cave_iso = clamp(raw_cave_iso, -5.0f, 5.0f);
	//cave_iso = iso_invert(cave_iso);
	
	if (raw_cave_iso > 0.0 && world_pos.y < (height - 0.0f)){
		iso_result = clamp(clamp(height_iso, -5.0f, 5.0f) - cave_iso, -5.0f, 5.0f);
	}
	else{
		iso_result = height_iso;
	}
	
	if (world_pos.y < (height - 5)){
		type_res = 2; // Stone
	}
	else if (world_pos.y < (height - 2)){
		type_res = 0; // dirt
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


ivec4 Get_Direction_Offset(uint i){
	return get_buffer_vector(in_directionOffsets, i);
}

fvec4 Get_Global_Offset(uint i){
	return get_buffer_vector(in_globalOffsets, i);
}

void SetStitchMap(uint index, int val){
	set_buffer_scalar(out_stitch_map_data, index, val);
}

int Get_Edgetable(uint index){
	return get_buffer_scalar(in_edgeTable_data, index);
}

int Get_Tritable(uint index){
	return get_buffer_scalar(in_TriTable_data, index);
}

void Set_Count(uint index, int count){
	set_buffer_scalar(out_counts_data, index, count);
}

void Set_Debug_Data(uint index, fvec4 data){
	set_buffer_vector(out_debug_data, index, data);
}

int SetMeshEntry(uint voxelIndex, fvec4 p1, fvec4 n1, int t1, int count){

    uint vert_index = (voxelIndex * 15) + count;

	set_buffer_vector(out_vertex_data, vert_index, p1);
	set_buffer_vector(out_normal_data, vert_index, n1);

	set_buffer_scalar(out_triangles_data, vert_index, t1);

    count = count + 1;

    return count;
}

void Process_Voxel(STRUCT Static_Settings static_settings, ivec4 batch, uint g_index, uint v_index, uint cubeIndex, int edge_ent, ivec2 vert_map[12], fvec4 grid[GRIDPOINT_STRIDE * 8], float isoLevel)
{
	if(edge_ent == 0){
        return;
    }
	
	ivec3 index_pos = C_1D_to_3D(v_index, static_settings.ChunkSize.x, static_settings.ChunkSize.y);
	
	STRUCT Settings s = New_Settings();
    s.ChunkSizeX = static_settings.ChunkSize.x + 2;
    s.ChunkSizeY = static_settings.ChunkSize.y + 2;
    s.ChunkSizeZ = static_settings.ChunkSize.z + 2;
    s.VoxelsPerMeter = static_settings.VoxelsPerMeter[0];
    s.seed = 0;
    s.quality = QUALITY_STD;

    int count = 0;
	
	uint tritble_start = cubeIndex * 16;
	int i = 0;
	for (i = 0; Get_Tritable(cubeIndex * 16 + i) != -1; i += 3)
    {

        int curTris = count;


		
		NEW_GRID_VERTEX_1(gv_1)
		NEW_GRID_VERTEX_1(gv_2)
		NEW_GRID_VERTEX_1(gv_3)
		
		ivec2 edge1_idx = vert_map[Get_Tritable(cubeIndex * 16 + (i + 0))];
		ivec2 edge2_idx = vert_map[Get_Tritable(cubeIndex * 16 + (i + 1))];
		ivec2 edge3_idx = vert_map[Get_Tritable(cubeIndex * 16 + (i + 2))];
		
		if ((edge1_idx.x == -1 || edge1_idx.y == -1) ||
			(edge2_idx.x == -1 || edge2_idx.y == -1) ||
			(edge3_idx.x == -1 || edge3_idx.y == -1))
		{
			//Print("Invalid edge detected\n");
			continue;
		}

		VERTEX_INTERP(gv_1, isoLevel, grid, edge1_idx.x, edge1_idx.y)
		VERTEX_INTERP(gv_2, isoLevel, grid, edge2_idx.x, edge2_idx.y)
		VERTEX_INTERP(gv_3, isoLevel, grid, edge3_idx.x, edge3_idx.y)
		
		//fvec4 g_pos_1_after = GRIDPOINT_LOCAL_POS(grid, edge1_idx.x);
		
		//Print_Vec_2("Before: (%f, %f, %f, %f), After: (%f, %f, %f, %f)\n", g_pos_1, g_pos_1_after);
		
		
		fvec4 pos1 = GRID_VERTEX_LOCAL_POS(gv_1, 0);// p1.localPosition;
		fvec4 pos2 = GRID_VERTEX_LOCAL_POS(gv_2, 0);// p2.localPosition;
		fvec4 pos3 = GRID_VERTEX_LOCAL_POS(gv_3, 0);// p3.localPosition;
		
		fvec3 dir = cross(pos2.xyz - pos1.xyz, pos3.xyz - pos1.xyz);
        fvec3 norm = -normalize(dir);
		fvec4 norm_v4 = FVEC4(norm.x, norm.y, norm.z, 0.0f);
		
		fvec3 norm1 = GRID_VERTEX_NORMAL(gv_1, 0).xyz;// p1.normal.xyz;// normal_sample(s, index_pos, p1.globalPosition.xyz);
		fvec3 norm2 = GRID_VERTEX_NORMAL(gv_2, 0).xyz;// p2.normal.xyz;// normal_sample(s, index_pos, p2.globalPosition.xyz);
		fvec3 norm3 = GRID_VERTEX_NORMAL(gv_3, 0).xyz;// p3.normal.xyz;//normal_sample(s, index_pos, p3.globalPosition.xyz);
		
		//pos1.w = v_index;
		//pos2.w = v_index;
		//pos3.w = v_index;
		

        //p1.position.w = p1.type.x + p1.type_ratio.x;
        //p1.normal.w = p1.type.y + p1.type_ratio.y;

        //p2.position.w = p2.type.x + p2.type_ratio.x;
        //p2.normal.w = p2.type.y + p2.type_ratio.y;

        //p3.position.w = p3.type.x + p3.type_ratio.x;
        //p3.normal.w = p3.type.y + p3.type_ratio.y;


        //fvec3 dir = cross(p2.localPosition.xyz - p1.localPosition.xyz, p3.localPosition.xyz - p1.localPosition.xyz);
        //fvec3 norm = -normalize(dir);
		//fvec4 norm_v4 = FVEC4(norm.x, norm.y, norm.z, 0.0f);
		
		if (vector_angle(norm, norm1) > 45.0f){
			norm1 = norm;
		}
		if (vector_angle(norm, norm2) > 45.0f){
			norm2 = norm;
		}
		if (vector_angle(norm, norm3) > 45.0f){
			norm3 = norm;
		}

        //vec4 n1 = vec4(norm.x, norm.y, norm.z, 0);
        int t1 = INT(curTris) + 2;
        count = SetMeshEntry(GetBatchIndex(batch[0], v_index, batch[2]), pos1, FVEC4(norm1.x, norm1.y, norm1.z, 0), t1, count);
        
        //vec4 n2 = vec4(norm.x, norm.y, norm.z, 0);
        int t2 = INT(curTris) + 1;
        count = SetMeshEntry(GetBatchIndex(batch[0], v_index, batch[2]), pos2, FVEC4(norm2.x, norm2.y, norm2.z, 0), t2, count);
        
        //vec4 n3 = vec4(norm.x, norm.y, norm.z, 0);
        int t3 = INT(curTris) + 0;
        count = SetMeshEntry(GetBatchIndex(batch[0], v_index, batch[2]), pos3, FVEC4(norm3.x, norm3.y, norm3.z, 0), t3, count);
		
		
		/*Print_4("%i: Poly Verts 1: (%f, %f, %f)\n", i,
			pos1.x, pos1.y, pos1.z);
		Print_4("%i: Poly Verts 2: (%f, %f, %f)\n", i,
			pos2.x, pos2.y, pos2.z);
		Print_4("%i: Poly Verts 3: (%f, %f, %f)\n", i,
			pos3.x, pos3.y, pos3.z);*/
    }
	
    Set_Count(GetBatchIndex(batch[0], v_index, batch[2]), count);
	//Set_Debug_Data(v_index, FVEC4(edge_ent, cubeIndex, count, i));
	//Set_Debug_Data(v_index, FVEC4(count, 0, 0, 0));
}

void CreateVertList(STRUCT Static_Settings static_settings, ivec4 batch, uint g_index, uint v_index, fvec4 grid[8 * GRIDPOINT_STRIDE], float isoLevel, uint ChunkSizeX, uint ChunkSizeY, uint ChunkSizeZ){

    //STRUCT GridPoint vertList[12];
	ivec2 vert_map[12];
	
    int cubeIndex = 0;

    /*SetDebug_Grid(index, 0, grid[0]);
    SetDebug_Grid(index, 1, grid[1]);
    SetDebug_Grid(index, 2, grid[2]);
    SetDebug_Grid(index, 3, grid[3]);
    SetDebug_Grid(index, 4, grid[4]);
    SetDebug_Grid(index, 5, grid[5]);
    SetDebug_Grid(index, 6, grid[6]);
    SetDebug_Grid(index, 7, grid[7]);*/


	if (GRIDPOINT_ISO(grid, 0) > isoLevel) cubeIndex |= 1;
    if (GRIDPOINT_ISO(grid, 1) > isoLevel) cubeIndex |= 2;
    if (GRIDPOINT_ISO(grid, 2) > isoLevel) cubeIndex |= 4;
    if (GRIDPOINT_ISO(grid, 3) > isoLevel) cubeIndex |= 8;
    if (GRIDPOINT_ISO(grid, 4) > isoLevel) cubeIndex |= 16;
    if (GRIDPOINT_ISO(grid, 5) > isoLevel) cubeIndex |= 32;
    if (GRIDPOINT_ISO(grid, 6) > isoLevel) cubeIndex |= 64;
    if (GRIDPOINT_ISO(grid, 7) > isoLevel) cubeIndex |= 128;

    int edge_ent = Get_Edgetable(cubeIndex);

	//Set_Debug_Data(g_index, FVEC4(grid[0].position.w, grid[1].position.w, grid[2].position.w, grid[3].position.w));
	//Set_Debug_Data(g_index, FVEC4(grid[4].position.w, grid[5].position.w, grid[6].position.w, grid[7].position.w));
	//Set_Debug_Data(g_index, FVEC4(g_index, FLOAT(cubeIndex), edge_ent, 0));

    bool invalid = (edge_ent == 0);
	
	
	VERTEX_SELECT(vert_map, 0,  edge_ent, 1,    0, 1, invalid);
    VERTEX_SELECT(vert_map, 1,  edge_ent, 2,    1, 2, invalid);
    VERTEX_SELECT(vert_map, 2,  edge_ent, 4,    2, 3, invalid);
    VERTEX_SELECT(vert_map, 3,  edge_ent, 8,    3, 0, invalid);

    VERTEX_SELECT(vert_map, 4,  edge_ent, 16,   4, 5, invalid);
    VERTEX_SELECT(vert_map, 5,  edge_ent, 32,   5, 6, invalid);
    VERTEX_SELECT(vert_map, 6,  edge_ent, 64,   6, 7, invalid);
    VERTEX_SELECT(vert_map, 7,  edge_ent, 128,  7, 4, invalid);

    VERTEX_SELECT(vert_map, 8,  edge_ent, 256,  0, 4, invalid);
    VERTEX_SELECT(vert_map, 9,  edge_ent, 512,  1, 5, invalid);
    VERTEX_SELECT(vert_map, 10, edge_ent, 1024, 2, 6, invalid);
    VERTEX_SELECT(vert_map, 11, edge_ent, 2048, 3, 7, invalid);


    Process_Voxel(static_settings, batch, g_index, v_index, cubeIndex, edge_ent, vert_map, grid, isoLevel);
    
}

void RenderBlock(ivec4 batch, uint g_index, uint v_index, STRUCT ISO_Material gr_point, STRUCT Static_Settings static_settings, STRUCT Run_Settings run_settings){

    //float iso = gr_point.final_iso.w;
    //ivec4 material_types = gr_point.material_types;
    //fvec4 material_ratios = gr_point.material_ratios;

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

    for (int i = 0; i < 8; ++i){
		
		uint index = v_index;
		ivec4 p_index_pos = index_pos + Get_Direction_Offset(i);
		fvec4 globalWorldPos = pos_originWorld + Get_Global_Offset(i);
		fvec4 localWorldPos = pos_localWorld + Get_Global_Offset(i);
		
		uint index_exp = C_3D_to_1D(p_index_pos.x, p_index_pos.y, p_index_pos.z, ChunkSizeX + 1, ChunkSizeZ + 1);

		uint index2 = C_3D_to_1D(p_index_pos.x, p_index_pos.y, p_index_pos.z, ChunkSizeX, ChunkSizeZ);
		
		uint batch_index = GetBatchIndex((UINT(batch[1]) % UINT(batch_settings[0])), index_exp, batch[3]);
		
		STRUCT IsoSample iso_sample = iso_sample_4(s, p_index_pos.xyz, globalWorldPos.xyz, 4);
		fvec3 normal = normal_sample(s, p_index_pos.xyz, globalWorldPos.xyz);
		
		//if (i == 3 && p_index_pos.x == 0 && p_index_pos.z == 0){
			//Print_2("(%i): ISO: %f\n", p_index_pos.y, iso_val);
		//}
		
		
		//NEW_GRIDPOINT_1(gr_point)
		GRIDPOINT_ISO(grid, i) = iso_sample.iso;
		GRIDPOINT_LOCAL_POS(grid, i) = localWorldPos;
		GRIDPOINT_GLOBAL_POS(grid, i) = globalWorldPos;
		GRIDPOINT_VOXEL_ID(grid, i) = FVEC4(p_index_pos.x, p_index_pos.y, p_index_pos.z, 0);
		GRIDPOINT_NORMAL(grid, i) = FVEC4(normal.x, normal.y, normal.z, 0);
		GRIDPOINT_TYPE(grid, i) = FVEC4(iso_sample.type_id, 0, 0, 0);
		GRIDPOINT_TYPE_RATIO(grid, i) = FVEC4(1, 0, 0, 0);
		
		//STRUCT GridPoint gr_point = New_GridPoint();
		//gr_point.iso = iso_val;
		//gr_point.localPosition = localWorldPos;
		//gr_point.globalPosition = globalWorldPos;
		//gr_point.normal = FVEC4(normal.x, normal.y, normal.z, 0);
		//gr_point.type = IVEC4(0, 0, 0, 0);// iso_mat.material_types;
		//gr_point.type_ratio = FVEC4(0, 0, 0, 0);// iso_mat.material_ratios;
    }

    Set_Count(GetBatchIndex(batch[0], v_index, batch[2]), 0);
    SetStitchMap(GetBatchIndex(batch[0], v_index, batch[2]), -1);

    CreateVertList(static_settings, batch, g_index, v_index, grid, 0, ChunkSizeX, ChunkSizeY, ChunkSizeZ);
    
}

void smoothrender_construct_main(uint index)
{
	STRUCT Static_Settings static_settings = GET_STATIC_SETTINGS();
	
	uint numBatchesPerGroup = static_settings.batches[0];
	uint numBatchGroups = static_settings.batches[1];
	
	STRUCT Run_Settings run_settings_ctrl = GET_RUN_SETTINGS_CTRL();
	uint batch_group = run_settings_ctrl.Location.x;
	uint start_index = (batch_group * numBatchesPerGroup);
	
	uint inst_index = start_index + (INT(index) / static_settings.FullChunkSize[0]);
	STRUCT Run_Settings run_settings = GET_RUN_SETTINGS(inst_index);
	
	
	int batchIndex = run_settings.Location.w;
    int batchIndexExpanded = run_settings.Location.w;
    ivec4 batch = IVEC4(batchIndex, batchIndexExpanded, static_settings.FullChunkSize[0], static_settings.FullChunkSize[1]);

	uint voxel_index = GetVoxelIndex(batchIndex, index, batch[2]);
	
	//if (voxel_index == 0)
	//	Print_1("construct: inst_index: %i\n", batchIndex);
	//	Print_1("m_numBatchesPerGroup: %i\n", m_numBatchesPerGroup);
	//	Print_2("%i: batch_group: %i\n", index, batch_group);
	
	if (run_settings.Location.w == -1){
        Set_Count(GetBatchIndex(batch[0], voxel_index, batch[2]), 0);
        SetStitchMap(GetBatchIndex(batch[0], voxel_index, batch[2]), -1);
        return;
    }
	
	
	
	//ISO_Material mat_point = in_iso_mat_data[index];
	uint batch_ind = GetBatchIndex((batchIndexExpanded % static_settings.batches[0]), voxel_index, batch[3]);
    
	STRUCT ISO_Material mat_point = New_ISO_Material();//get_buffer_object(in_iso_mat_data, batch_ind);//get_iso_mat(index);
    
	//get_buffer_vector(in_directionOffsets, i)
	//Set_Debug_Data(index, FVEC4(index, batch.x, batch.y, mat_point.final_iso.w));
	
	
    RenderBlock(batch, index, voxel_index, mat_point, static_settings, run_settings);

	if (index == 0){
		run_settings_ctrl.Location.x = INT(batch_group) + 1;
		
		if (run_settings_ctrl.Location.x == numBatchGroups)
		{
			run_settings_ctrl.Location.x = 0;
		}
		
		RUN_SETTINGS_CTRL_SET(run_settings_ctrl);
	}
	
	//if (index == 0)
	//	Set_Debug_Data(index, FVEC4(1, 1, 1, 1));
}

#endif // _SMOOTHRENDER_CONSTRUCT_INC_