#ifndef _SMOOTHRENDER_CONSTRUCT_INC_
#define _SMOOTHRENDER_CONSTRUCT_INC_

#define USE_LIBNOISE
#include "voxel_lib_core.inc"


#define USE_HEIGHTMAP 1
#define USE_MATERIALS 1
#define GEN_CRATER_MAP 1

#include "sampler.inc"

#define SURFACE_TYPE_FLOOR 1
#define SURFACE_TYPE_WALL 2
#define SURFACE_TYPE_CEILING 3
#define Surface_Type int

read_buffer_struct(Static_Settings, 0, in_static_settings)
read_buffer_struct(Run_Settings, 1, in_run_settings)

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



readwrite_buffer_struct(ISO_Material, 7, in_iso_mat_data)

readwrite_buffer(fvec4, 8, out_vertex_data)
readwrite_buffer(fvec4, 9, out_normal_data)
readwrite_buffer_int(10, out_triangles_data)
readwrite_buffer_int(11, out_counts_data)
readwrite_buffer_int(12, out_stitch_map_data)

readwrite_buffer(fvec4, 13, out_debug_data)




#define VERTEX_SELECT(vert_list, res_idx, edge_ent, test, p1_idx, p2_idx, invalid)         \
do {                                                                                       \
    if ((edge_ent & test) == 0 || invalid){                                                \
		vert_list[res_idx] = IVEC2(-1, -1);                                                \
        break;                                                                             \
    }                                                                                      \
	vert_list[res_idx] = IVEC2(p1_idx, p2_idx);                                            \
}while(false);


#define VERTEX_INTERP(grid_vert, isoLevel, grid, p1_idx, p2_idx)                  \
do{                                                                               \
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
	GRID_VERTEX_LOCAL_POS(grid_vert, 0) = res_pos_local;                          \
	GRID_VERTEX_NORMAL(grid_vert, 0) = res_normal;                                \
}while(false);

//type = mu < 0.5 ? p1.type : p2.type;
//type_ratios = mu < 0.5 ? p1.type_ratio : p2.type_ratio;



float iso_sample_3D_4(float x, float y, float z, STRUCT Settings s){

    return Sample_3D(s, x, y, z);

    //float radius = 10;
    //return sphere_sample_4(radius, x, y, z);
}

float iso_sample_3D_2(fvec3 p, STRUCT Settings s){
    return iso_sample_3D_4(p.x, p.y, p.z, s);
}

float height_sample_3(float x, float y, STRUCT Settings s){

    return Sample_2D(s, x, y);
}

float height_sample_2(fvec2 p, STRUCT Settings s){
    return height_sample_3(p.x, p.y, s);
}

float Combine_ISO(STRUCT Settings s, ivec3 voxel_pos, fvec3 world_pos, float iso_3D, float height, float edit_iso){

	float result = iso_3D;

    //return iso_3D;

    //float result = clamp(height - (global_pos.z), -5, 5); // Apply heightmap

#if GEN_CRATER_MAP == 1

    float upper_limit = 10;
    float lower_limit = -10;

    // start everything is inside ground
    float result = upper_limit - 1;

    // TODO: Scale the cave noise by the distance past the cave bounds. 


    float wall_noise = 0;//Scale(iso_sample_3D(world_pos + vec3(498, -320, 103) * 3, s), lower_limit, upper_limit, 0, upper_limit);

    // distance(vec2(0, 0), vec2(world_pos.x, world_pos.y)) < (CRATER_METER_DIAMETER  / 2) - (33 + wall_noise)

    float blend_region = 60;
    float dist_center = distance(FVEC2(0, 0), FVEC2(world_pos.x, world_pos.y));
    float dist_edge = (CRATER_METER_DIAMETER  / 2) - (33);
    float blend_delta = abs(dist_edge - dist_center) / blend_region;

    // outside ground is negative.
    // inside ground is postive.
    iso_3D = Scale(iso_3D, lower_limit, upper_limit, 0, upper_limit);
    //if (iso_3D >= (upper_limit * 0.75) && 
    //    world_pos.z < (CRATER_BOTTOM + 3)){
        if (iso_3D >= (upper_limit * 0.85) && 
            world_pos.z < (CRATER_BOTTOM + 3)){

            if(dist_center > dist_edge){
                iso_3D = 0;
            }
            else if (dist_center > (dist_edge - blend_region) && dist_center <= dist_edge){
                iso_3D *= blend_delta;
                //iso_3D = LERP(iso_3D, 0, abs(dist_edge - dist_center) / 10.0)
            }
            

            //result = clamp(result, -2, 2) - iso_3D;
            result = clamp(result - iso_3D, lower_limit, upper_limit);

        }
        result = Scale(result, lower_limit, upper_limit, -1, 1);
    //}
	
#endif
    

#if USE_HEIGHTMAP == 1

    //height = 0;

    //above ground is negative.
    //below ground is postive.'
    height = LERP(height + 10, height, blend_delta);
    float height_iso = clamp(height - (world_pos.z), lower_limit, 0);
    result = clamp(result + height_iso, lower_limit, upper_limit);
	result = clamp(result, lower_limit, upper_limit);
	
#endif
    //result += height;

    //result = clamp(result, 0, 1) - clamp(iso_3D, 14, 1000); // Apply cave 3D noise

    // TODO: Apply edit

    return result; 
}

