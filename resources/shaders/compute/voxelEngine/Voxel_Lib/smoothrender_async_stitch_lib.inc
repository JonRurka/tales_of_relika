#ifndef _SMOOTHRENDER_ASYNC_STITCH_INC_
#define _SMOOTHRENDER_ASYNC_STITCH_INC_

#include "voxel_lib_core.inc"

read_buffer_struct(Static_Settings, 0, in_static_settings)
read_buffer_struct(Run_Settings, 1, in_run_settings)

read_buffer(fvec4, 2, in_vertex_data)
read_buffer(fvec4, 3, in_normal_data)
read_buffer_int(4, in_tris_data)

read_buffer_int(5, in_counts_data)
read_buffer_int(6, in_stitch_map_data)

readwrite_buffer(fvec4, 7, out_vertex_data)
readwrite_buffer(fvec4, 8, out_normal_data)
readwrite_buffer_int(9, out_triangles_data)

readwrite_buffer(ivec4, 10, out_counts_data)

readwrite_buffer(fvec4, 11, out_debug_data)


void SetTriangle(uint index, int tris){
	set_buffer_scalar(out_triangles_data, index, tris);
}

int GetStitchMap(uint voxel_index){
	return get_buffer_scalar(in_stitch_map_data, voxel_index);
}

void Set_Debug_Data(uint index, fvec4 data){
	set_buffer_vector(out_debug_data, index, data);
}

void AddMeshEntry(uint buffer_pos, fvec4 vert, fvec4 norm, int tris){

	set_buffer_vector(out_vertex_data, buffer_pos, vert);
	set_buffer_vector(out_normal_data, buffer_pos, norm);

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

void Process_Voxel(ivec4 batch, uvec4 batch_settings, uint g_index, uint v_index){

    //out_debug_data[g_index].values = vec4(batch[0], g_index, v_index, 0);
    //out_debug_data[GetBatchIndex(batch[0], v_index, batch[2])].values = vec4(batch[0], g_index, v_index, 0);

    int this_count = GetCount(GetBatchIndex(batch[0], v_index, batch[2]));
    int buffer_index = GetStitchMap(GetBatchIndex(batch[0], v_index, batch[2]));

    int batch_offset = batch[0] * (batch[2] * 15);

    int dst_offset = getSourceOffset(batch[0], batch_settings[2], (v_index % batch[2]) == 0);//batch[0] * 65535;

	

    if (buffer_index == -1 || this_count == 0){
        //out_debug_data[GetBatchIndex(batch[0], v_index, batch[2])].values = vec4(g_index, buffer_index, this_count, 0);
        return;
    }

    int tri_start = buffer_index;
    //out_debug_data[GetBatchIndex(batch[0], v_index, batch[2])].values = vec4(g_index, buffer_index, this_count, tri_start);

	//Set_Debug_Data(v_index, FVEC4(v_index, buffer_index, this_count, 0));

    for (int i = 0; i < this_count; ++i){
 
        int source_index = batch_offset + (INT(v_index) * 15) + i;

        fvec4 vert = get_buffer_vector(in_vertex_data, source_index);
        fvec4 norm = get_buffer_vector(in_normal_data, source_index);
        int tris = tri_start + GetTriangle(source_index);
		
		//vert.w = dst_offset + buffer_index + i;
		//Set_Debug_Data(v_index, vert);

        AddMeshEntry(dst_offset + buffer_index + i, vert, norm, tris);
    }

    
}

void smoothrender_async_stitch_main(uint index)
{
	STRUCT Static_Settings static_settings = GET_STATIC_SETTINGS();
	
	uint inst_index = index / static_settings.FullChunkSize[0];
    STRUCT Run_Settings run_settings = in_run_settings[inst_index]; 
	
	int batchIndex = run_settings.Location.w;
    int batchIndexExpanded = run_settings.Location.w;
    ivec4 batch = ivec4(batchIndex, batchIndexExpanded, static_settings.FullChunkSize[0], static_settings.FullChunkSize[1]);
	
	uint voxel_index = GetVoxelIndex(batchIndexExpanded, index, batch[2]);

    if (run_settings.Location.w == -1){
        return;
    }
	
	Process_Voxel(batch, static_settings.batches, index, voxel_index);
	
	//if (index == 0)
	//	Set_Debug_Data(index, FVEC4(1, 1, 1, 1));
}


#endif // _SMOOTHRENDER_ASYNC_STITCH_INC_