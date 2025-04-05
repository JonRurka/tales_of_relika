#ifndef _SMOOTHRENDER_CONSTRUCT_INC_
#define _SMOOTHRENDER_CONSTRUCT_INC_

#include "voxel_lib_core.inc"

read_buffer_struct(Static_Settings, 0, in_static_settings)
read_buffer_struct(Run_Settings, 1, in_run_settings)

read_buffer(fvec4, 2, in_locOffset)
read_buffer(fvec4, 3, in_globalOffsets)
read_buffer(ivec4, 4, in_directionOffsets)

read_buffer_int(5, in_edgeTable_data)
read_buffer_int(6, in_TriTable_data)

readwrite_buffer_struct(ISO_Material, 7, in_iso_mat_data)

readwrite_buffer(fvec4, 8, out_vertex_data)
readwrite_buffer(fvec4, 9, out_normal_data)
readwrite_buffer_int(10, out_triangles_data)
readwrite_buffer_int(11, out_counts_data)
readwrite_buffer_int(12, out_stitch_map_data)

readwrite_buffer(fvec4, 13, out_debug_data)

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

    count++;

    return count;
}

STRUCT GridPoint GetGridPoint(
    ivec4 batch, 
    uvec4 batch_settings, 
    uint index, 
    ivec4 index_pos, 
    fvec4 localWorldPos, 
    uint i,
    uint ChunkSizeX, 
    uint ChunkSizeY, 
    uint ChunkSizeZ
){
	uint index_exp = C_3D_to_1D(index_pos.x, index_pos.y, index_pos.z, ChunkSizeX + 1, ChunkSizeZ + 1);

    uint index2 = C_3D_to_1D(index_pos.x, index_pos.y, index_pos.z, ChunkSizeX, ChunkSizeZ);
	
	uint batch_index = GetBatchIndex((UINT(batch[1]) % UINT(batch_settings[0])), index_exp, batch[3]);
	STRUCT ISO_Material iso_mat = get_buffer_object(in_iso_mat_data, batch_index);
	
	STRUCT GridPoint gr_point = New_GridPoint();
    gr_point.position = FVEC4(localWorldPos.x, localWorldPos.y, localWorldPos.z, iso_mat.final_iso.w);
    gr_point.normal = FVEC4(iso_mat.final_iso.x, iso_mat.final_iso.y, iso_mat.final_iso.z, 0);
    gr_point.type = iso_mat.material_types;
    gr_point.type_ratio = iso_mat.material_ratios;

    return gr_point;
}

STRUCT GridPoint VertexInterp(int edge_ent, int test, float isoLevel, GridPoint p1, GridPoint p2, bool invalid){
    if ((edge_ent & test) == 0 || invalid){
        STRUCT GridPoint gr_point = New_GridPoint();
        gr_point.position = FVEC4(0, 0, 0, -1000000); // invalid
        return gr_point;
    }

    
    fvec4 pos_1 = p1.position;
    fvec4 norm_1 = p1.normal;

    fvec4 pos_2 = p2.position;
    fvec4 norm_2 = p2.normal;

    if (abs(isoLevel - pos_1.w) < 0.00001f)
        return p1;
    if (abs(isoLevel - pos_2.w) < 0.00001f)
        return p2;
    if (abs(pos_1.w - pos_2.w) < 0.00001f)
        return p1;

    float mu = (isoLevel - pos_1.w) / (pos_2.w - pos_1.w);

    
    fvec4 res_pos = FVEC4_ZERO;
    fvec4 res_norm = FVEC4_ZERO;
    ivec4 type = IVEC4_ZERO;
    fvec4 type_ratios = FVEC4_ZERO;

    res_pos.x = pos_1.x + mu * (pos_2.x - pos_1.x);
    res_pos.y = pos_1.y + mu * (pos_2.y - pos_1.y);
    res_pos.z = pos_1.z + mu * (pos_2.z - pos_1.z);

    res_norm.x = norm_1.x + mu * (norm_2.x - norm_1.x);
    res_norm.y = norm_1.y + mu * (norm_2.y - norm_1.y);
    res_norm.z = norm_1.z + mu * (norm_2.z - norm_1.z);

    res_pos.w = pos_1.w > 0 ? pos_1.w : pos_2.w; // INSIDE IS POSITIVE
    type = mu < 0.5 ? p1.type : p2.type;
    type_ratios = mu < 0.5 ? p1.type_ratio : p2.type_ratio;


    STRUCT GridPoint gr_point = New_GridPoint();
    gr_point.position = res_pos;
    gr_point.normal = res_norm;
    gr_point.type = type;
    gr_point.type_ratio = type_ratios;
    return gr_point;
}

