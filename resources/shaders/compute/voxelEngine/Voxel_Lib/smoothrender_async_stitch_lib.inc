#ifndef _SMOOTHRENDER_ASYNC_STITCH_INC_
#define _SMOOTHRENDER_ASYNC_STITCH_INC_

#include "voxel_lib_core.inc"

#define VERT_LIMIT 40000

read_buffer_struct(Static_Settings, 0, in_static_settings)
readwrite_buffer_struct(Run_Settings, 1, in_run_settings)

read_buffer(fvec4, 2, in_vertex_data)
read_buffer(fvec4, 3, in_normal_data)
read_buffer_int(4, in_tris_data)

read_buffer_int(5, in_counts_data)
read_buffer_int(6, in_stitch_map_data)

readwrite_buffer(ivec4, 7, in_stitch_map_offsets)	

/*
#if USL_OPENCL == 1
ivec4* in_stitch_map_offsets;
#else
readwrite_buffer(ivec4, 7, in_stitch_map_offsets)	
#endif
*/

readwrite_buffer(fvec4, 8, out_vertex_data)
readwrite_buffer(fvec4, 9, out_normal_data)
readwrite_buffer(fvec4, 10, out_mat_data)
readwrite_buffer_int(11, out_triangles_data)

readwrite_buffer(ivec4, 12, out_counts_data)

readwrite_buffer(fvec4, 13, out_debug_data)


void SetTriangle(uint index, int tris){
	set_buffer_scalar(out_triangles_data, index, tris);
}

int GetStitchMap(uint voxel_index){
	return get_buffer_scalar(in_stitch_map_data, voxel_index);
}

ivec4 GetStitchOffset(uint grid_global_idx){
	return get_buffer_vector(in_stitch_map_offsets, grid_global_idx);
}

void Set_Debug_Data(uint index, fvec4 data){
	set_buffer_vector(out_debug_data, index, data);
}

void AddMeshEntry(uint buffer_pos, fvec4 vert, fvec4 norm, fvec4 mat, int tris){

	set_buffer_vector(out_vertex_data, buffer_pos, vert);
	set_buffer_vector(out_normal_data, buffer_pos, norm);
	set_buffer_vector(out_mat_data, buffer_pos, mat);

    SetTriangle(buffer_pos, tris);
}

int GetTriangle(uint index){
	return get_buffer_scalar(in_tris_data, index);
}

int GetCount(uint index){
	return get_buffer_scalar(in_counts_data, index);
}

int getSourceOffset(uint batch_num, uint total_batches, bool source){
    int result = 0;
    for (uint i = 0; i < total_batches; ++i){
        
        if (i >= batch_num){
            //if (source)
            //    out_counts_data[i].values[2] = result; // not a run condition!
            return result;
        }
        
        ivec4 cnt = get_buffer_vector(out_counts_data, i);
        result += cnt[0];
    }

    return -2147483648; // should not return here.
}