float iso_sample_3(STRUCT Settings s, ivec3 p, fvec3 world_pos){
    float iso = iso_sample_3D_2(world_pos, s);
    float height = height_sample_3(world_pos.x, world_pos.y, s);

    return iso;// Combine_ISO(s, p, world_pos, iso, height, 0);
}

fvec3 normal_sample(STRUCT Settings s, ivec3 p, fvec3 world_pos)
{
	float epsilon = 0.01;
	
	float x1 = iso_sample_3(s, p - IVEC3(1, 0, 0), world_pos - FVEC3(epsilon, 0, 0));
	float x2 = iso_sample_3(s, p + IVEC3(1, 0, 0), world_pos + FVEC3(epsilon, 0, 0));

	float y1 = iso_sample_3(s, p - IVEC3(0, 1, 0), world_pos - FVEC3(0, epsilon, 0));
	float y2 = iso_sample_3(s, p + IVEC3(0, 1, 0), world_pos + FVEC3(0, epsilon, 0));

	float z1 = iso_sample_3(s, p - IVEC3(0, 0, 1), world_pos - FVEC3(0, 0, epsilon));
	float z2 = iso_sample_3(s, p + IVEC3(0, 0, 1), world_pos + FVEC3(0, 0, epsilon));

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

STRUCT GridPoint GetGridPoint(
	STRUCT Static_Settings static_settings,
    ivec4 batch, 
    uvec4 batch_settings, 
    uint index, 
    ivec4 index_pos,
	fvec4 globalWorldPos,
    fvec4 localWorldPos, 
    uint i,
    uint ChunkSizeX, 
    uint ChunkSizeY, 
    uint ChunkSizeZ
){
	STRUCT Settings s = New_Settings();
    s.ChunkSizeX = ChunkSizeX + 2;
    s.ChunkSizeY = ChunkSizeY + 2;
    s.ChunkSizeZ = ChunkSizeZ + 2;
    s.VoxelsPerMeter = static_settings.VoxelsPerMeter[0];
    s.seed = 0;
    s.quality = QUALITY_STD;
	
	uint index_exp = C_3D_to_1D(index_pos.x, index_pos.y, index_pos.z, ChunkSizeX + 1, ChunkSizeZ + 1);

    uint index2 = C_3D_to_1D(index_pos.x, index_pos.y, index_pos.z, ChunkSizeX, ChunkSizeZ);
	
	uint batch_index = GetBatchIndex((UINT(batch[1]) % UINT(batch_settings[0])), index_exp, batch[3]);
	
	float iso_val = iso_sample_3(s, index_pos.xyz, globalWorldPos.xyz);
	fvec3 normal = normal_sample(s, index_pos.xyz, globalWorldPos.xyz);
	
	//STRUCT ISO_Material iso_mat;// = get_buffer_object(in_iso_mat_data, batch_index);
	//if (i == 3)
	//	Set_Debug_Data(index, FVEC4(localWorldPos.x, localWorldPos.y, localWorldPos.z, iso_val));
	
	//iso_mat.final_iso = FVEC4(0, 0, 0, iso_val);
	//iso_mat.material_types =IVEC4(INT(0), INT(0), 0, 0);
	//iso_mat.material_ratios = FVEC4(1, 0, 0, 0);
	
	STRUCT GridPoint gr_point = New_GridPoint();
	gr_point.iso = iso_val;
    gr_point.localPosition = localWorldPos;
	gr_point.globalPosition = globalWorldPos;
    gr_point.normal = FVEC4(normal.x, normal.y, normal.z, 0);
    gr_point.type = IVEC4(0, 0, 0, 0);// iso_mat.material_types;
    gr_point.type_ratio = FVEC4(0, 0, 0, 0);// iso_mat.material_ratios;

    return gr_point;
}

STRUCT GridPoint VertexInterp(int edge_ent, int test, float isoLevel, STRUCT GridPoint p1, STRUCT GridPoint p2, bool invalid){
    if ((edge_ent & test) == 0 || invalid){
        STRUCT GridPoint gr_point = New_GridPoint();
        gr_point.localPosition = FVEC4(0, 0, 0, -1000000); // invalid
        return gr_point;
    }

    float iso_1 = p1.iso;
    fvec4 pos_1_local = p1.localPosition;
	fvec4 pos_1_world = p1.globalPosition;
	fvec4 normal_1 = p1.normal;

	float iso_2 = p2.iso;
    fvec4 pos_2_local = p2.localPosition;
	fvec4 pos_2_world = p2.globalPosition;
    fvec4 normal_2 = p2.normal;

    if (FABS(isoLevel - iso_1) < 0.00001f)
        return p1;
    if (FABS(isoLevel - iso_2) < 0.00001f)
        return p2;
    if (FABS(iso_1 - iso_2) < 0.00001f)
        return p1;

    float mu = (isoLevel - iso_1) / (iso_2 - iso_1);

    
    fvec4 res_pos_local = FVEC4_ZERO;
    fvec4 res_pos_world = FVEC4_ZERO;
	fvec4 res_normal = FVEC4_ZERO;
    ivec4 type = IVEC4_ZERO;
    fvec4 type_ratios = FVEC4_ZERO;

    res_pos_local.x = pos_1_local.x + mu * (pos_2_local.x - pos_1_local.x);
    res_pos_local.y = pos_1_local.y + mu * (pos_2_local.y - pos_1_local.y);
    res_pos_local.z = pos_1_local.z + mu * (pos_2_local.z - pos_1_local.z);

    res_pos_world.x = pos_1_world.x + mu * (pos_1_world.x - pos_2_world.x);
    res_pos_world.y = pos_1_world.y + mu * (pos_1_world.y - pos_2_world.y);
    res_pos_world.z = pos_1_world.z + mu * (pos_1_world.z - pos_2_world.z);
	
	res_normal.x = normal_1.x + mu * (normal_2.x - normal_1.x);
    res_normal.y = normal_1.y + mu * (normal_2.y - normal_1.y);
    res_normal.z = normal_1.z + mu * (normal_2.z - normal_1.z);
	

    //res_pos_local.w = 1;// pos_1.w > 0 ? pos_1.w : pos_2.w; // INSIDE IS POSITIVE
	//res_pos_world.w = 1;// pos_1.w > 0 ? pos_1.w : pos_2.w; // INSIDE IS POSITIVE
    type = mu < 0.5 ? p1.type : p2.type;
    type_ratios = mu < 0.5 ? p1.type_ratio : p2.type_ratio;

	

    STRUCT GridPoint gr_point = New_GridPoint();
	gr_point.iso = iso_1 > 0 ? iso_1 : iso_2;
    gr_point.localPosition = res_pos_local;
	gr_point.globalPosition = res_pos_world;
    gr_point.normal = res_normal;
    gr_point.type = type;
    gr_point.type_ratio = type_ratios;
    return gr_point;
}

void Process_Voxel(STRUCT Static_Settings static_settings, ivec4 batch, uint g_index, uint v_index, uint cubeIndex, int edge_ent, ivec2 vert_map[12] /*,STRUCT GridPoint vertList[12]*/, fvec4 grid[GRIDPOINT_STRIDE * 8], float isoLevel)
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
    //for (int i = 0; local_tritable[cubeIndex * 16 + i] != -1; i += 3)
    {

        int curTris = count;

        //STRUCT GridPoint p1 = vertList[Get_Tritable(cubeIndex * 16 + (i + 0))];
        //STRUCT GridPoint p2 = vertList[Get_Tritable(cubeIndex * 16 + (i + 1))];
        //STRUCT GridPoint p3 = vertList[Get_Tritable(cubeIndex * 16 + (i + 2))];
		
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
		fvec4 g_pos_1 = GRIDPOINT_LOCAL_POS(grid, edge1_idx.x);
		//fvec4 g_pos_2 = GRIDPOINT_LOCAL_POS(grid, edge2_idx.y);
		//fvec4 g_pos_3 = GRIDPOINT_LOCAL_POS(grid, edge3_idx.y);
		
		/*Print_4("%i: Grid Verts 1: (%f, %f, %f)\n", i,
			g_pos_1.x, g_pos_1.y, g_pos_1.z);
		Print_4("%i: Grid Verts 2: (%f, %f, %f)\n", i,
			g_pos_2.x, g_pos_2.y, g_pos_2.z);
		Print_4("%i: Grid Verts 3: (%f, %f, %f)\n", i,
			g_pos_3.x, g_pos_3.y, g_pos_3.z);*/
		
		//Print_2("Process edge: %i -> %i\n", edge1_idx.x, edge1_idx.y);
		
		VERTEX_INTERP(gv_1, isoLevel, grid, edge1_idx.x, edge1_idx.y)
		VERTEX_INTERP(gv_2, isoLevel, grid, edge2_idx.x, edge2_idx.y)
		VERTEX_INTERP(gv_3, isoLevel, grid, edge3_idx.x, edge3_idx.y)
		
		fvec4 g_pos_1_after = GRIDPOINT_LOCAL_POS(grid, edge1_idx.x);
		
		//Print_Vec_2("Before: (%f, %f, %f, %f), After: (%f, %f, %f, %f)\n", g_pos_1, g_pos_1_after);
		
		fvec4 pos1 = GRID_VERTEX_LOCAL_POS(gv_1, 0);// p1.localPosition;
		fvec3 norm1 = GRID_VERTEX_NORMAL(gv_1, 0).xyz;// p1.normal.xyz;// normal_sample(s, index_pos, p1.globalPosition.xyz);
		
		fvec4 pos2 = GRID_VERTEX_LOCAL_POS(gv_2, 0);// p2.localPosition;
		fvec3 norm2 = GRID_VERTEX_NORMAL(gv_2, 0).xyz;// p2.normal.xyz;// normal_sample(s, index_pos, p2.globalPosition.xyz);
		
		fvec4 pos3 = GRID_VERTEX_LOCAL_POS(gv_3, 0);// p3.localPosition;
		fvec3 norm3 = GRID_VERTEX_NORMAL(gv_3, 0).xyz;// p3.normal.xyz;//normal_sample(s, index_pos, p3.globalPosition.xyz);
		
		/*Print_4("%i: Poly Verts 1: (%f, %f, %f)\n", i,
			pos1.x, pos1.y, pos1.z);
		Print_4("%i: Poly Verts 2: (%f, %f, %f)\n", i,
			pos2.x, pos2.y, pos2.z);
		Print_4("%i: Poly Verts 3: (%f, %f, %f)\n", i,
			pos3.x, pos3.y, pos3.z);*/
		

        //p1.position.w = p1.type.x + p1.type_ratio.x;
        //p1.normal.w = p1.type.y + p1.type_ratio.y;

        //p2.position.w = p2.type.x + p2.type_ratio.x;
        //p2.normal.w = p2.type.y + p2.type_ratio.y;

        //p3.position.w = p3.type.x + p3.type_ratio.x;
        //p3.normal.w = p3.type.y + p3.type_ratio.y;


        //fvec3 dir = cross(p2.localPosition.xyz - p1.localPosition.xyz, p3.localPosition.xyz - p1.localPosition.xyz);
        //fvec3 norm = -normalize(dir);
		//fvec4 norm4 = FVEC4(norm.x, norm.y, norm.z, 0.0f);

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

    /*if (grid[0].iso > isoLevel) cubeIndex |= 1;
    if (grid[1].iso > isoLevel) cubeIndex |= 2;
    if (grid[2].iso > isoLevel) cubeIndex |= 4;
    if (grid[3].iso > isoLevel) cubeIndex |= 8;
    if (grid[4].iso > isoLevel) cubeIndex |= 16;
    if (grid[5].iso > isoLevel) cubeIndex |= 32;
    if (grid[6].iso > isoLevel) cubeIndex |= 64;
    if (grid[7].iso > isoLevel) cubeIndex |= 128;*/
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
	
    /*
	vertList[0] = VertexInterp(edge_ent, 1, isoLevel, grid[0], grid[1], invalid);
    vertList[1] = VertexInterp(edge_ent, 2, isoLevel, grid[1], grid[2], invalid);
    vertList[2] = VertexInterp(edge_ent, 4, isoLevel, grid[2], grid[3], invalid);
    vertList[3] = VertexInterp(edge_ent, 8, isoLevel, grid[3], grid[0], invalid);


    vertList[4] = VertexInterp(edge_ent, 16, isoLevel, grid[4], grid[5], invalid);
    vertList[5] = VertexInterp(edge_ent, 32, isoLevel, grid[5], grid[6], invalid);
    vertList[6] = VertexInterp(edge_ent, 64, isoLevel, grid[6], grid[7], invalid);
    vertList[7] = VertexInterp(edge_ent, 128, isoLevel, grid[7], grid[4], invalid);


    vertList[8] = VertexInterp(edge_ent, 256, isoLevel, grid[0], grid[4], invalid);
    vertList[9] = VertexInterp(edge_ent, 512, isoLevel, grid[1], grid[5], invalid);
    vertList[10] = VertexInterp(edge_ent, 1024, isoLevel, grid[2], grid[6], invalid);
    vertList[11] = VertexInterp(edge_ent, 2048, isoLevel, grid[3], grid[7], invalid);*/


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
    s.ChunkSizeX = ChunkSizeX + 2;
    s.ChunkSizeY = ChunkSizeY + 2;
    s.ChunkSizeZ = ChunkSizeZ + 2;
    s.VoxelsPerMeter = static_settings.VoxelsPerMeter[0];
    s.seed = 0;
    s.quality = QUALITY_STD;

    for (int i = 0; i < 8; ++i){
        /*grid[i] = GetGridPoint(
			static_settings,
            batch, 
            batch_settings, 
            v_index, 
            index_pos + Get_Direction_Offset(i),
			pos_originWorld + Get_Global_Offset(i),
            pos_localWorld + Get_Global_Offset(i), 
            i, 
            ChunkSizeX, 
            ChunkSizeY, 
            ChunkSizeZ
        );*/
		
		uint index = v_index;
		ivec4 p_index_pos = index_pos + Get_Direction_Offset(i);
		fvec4 globalWorldPos = pos_originWorld + Get_Global_Offset(i);
		fvec4 localWorldPos = pos_localWorld + Get_Global_Offset(i);
		
		uint index_exp = C_3D_to_1D(p_index_pos.x, p_index_pos.y, p_index_pos.z, ChunkSizeX + 1, ChunkSizeZ + 1);

		uint index2 = C_3D_to_1D(p_index_pos.x, p_index_pos.y, p_index_pos.z, ChunkSizeX, ChunkSizeZ);
		
		uint batch_index = GetBatchIndex((UINT(batch[1]) % UINT(batch_settings[0])), index_exp, batch[3]);
		
		float iso_val = iso_sample_3(s, p_index_pos.xyz, globalWorldPos.xyz);
		fvec3 normal = normal_sample(s, p_index_pos.xyz, globalWorldPos.xyz);
		
		//NEW_GRIDPOINT_1(gr_point)
		GRIDPOINT_ISO(grid, i) = iso_val;
		GRIDPOINT_LOCAL_POS(grid, i) = localWorldPos;
		GRIDPOINT_GLOBAL_POS(grid, i) = globalWorldPos;
		GRIDPOINT_VOXEL_ID(grid, i) = FVEC4(0, 0, 0, 0);
		GRIDPOINT_NORMAL(grid, i) = FVEC4(normal.x, normal.y, normal.z, 0);
		GRIDPOINT_TYPE(grid, i) = FVEC4(0, 0, 0, 0);
		GRIDPOINT_TYPE_RATIO(grid, i) = FVEC4(0, 0, 0, 0);
		
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
	
	uint inst_index = INT(index) / static_settings.FullChunkSize[0];
	STRUCT Run_Settings run_settings = GET_RUN_SETTINGS(inst_index);
	
	int batchIndex = run_settings.Location.w;
    int batchIndexExpanded = run_settings.Location.w;
    ivec4 batch = IVEC4(batchIndex, batchIndexExpanded, static_settings.FullChunkSize[0], static_settings.FullChunkSize[1]);

	uint voxel_index = GetVoxelIndex(batchIndex, index, batch[2]);
	
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

	//if (index == 0)
	//	Set_Debug_Data(index, FVEC4(1, 1, 1, 1));
}

#endif // _SMOOTHRENDER_CONSTRUCT_INC_