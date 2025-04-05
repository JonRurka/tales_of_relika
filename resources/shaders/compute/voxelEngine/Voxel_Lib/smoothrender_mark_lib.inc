#ifndef _SMOOTHRENDER_MARK_INC_
#define _SMOOTHRENDER_MARK_INC_

#include "voxel_lib_core.inc"

read_buffer_struct(Static_Settings, 0, in_static_settings)

read_buffer_int(1, in_counts_data)

readwrite_buffer_int(2, out_stitch_map_data)
readwrite_buffer(ivec4, 3, out_counts_data)

readwrite_buffer(fvec4, 4, out_debug_data)


void SetStitchMap(uint voxel_index, int poly_buffer_index){
	set_buffer_scalar(out_stitch_map_data, voxel_index, poly_buffer_index);
}

int GetCount(uint index){
	return get_buffer_scalar(in_counts_data, index);
}

void Set_Debug_Data(uint index, fvec4 data){
	set_buffer_vector(out_debug_data, index, data);
}

void CreateStitchMap(uint batch_index, uint batchesPerGroup, uint fullChunkSize){
	
    uint batch_offset = (batch_index) * fullChunkSize;

    int buffer_index = 0;

    for (uint v_index = 0; v_index < fullChunkSize; ++v_index){
        
        int poly_count = GetCount(batch_offset + v_index);

        if (poly_count == 0){
            continue;
        }

        SetStitchMap(batch_offset + v_index, buffer_index);
        
        buffer_index += poly_count;
    }

	set_buffer_vector(out_counts_data, batch_index, IVEC4(buffer_index, 0, batch_index, 0));
	
}

void smoothrender_mark_main(uint batch_index){

    STRUCT Static_Settings static_settings = GET_STATIC_SETTINGS();

    // TODO: disable with run settings if location.w is -1.

    CreateStitchMap(batch_index, static_settings.batches[0], UINT(static_settings.FullChunkSize[0]));
	
	if (batch_index == 0)
		Set_Debug_Data(batch_index, FVEC4(1, 1, 1, 1));
}


#endif // _SMOOTHRENDER_MARK_INC_