void Process_Voxel(ivec4 batch, uint g_index, uint v_index, uint cubeIndex, int edge_ent, STRUCT GridPoint vertList[12])
{
	if(edge_ent == 0){
        return;
    }

    int count = 0;
	
	uint tritble_start = cubeIndex * 16;
	
	for (int i = 0; Get_Tritable(cubeIndex * 16 + i) != -1; i += 3)
    //for (int i = 0; local_tritable[cubeIndex * 16 + i] != -1; i += 3)
    {

        int curTris = INT(count);

        STRUCT GridPoint p1 = vertList[Get_Tritable(cubeIndex * 16 + (i + 0))];
        STRUCT GridPoint p2 = vertList[Get_Tritable(cubeIndex * 16 + (i + 1))];
        STRUCT GridPoint p3 = vertList[Get_Tritable(cubeIndex * 16 + (i + 2))];

        p1.position.w = p1.type.x + p1.type_ratio.x;
        p1.normal.w = p1.type.y + p1.type_ratio.y;

        p2.position.w = p2.type.x + p2.type_ratio.x;
        p2.normal.w = p2.type.y + p2.type_ratio.y;

        p3.position.w = p3.type.x + p3.type_ratio.x;
        p3.normal.w = p3.type.y + p3.type_ratio.y;


        //vec3 dir = cross(p2.xyz - p1.xyz, p3.xyz - p1.xyz);
        //vec3 norm = -normalize(dir);

        //vec4 n1 = vec4(norm.x, norm.y, norm.z, 0);
        int t1 = INT(curTris) + 2;
        count = SetMeshEntry(GetBatchIndex(batch[0], v_index, batch[2]), p1.position, p1.normal, t1, count);
        
        //vec4 n2 = vec4(norm.x, norm.y, norm.z, 0);
        int t2 = INT(curTris) + 1;
        count = SetMeshEntry(GetBatchIndex(batch[0], v_index, batch[2]), p2.position, p2.normal, t2, count);
        
        //vec4 n3 = vec4(norm.x, norm.y, norm.z, 0);
        int t3 = INT(curTris) + 0;
        count = SetMeshEntry(GetBatchIndex(batch[0], v_index, batch[2]), p3.position, p3.normal, t3, count);

    }

    Set_Count(GetBatchIndex(batch[0], v_index, batch[2]), count);
	
}

void CreateVertList(ivec4 batch, uint g_index, uint v_index, STRUCT GridPoint grid[8], float isoLevel, uint ChunkSizeX, uint ChunkSizeY, uint ChunkSizeZ){

    STRUCT GridPoint vertList[12];
    int cubeIndex = 0;

    /*SetDebug_Grid(index, 0, grid[0]);
    SetDebug_Grid(index, 1, grid[1]);
    SetDebug_Grid(index, 2, grid[2]);
    SetDebug_Grid(index, 3, grid[3]);
    SetDebug_Grid(index, 4, grid[4]);
    SetDebug_Grid(index, 5, grid[5]);
    SetDebug_Grid(index, 6, grid[6]);
    SetDebug_Grid(index, 7, grid[7]);*/

    if (grid[0].position.w > isoLevel) cubeIndex |= 1;
    if (grid[1].position.w > isoLevel) cubeIndex |= 2;
    if (grid[2].position.w > isoLevel) cubeIndex |= 4;
    if (grid[3].position.w > isoLevel) cubeIndex |= 8;
    if (grid[4].position.w > isoLevel) cubeIndex |= 16;
    if (grid[5].position.w > isoLevel) cubeIndex |= 32;
    if (grid[6].position.w > isoLevel) cubeIndex |= 64;
    if (grid[7].position.w > isoLevel) cubeIndex |= 128;

    int edge_ent = Get_Edgetable(cubeIndex);

	//Set_Debug_Data(g_index, FVEC4(grid[0].position.w, grid[1].position.w, grid[2].position.w, grid[3].position.w));
	//Set_Debug_Data(g_index, FVEC4(grid[4].position.w, grid[5].position.w, grid[6].position.w, grid[7].position.w));
	//Set_Debug_Data(g_index, FVEC4(g_index, FLOAT(cubeIndex), edge_ent, 0));

    bool invalid = (edge_ent == 0);

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
    vertList[11] = VertexInterp(edge_ent, 2048, isoLevel, grid[3], grid[7], invalid);


    Process_Voxel(batch, g_index, v_index, cubeIndex, edge_ent, vertList);
    
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
    int xStart = cx * int(ChunkSizeX);
    int yStart = cy * int(ChunkSizeY);
    int zStart = cz * int(ChunkSizeZ);

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
    STRUCT GridPoint grid[8];
	

    for (int i = 0; i < 8; ++i){
        grid[i] = GetGridPoint(
            batch, 
            batch_settings, 
            v_index, 
            index_pos + get_buffer_vector(in_directionOffsets, i), 
            pos_localWorld + get_buffer_vector(in_globalOffsets, i), 
            i, 
            ChunkSizeX, 
            ChunkSizeY, 
            ChunkSizeZ
        );
    }

    Set_Count(GetBatchIndex(batch[0], v_index, batch[2]), 0);
    SetStitchMap(GetBatchIndex(batch[0], v_index, batch[2]), -1);

    CreateVertList(batch, g_index, v_index, grid, 0, ChunkSizeX, ChunkSizeY, ChunkSizeZ);
    
}

void smoothrender_construct_main(uint index)
{
	STRUCT Static_Settings static_settings = GET_STATIC_SETTINGS();
	
	uint inst_index = int(index) / static_settings.FullChunkSize[0];
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

	if (index == 0)
		Set_Debug_Data(index, FVEC4(1, 1, 1, 1));
}

#endif // _SMOOTHRENDER_CONSTRUCT_INC_