void Process_Voxel(ivec4 batch, uvec4 batch_settings, uint g_index, uint v_index, uvec4 chunkSize){

    //out_debug_data[g_index].values = vec4(batch[0], g_index, v_index, 0);
    //out_debug_data[GetBatchIndex(batch[0], v_index, batch[2])].values = vec4(batch[0], g_index, v_index, 0);

	ivec3 v_coord = C_1D_to_3D(v_index, chunkSize.x, chunkSize.y);
	ivec2 grid_coord = IVEC2(v_coord.y, v_coord.z);
	uint grid_size = (chunkSize.x * chunkSize.y);
	uint grid_local_idx = C_2D_to_1D(grid_coord.x, grid_coord.y, UINT(chunkSize.y));
	uint grid_global_idx = (batch[0] * grid_size) + grid_local_idx;
	ivec4 row_offset_data = GetStitchOffset(grid_global_idx);
	int row_offset = row_offset_data.x;
	int row_size = row_offset_data.y;

    int this_count = GetCount(GetBatchIndex(batch[0], v_index, batch[2]));
	int row_index = GetStitchMap(GetBatchIndex(batch[0], v_index, batch[2]));
    int buffer_index = row_offset + row_index;

    int batch_offset = batch[0] * (batch[2] * 15);

    int dst_offset = getSourceOffset(batch[0], batch_settings[2], (v_index % batch[2]) == 0);//batch[0] * 65535;

	//if (v_coord.x == 0 && v_coord.y == 4 && v_coord.z == 0){
		//Print_1("Stitch count: %i\n", this_count);
	//}

    if (buffer_index == -1 || this_count == 0){
        //out_debug_data[GetBatchIndex(batch[0], v_index, batch[2])].values = vec4(g_index, buffer_index, this_count, 0);
        return;
    }

    int tri_start = buffer_index;
    //out_debug_data[GetBatchIndex(batch[0], v_index, batch[2])].values = vec4(g_index, buffer_index, this_count, tri_start);

	//Set_Debug_Data(v_index, FVEC4(v_index, buffer_index, this_count, 0));

	
	int poly_idx = 0;
	for (poly_idx = 0; poly_idx < (this_count / 3); poly_idx++)
	{
		int index_1 = ((poly_idx * 3) + 0);
		int index_2 = ((poly_idx * 3) + 1);
		int index_3 = ((poly_idx * 3) + 2);
		
		int source_index_1 = batch_offset + (INT(v_index) * 15) + index_1;
		int source_index_2 = batch_offset + (INT(v_index) * 15) + index_2;
		int source_index_3 = batch_offset + (INT(v_index) * 15) + index_3;
		
		fvec4 vert_1 = get_buffer_vector(in_vertex_data, source_index_1);
        fvec4 norm_1 = get_buffer_vector(in_normal_data, source_index_1);
        int tris_1 = tri_start + GetTriangle(source_index_1);
		
		fvec4 vert_2 = get_buffer_vector(in_vertex_data, source_index_2);
        fvec4 norm_2 = get_buffer_vector(in_normal_data, source_index_2);
        int tris_2 = tri_start + GetTriangle(source_index_2);
		
		fvec4 vert_3 = get_buffer_vector(in_vertex_data, source_index_3);
        fvec4 norm_3 = get_buffer_vector(in_normal_data, source_index_3);
        int tris_3 = tri_start + GetTriangle(source_index_3);
		
		fvec2 uv_loc_1 = FVEC2(0, 0);
		fvec2 uv_loc_2 = FVEC2(1, 0);
		fvec2 uv_loc_3 = FVEC2(0.5, 1);
		
		float mat_idx_1 = vert_1.w;
		float mat_idx_2 = vert_2.w;
		float mat_idx_3 = vert_3.w;
		
		
		vert_1.w = uv_loc_1.x;
		norm_1.w = uv_loc_1.y;
		
		vert_2.w = uv_loc_2.x;
		norm_2.w = uv_loc_2.y;
		
		vert_3.w = uv_loc_3.x;
		norm_3.w = uv_loc_3.y;
		
		
		AddMeshEntry((dst_offset + buffer_index + index_1), vert_1, norm_1, FVEC4(mat_idx_1, mat_idx_2, mat_idx_3, poly_idx), tris_1);
		AddMeshEntry((dst_offset + buffer_index + index_2), vert_2, norm_2, FVEC4(mat_idx_1, mat_idx_2, mat_idx_3, poly_idx), tris_2);
		AddMeshEntry((dst_offset + buffer_index + index_3), vert_3, norm_3, FVEC4(mat_idx_1, mat_idx_2, mat_idx_3, poly_idx), tris_3);
	}

	
	//float last_mat[2] = {0, 0}; // keep two previous values.
	/*int i = 0;
    for (i = 0; i < this_count; ++i){
 
        int source_index = batch_offset + (INT(v_index) * 15) + i;

        fvec4 vert = get_buffer_vector(in_vertex_data, source_index);
        fvec4 norm = get_buffer_vector(in_normal_data, source_index);
        int tris = tri_start + GetTriangle(source_index);
		
		
		int poly_idx = tris / 3;
		int poly_vert = poly_idx % 3;
		
		// mat_idx_x Will have the same material values accross each polygon, telling which vertex in the poly had
		// what material index.
		//float mat_idx_0 = 0;
		//float mat_idx_1 = 0;
		//float mat_idx_2 = 0;
		
		fvec2 uv_loc;
		
		// TODO: Optimize all this
		switch (poly_vert){
			case 0:
				uv_loc = FVEC2(0, 0);
				mat_idx_0 = vert.w;
				mat_idx_1 = get_buffer_vector(in_vertex_data, source_index + 1).w;
				mat_idx_2 = get_buffer_vector(in_vertex_data, source_index + 2).w;
				break;
			case 1:
				uv_loc = FVEC2(1, 0);
				mat_idx_0 = last_mat[1];
				mat_idx_1 = vert.w;
				mat_idx_2 = get_buffer_vector(in_vertex_data, source_index + 1).w;
				break;
			case 2:
				uv_loc = FVEC2(0.5, 1);
				mat_idx_0 = last_mat[0];
				mat_idx_1 = last_mat[1];
				mat_idx_2 = vert.w;
				break;
		}
		
		
		//vert.w = dst_offset + buffer_index + i;
		//Set_Debug_Data(v_index, vert);
		//if (v_index == 100)
			
		//vert.w = uv_loc.x;
		//norm.w = uv_loc.y;
		
		uint dst = dst_offset + buffer_index + i;
		
		//if (v_coord.x == 0 && v_coord.y == 4 && v_coord.z == 0)
		//	Print_4("stitch vert %i: (%f, %f, %f)\n", dst, vert.x, vert.y, vert.z);
		
		//if (row_offset != 0 && (grid_local_idx == 28 || grid_local_idx == 29) ){
			//Print_5("%i: In Offset (gdx: %i): %i + %i, dst: %i\n", i, grid_local_idx, row_offset, row_index, dst);
		//}
        AddMeshEntry(dst, vert, norm, FVEC4(0, 0, 0, 0), tris);
		
		//last_mat[0] = last_mat[1];
		//last_mat[1] = vert.w;
    }*/
	
    
}

void smoothrender_async_stitch_main(uint index)
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
	
	uint voxel_index = GetVoxelIndex(batchIndexExpanded, index, batch[2]);

    if (run_settings.Location.w == -1){
        return;
    }
	
	//if (voxel_index == 0)
	//	Print_1("stitch: inst_index: %i\n", batchIndex);
	
	Process_Voxel(batch, static_settings.batches, index, voxel_index, static_settings.ChunkSize);
	
	if (index == 0){
		run_settings_ctrl.Location.x = INT(batch_group) + 1;
		
		if (run_settings_ctrl.Location.x == numBatchGroups)
		{
			run_settings_ctrl.Location.x = 0;
		}
		
		//Print_2("set batch group from %i to %i\n", batch_group, run_settings_ctrl.Location.x);
		RUN_SETTINGS_CTRL_SET(run_settings_ctrl);
	}
	
	//if (index == 0)
	//	Set_Debug_Data(index, FVEC4(1, 1, 1, 1));
}


#endif // _SMOOTHRENDER_ASYNC_STITCH_INC_