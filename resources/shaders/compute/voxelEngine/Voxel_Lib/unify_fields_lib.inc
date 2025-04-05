#ifndef _UNIFY_FIELDS_INC_
#define _UNIFY_FIELDS_INC_


#define USE_HEIGHTMAP 1
#define USE_MATERIALS 1
#define GEN_CRATER_MAP 1

#include "voxel_lib_core.inc"
#include "sampler.inc"

#define SURFACE_TYPE_FLOOR 1
#define SURFACE_TYPE_WALL 2
#define SURFACE_TYPE_CEILING 3

#define Surface_Type int

read_buffer_struct(Static_Settings, 0, in_static_settings)
read_buffer_struct(Run_Settings, 1, in_run_settings)

read_buffer_float(2, in_heightmap_data)
read_buffer(fvec4, 3, in_iso_data)
read_buffer_struct(Materials, 4, in_material_data)

readwrite_buffer_struct(ISO_Material, 5, out_data)

readwrite_buffer(fvec4, 6, out_debug_data)

fvec4 getISOval(uint index){
	return get_buffer_vector(in_iso_data, index);
	
    //return in_iso_data[index].values;
	
    /*uint buffer_index = index / 4;
	uint access_index = index % 4;

    return in_iso_data[buffer_index].values[access_index];*/
}

void Set_Debug_Data(uint index, fvec4 data){
	set_buffer_vector(out_debug_data, index, data);
}

float getHeightVal(uint index){
	return get_buffer_scalar(in_heightmap_data, index);
	
    /*uint buffer_index = index / 4;
	uint access_index = index % 4;
	
	return in_heightmap_data[buffer_index].values[access_index];*/
}

float iso_sample_3D(float x, float y, float z, STRUCT Settings s){

    return Sample_3D(s, x, y, z);

    //float radius = 10;
    //return sphere_sample(radius, x, y, z);
}

float iso_sample_3D(fvec3 p, STRUCT Settings s){
    return iso_sample_3D(p.x, p.y, p.z,  s);
}

float height_sample(float x, float y, STRUCT Settings s){

    return Sample_2D(s, x, y);
}

float height_sample(fvec2 p, STRUCT Settings s){
    return height_sample(p.x, p.y, s);
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

float iso_sample(STRUCT Settings s, ivec3 p, fvec3 world_pos, uint size_x, uint size_z, uint fullSize, uint heights_size, uint batch, uint batchesPerGroup){
    //int voxel_index = C_3D_to_1D(p.x, p.y, p.z, size_x, size_z);
    //int height_index = C_2D_to_1D(p.x, p.y, size_z);

    //vec3 world_pos = FVEC3(global_pos.x * s.VoxelsPerMeter, global_pos.y * s.VoxelsPerMeter, global_pos.z * s.VoxelsPerMeter);

    float iso = iso_sample_3D(world_pos, s);
    float height = height_sample(world_pos.x, world_pos.y, s);
    
    //float iso = getISOval(GetBatchIndex((batch % batchesPerGroup), voxel_index, fullSize)).w;
    //float height = getHeightVal(GetBatchIndex((batch % batchesPerGroup), height_index, heights_size));
    // TODO: sample edit field.

    return Combine_ISO(s, p, world_pos, iso, height, 0);
}

bool is_surface(STRUCT Settings s, ivec3 p, fvec3 world_pos, float centerDistance, uint size_x, uint size_z, uint fullSize, uint heights_size, uint batch, uint batchesPerGroup){
    float epsilon = 1;

    float x1 = iso_sample(s, p - IVEC3(1, 0, 0), world_pos - FVEC3(epsilon, 0, 0), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
    float x2 = iso_sample(s, p + IVEC3(1, 0, 0), world_pos + FVEC3(epsilon, 0, 0), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
    bool crosses_x = (x1 >= 0 && x2 < 0) || (x1 <= 0 && x2 > 0);

    float y1 = iso_sample(s, p - IVEC3(0, 1, 0), world_pos - FVEC3(0, epsilon, 0), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
    float y2 = iso_sample(s, p + IVEC3(0, 1, 0), world_pos + FVEC3(0, epsilon, 0), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
    bool crosses_y = (y1 >= 0 && y2 < 0) || (y1 <= 0 && y2 > 0);

    float z1 = iso_sample(s, p - IVEC3(0, 0, 1), world_pos - FVEC3(0, 0, epsilon), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
    float z2 = iso_sample(s, p + IVEC3(0, 0, 1), world_pos + FVEC3(0, 0, epsilon), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
    bool crosses_z = (z1 >= 0 && z2 < 0) || (z1 <= 0 && z2 > 0);

    // TODO: corners

    return (crosses_x || crosses_y || crosses_z);
}

fvec3 normal_sample(STRUCT Settings s, uint g_index, ivec3 p, fvec3 world_pos, float centerDistance, uint size_x, uint size_z, uint fullSize, uint heights_size, uint batch, uint batchesPerGroup){
    float epsilon = 0.001;

    if ((p.x > 0 && p.y > 0 && p.z > 0) || true){
        float x1 = iso_sample(s, p - IVEC3(1, 0, 0), world_pos - FVEC3(epsilon, 0, 0), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
        float x2 = iso_sample(s, p + IVEC3(1, 0, 0), world_pos + FVEC3(epsilon, 0, 0), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);

        float y1 = iso_sample(s, p - IVEC3(0, 1, 0), world_pos - FVEC3(0, epsilon, 0), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
        float y2 = iso_sample(s, p + IVEC3(0, 1, 0), world_pos + FVEC3(0, epsilon, 0), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);

        float z1 = iso_sample(s, p - IVEC3(0, 0, 1), world_pos - FVEC3(0, 0, epsilon), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
        float z2 = iso_sample(s, p + IVEC3(0, 0, 1), world_pos + FVEC3(0, 0, epsilon), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
	
		//Set_Debug_Data(g_index, FVEC4(y1, y2, y1 - y2, 1));
	
		fvec4 res = FVEC4((x1 - x2), (y1 - y2), (z1 - z2), 0);
		//Set_Debug_Data(g_index, res);
		
		res = normalize(res);
		Set_Debug_Data(g_index, res);
		
        return res.xyz;
    }
    else{
        //vec3 p = vec3(x, y, z);
        //int epsilon = 1; // arbitrary - should be smaller than any surface detail in your distance function, but not so small as to get lost in float precision
        //float centerDistance = iso_sample(p);
        float xDistance = iso_sample(s, p + IVEC3(1, 0, 0), world_pos + FVEC3(epsilon, 0, 0), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
        float yDistance = iso_sample(s, p + IVEC3(0, 1, 0), world_pos + FVEC3(0, epsilon, 0), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
        float zDistance = iso_sample(s, p + IVEC3(0, 0, 1), world_pos + FVEC3(0, 0, epsilon), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
        fvec3 normal = (FVEC3(xDistance - centerDistance, yDistance - centerDistance, zDistance - centerDistance)) / epsilon;
        return -normal;
    }

    /*float xDistance = iso_sample(s, p + IVEC3(epsilon, 0, 0), global_pos + IVEC3(epsilon, 0, 0), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
    float yDistance = iso_sample(s, p + IVEC3(0, epsilon, 0), global_pos + IVEC3(0, epsilon, 0), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
    float zDistance = iso_sample(s, p + IVEC3(0, 0, epsilon), global_pos + IVEC3(0, 0, epsilon), size_x, size_z, fullSize, heights_size, batch, batchesPerGroup);
    fvec3 normal = (FVEC3(xDistance - centerDistance, yDistance - centerDistance, zDistance - centerDistance)) / epsilon;
    return -normal;*/
}

float log_delta(float delta){
    //return 0.5 * log(0.99 * delta + 0.01) + 1;
    float A = 0.197382;
    float S = 23.6439;
    float O = -0.150036;
    float C = 0.37441;
    return A * log(S * delta + O) + C;
}

float pow_delta(float delta){
    float A = 2.18203;
    float S = 6.79111;
    float O = -6.78469;
    float C = -0.0050226;
    return pow(A, S * delta + O) + C;
}

float cube_root_delta(float delta){
    float A = 0.37;
    float S = 5;
    float O = -2.5;
    float C = 0.5;
    return A * pow(S * delta + O, 1 / float(3)) + C;
}

fvec4 material_sample(float iso, fvec3 normal, fvec3 world_pos, bool isSurface, Surface_Type surface_type){

#if GEN_CRATER_MAP == 1

    int type_1 = 0;
    int type_2 = 0;
    float type_delta_1 = 1;
    float type_delta_2 = 0;

    float stand_top_offset = 60;
    float stand_bottom = -30;

    STRUCT Module dirt_patch = GetModule(world_pos.x * 0.02, 0, world_pos.y * 0.035);
    dirt_patch.m_seed = 5346;
    dirt_patch.m_frequency = 0.7;
    dirt_patch.m_noiseQuality = QUALITY_STD;
    dirt_patch = Perlin_GetValue(dirt_patch);

    STRUCT Module gravel_patch = GetModule(world_pos.x * 0.02, 0, world_pos.y * 0.02);
    gravel_patch.m_seed = 2054;
    gravel_patch.m_frequency = 0.3;
    gravel_patch.m_noiseQuality = QUALITY_STD;
    gravel_patch = Perlin_GetValue(gravel_patch);


    if (isSurface && 
        surface_type == SURFACE_TYPE_FLOOR){

        if (world_pos.z < (CRATER_BOTTOM + stand_top_offset) && world_pos.z > (CRATER_BOTTOM + stand_bottom)){

            type_1 = 0;
            type_2 = 2;

            float blendStart = CRATER_BOTTOM + 4;
            float blendEnd = CRATER_BOTTOM + stand_top_offset;

            float sand_blend_delta = clamp(abs(blendEnd - clamp(world_pos.z, blendStart, blendEnd)) / abs(blendEnd - blendStart), 0, 1);



            float dir_patch_val = Scale(dirt_patch.Value * sand_blend_delta, -1, 1, 0, 1);

            float patch_noise = SCurve5(dir_patch_val); //SCurve5

            type_delta_2 = patch_noise;
            type_delta_1 = 1 - patch_noise;
        }

        if (world_pos.z < (CRATER_BOTTOM + stand_bottom + 5)){
            type_1 = 0;
            type_2 = 3;

            float gravel_patch_val = Scale(gravel_patch.Value, -1, 1, 0, 1);

            float patch_noise = cube_root_delta(gravel_patch_val); //SCurve5

            type_delta_2 = patch_noise;
            type_delta_1 = 1 - patch_noise;
        }

    }

    STRUCT Module stone_region_start_patch = GetModule(world_pos.x * 0.025, 0, world_pos.y * 0.025);
    stone_region_start_patch.m_seed = 6939;
    stone_region_start_patch.m_frequency = 0.3;
    stone_region_start_patch.m_noiseQuality = QUALITY_STD;
    stone_region_start_patch = Perlin_GetValue(stone_region_start_patch);
    float stone_region_start_patch_val = stone_region_start_patch.Value * 20;

    if (world_pos.z < (CRATER_BOTTOM + stand_bottom - 10 + stone_region_start_patch_val)){
        type_1 = 4;
        type_2 = 4;

        type_delta_1 = 1;
        type_delta_2 = 0;
    }

    float n1 = GetSandStart(world_pos.x, world_pos.y);
    float n2 = GetSandEnd(world_pos.x, world_pos.y);

    if (isInSandBumpRegion(world_pos.x, world_pos.y, n1 - 10, n2 + 3) &&
        world_pos.z > CRATER_BOTTOM){
        
        float gravel_blend = gravel_patch.Value;

        type_1 = 2;
        type_2 = 3;
        type_delta_1 = gravel_blend;
        type_delta_2 = 1 - gravel_blend;
    }


    /*type_1 = 3;
    type_2 = 3;
    type_delta_2 = 1;
    type_delta_1 = 0;*/

    return FVEC4(type_1, type_delta_1, type_2, type_delta_2);
	
#else
	return FVEC4_ZERO;
#endif

}

void unify_fields_main(uint index){
	
	STRUCT Static_Settings static_settings = GET_STATIC_SETTINGS();
	
	uint ChunkSizeX = static_settings.ChunkSize.x;
    uint ChunkSizeY = static_settings.ChunkSize.y;
    uint ChunkSizeZ = static_settings.ChunkSize.z;
	
	uint FullChunkSize = (ChunkSizeX) * (ChunkSizeY) * (ChunkSizeZ);
    uint FullChunkSize_src_exp = (ChunkSizeX + 2) * (ChunkSizeY + 2) * (ChunkSizeZ + 2);
    uint FullChunkSize_dst_exp = (ChunkSizeX + 1) * (ChunkSizeY + 1) * (ChunkSizeZ + 1);
	
	uint inst_index = index / FullChunkSize_dst_exp;
    STRUCT Run_Settings run_settings = GET_RUN_SETTINGS(inst_index);
	
	uint numBatchesPerGroup = static_settings.batches[0];
	
	if (run_settings.Location.w == -1 || index >= (FullChunkSize_dst_exp * numBatchesPerGroup)){
        return;
    }
	
	uint batchIndex = run_settings.Location.w;
    uint batchIndexExpanded = run_settings.Location.w;
    ivec4 batch = IVEC4(batchIndex, batchIndexExpanded, static_settings.FullChunkSize[0], static_settings.FullChunkSize[1]);
	
	uint heightmap_size = (ChunkSizeX + 2) * (ChunkSizeY + 2);
	
	// Index of voxel in ISO_Material (expanded by 1)
    uint dst_voxel_index = GetVoxelIndex(batchIndexExpanded, index, FullChunkSize_dst_exp);
	
	// 3D vector location of voxel inside ISO_Material (expanded by 1).
    ivec3 dst_voxel_pos = C_1D_to_3D(dst_voxel_index, ChunkSizeX + 1, ChunkSizeY + 1);
	
	// Index of voxel inside iso data (expanded by 2).
    uint src_voxel_index = C_3D_to_1D(dst_voxel_pos.x, dst_voxel_pos.y, dst_voxel_pos.z, ChunkSizeX + 2, ChunkSizeY + 2);
	
	// 3D vector location of voxel inside iso data (expanded by 2).
    ivec3 src_voxel_pos = C_1D_to_3D(src_voxel_index, ChunkSizeX + 2, ChunkSizeY + 2);
	
	Settings s = New_Settings();
    s.ChunkSizeX = ChunkSizeX + 2;
    s.ChunkSizeY = ChunkSizeY + 2;
    s.ChunkSizeZ = ChunkSizeZ + 2;
    s.VoxelsPerMeter = static_settings.VoxelsPerMeter[0];
    s.seed = 0;
    s.quality = QUALITY_STD;
	
	ISO_Material mat_point = New_ISO_Material();
	
	float xSideLength = static_settings.SideLength.x;
    float ySideLength = static_settings.SideLength.y;
    float zSideLength = static_settings.SideLength.z;
	
	// Coordinate of voxel
    int x = src_voxel_pos.x;
    int y = src_voxel_pos.y;
    int z = src_voxel_pos.z;
	
	// Coordinate of Chunk
    int cx = run_settings.Location.x;
    int cy = run_settings.Location.y;
    int cz = run_settings.Location.z;
	
	// Global coordinate of first voxel in chunk.
    int xStart = cx * INT(ChunkSizeX);
    int yStart = cy * INT(ChunkSizeY);
    int zStart = cz * INT(ChunkSizeZ);
	
	// Global coordinate of voxel
    int global_X = xStart + x;
    int global_Y = yStart + y;
    int global_Z = zStart + z;
    ivec3 global_pos = IVEC3(global_X, global_Y, global_Z);
	
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
	
	
    float iso = iso_sample(s, src_voxel_pos, pos_originWorld, ChunkSizeX + 2, ChunkSizeY + 2, FullChunkSize_src_exp, heightmap_size, batchIndexExpanded, static_settings.batches[0]); //getISOval(GetBatchIndex((batchIndexExpanded % static_settings.batches[0]), voxel_index, batch[3])).w;
	
	//vec4 iso_normal = getISOval(GetBatchIndex((batchIndexExpanded % static_settings.batches[0]), voxel_index, batch[3]));
    fvec3 normal = normal_sample(s, index, src_voxel_pos, pos_originWorld, iso, ChunkSizeX + 2, ChunkSizeY + 2, FullChunkSize_src_exp, heightmap_size, batchIndexExpanded, static_settings.batches[0]);
    fvec4 iso_normal = FVEC4(normal.x, normal.y, normal.z, iso);
	
	//Set_Debug_Data(index, FVEC4(iso, 1, 1, 1));
	
	mat_point.final_iso = iso_normal;
	
	bool isSurface = is_surface(s, src_voxel_pos, pos_originWorld, iso, ChunkSizeX + 2, ChunkSizeY + 2, FullChunkSize_src_exp, heightmap_size, batchIndexExpanded, static_settings.batches[0]);
	
	// TODO: Might be good generate materials in here.
    // In fact, it might be good to just generate the 3D iso in here...
	
#if USE_MATERIALS == 1
	
	Surface_Type surface_type = -1;
    float angle = vector_angle(FVEC3(0, 0, 1), normal);
    if (angle > 0 && angle <= 45){
        surface_type = SURFACE_TYPE_FLOOR;
    } else if(angle > 45 && angle <= 135){
        surface_type = SURFACE_TYPE_WALL;
    }else if (angle > 135){
        surface_type = SURFACE_TYPE_CEILING;
    }
	
	
	fvec4 mat = material_sample(iso, normal, pos_originWorld, isSurface, surface_type);
    mat_point.material_types = IVEC4(INT(mat.x), INT(mat.z), 0, 0);
    mat_point.material_ratios = FVEC4(clamp(mat.y, 0, 0.999), clamp(mat.w, 0, 0.999), 0, 0);
#endif
	
	//point.material_types = in_material_data[GetBatchIndex((batchIndexExpanded % static_settings.batches[0]), src_voxel_index, src_voxel_index)].material_types;
    //point.material_ratios = in_material_data[GetBatchIndex((batchIndexExpanded % static_settings.batches[0]), src_voxel_index, src_voxel_index)].material_ratios;

    //out_debug_data[index].values = vec4(voxel_index, batchIndexExpanded, index, batch[3]);
    //out_debug_data[GetBatchIndex(batchIndexExpanded, voxel_index, batch[3])].values = vec4(batchIndex, index, voxel_index, inst_index);

    //out_data[index] = mat_point;
	uint batch_index = GetBatchIndex((batchIndexExpanded % static_settings.batches[0]), dst_voxel_index, batch[3]);
    set_buffer_object(out_data, batch_index, mat_point);
	
	//if (index == 0)
	//	Set_Debug_Data(index, FVEC4(1, 1, 1, 1));
}


#endif // _UNIFY_FIELDS_INC_




