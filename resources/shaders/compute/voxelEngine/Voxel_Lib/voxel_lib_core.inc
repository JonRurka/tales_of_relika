#ifndef _VOXEL_LIB_CORE_INC_
#define _VOXEL_LIB_CORE_INC_

#include "../../libUSL/USL.inc"

#ifdef USE_LIBNOISE
#include "../../libnoise/libnoise.inc"
#endif

struct Static_Settings{
    fvec4 VoxelsPerMeter;
    uvec4 ChunkMeterSize;
    uvec4 ChunkSize;
    uvec4 FullChunkSize;
    fvec4 skipDist;
    fvec4 half_;
    fvec4 SideLength;
    uvec4 batches;
};

struct Run_Settings{
    ivec4 Location;
};

struct Materials{
    ivec4 material_types;
    fvec4 material_ratios;
};

struct ISO_Material{
    fvec4 final_iso;
    ivec4 material_types;
    fvec4 material_ratios;
};

struct GridPoint{
	float iso;
    fvec4 localPosition;
	fvec4 globalPosition;
	ivec3 voxelIndex;
    fvec4 normal;
    ivec4 type;
    fvec4 type_ratio;
};

// #### Start VERTEX ####
#define GRID_VERTEX_STRIDE 2
#define GRID_VERTEX_IDX(i) ((i) * GRID_VERTEX_STRIDE)

#define GRID_VERTEX_LOCAL_POS(arr, i) 	(arr[GRID_VERTEX_IDX(i) + 0])
#define GRID_VERTEX_NORMAL(arr, i) 		(arr[GRID_VERTEX_IDX(i) + 1])

#define NEW_GRID_VERTEX_1(name) \
fvec4 name[GRID_VERTEX_STRIDE * 1];
// #### End VERTEX ####


// #### Start Grid Point ####
#define GRIDPOINT_STRIDE 7
#define GRIDPOINT_IDX(i) ((i) * GRIDPOINT_STRIDE)

#define GRIDPOINT_ISO(arr, i) 			(arr[GRIDPOINT_IDX(i) + 0].x)
#define GRIDPOINT_LOCAL_POS(arr, i) 	(arr[GRIDPOINT_IDX(i) + 1])
#define GRIDPOINT_GLOBAL_POS(arr, i)	(arr[GRIDPOINT_IDX(i) + 2])
#define GRIDPOINT_VOXEL_ID(arr, i) 		(arr[GRIDPOINT_IDX(i) + 3])
#define GRIDPOINT_NORMAL(arr, i) 		(arr[GRIDPOINT_IDX(i) + 4])
#define GRIDPOINT_TYPE(arr, i) 			(arr[GRIDPOINT_IDX(i) + 5])
#define GRIDPOINT_TYPE_RATIO(arr, i) 	(arr[GRIDPOINT_IDX(i) + 6])

#define GRIDPOINT_ARRAY_GET(dst, arr, i) \
GRIDPOINT_ISO(dst, 0) = GRIDPOINT_ISO(arr, i); \
GRIDPOINT_LOCAL_POS(dst, 0) = GRIDPOINT_LOCAL_POS(arr, i); \
GRIDPOINT_GLOBAL_POS(dst, 0) = GRIDPOINT_GLOBAL_POS(arr, i); \
GRIDPOINT_VOXEL_ID(dst, 0) = GRIDPOINT_VOXEL_ID(arr, i); \
GRIDPOINT_NORMAL(dst, 0) = GRIDPOINT_NORMAL(arr, i); \
GRIDPOINT_TYPE(dst, 0) = GRIDPOINT_TYPE(arr, i); \
GRIDPOINT_TYPE_RATIO(dst, 0) = GRIDPOINT_TYPE_RATIO(arr, i);

#define GRIDPOINT_ARRAY_ASSIGN(dst, src, arr) \
GRIDPOINT_ISO(arr, dst) = GRIDPOINT_ISO(arr, src); \
GRIDPOINT_LOCAL_POS(arr, dst) = GRIDPOINT_LOCAL_POS(arr, src); \
GRIDPOINT_GLOBAL_POS(arr, dst) = GRIDPOINT_GLOBAL_POS(arr, src); \
GRIDPOINT_VOXEL_ID(arr, dst) = GRIDPOINT_VOXEL_ID(arr, src); \
GRIDPOINT_NORMAL(arr, dst) = GRIDPOINT_NORMAL(arr, src); \
GRIDPOINT_TYPE(arr, dst) = GRIDPOINT_TYPE(arr, src); \
GRIDPOINT_TYPE_RATIO(arr, dst) = GRIDPOINT_TYPE_RATIO(arr, src);

#define GRIDPOINT_ARRAY_GET_NEW(name, arr, i) \
fvec4 name[GRIDPOINT_STRIDE]; \
GRIDPOINT_ARRAY_GET(name, arr, i)

#define GRIDPOINT_ARRAY_SET(arr, i, src) \
GRIDPOINT_ISO(arr, i) = GRIDPOINT_ISO(src, 0); \
GRIDPOINT_LOCAL_POS(arr, i) = GRIDPOINT_LOCAL_POS(src, 0); \
GRIDPOINT_GLOBAL_POS(arr, i) = GRIDPOINT_GLOBAL_POS(src, 0); \
GRIDPOINT_VOXEL_ID(arr, i) = GRIDPOINT_VOXEL_ID(src, 0) ;\
GRIDPOINT_NORMAL(arr, i) = GRIDPOINT_NORMAL(src, 0); \
GRIDPOINT_TYPE(arr, i) = GRIDPOINT_TYPE(src, 0); \
GRIDPOINT_TYPE_RATIO(arr, i) = GRIDPOINT_TYPE_RATIO(src, 0); \

#define NEW_GRIDPOINT_1(name) \
fvec4 name[GRIDPOINT_STRIDE * 1]; \
GRIDPOINT_ISO(name, 0) = FVEC4_ZERO; \
GRIDPOINT_LOCAL_POS(name, 0) = FVEC4_ZERO; \
GRIDPOINT_GLOBAL_POS(name, 0) = FVEC4_ZERO; \
GRIDPOINT_VOXEL_ID(name, 0) = FVEC4_ZERO; \
GRIDPOINT_NORMAL(name, 0) = FVEC4_ZERO; \
GRIDPOINT_TYPE(name, 0) = FVEC4_ZERO; \
GRIDPOINT_TYPE_RATIO(name, 0) = FVEC4_ZERO;

#define NEW_GRIDPOINT_ARRAY(name, elements) \
fvec4 name[GRIDPOINT_STRIDE * elements];
// #### End Grid Point ####


// #### Start ISO_Material ####
#define ISO_MATERIAL_STRIDE 7
#define ISO_MATERIAL_IDX(i) (i * ISO_MATERIAL_STRIDE)

#define ISO_MATERIAL_ISO(arr, i) (arr[ISO_MATERIAL_IDX(i) + 0].x)
#define ISO_MATERIAL_TYPES(arr, i) (arr[ISO_MATERIAL_IDX(i) + 1])
#define ISO_MATERIAL_RATIOS(arr, i) (arr[ISO_MATERIAL_IDX(i) + 2])


#define NEW_ISO_MATERIAL_1(name) \
fvec4 name[ISO_MATERIAL_STRIDE * 1]; \
ISO_MATERIAL_ISO(name, 0) = 0; \
ISO_MATERIAL_TYPES(name, 0) = FVEC4_ZERO; \
ISO_MATERIAL_RATIOS(name, 0) = FVEC4_ZERO;
// #### End ISO_Material ####



struct Polygon{
	fvec4 vertices[3];
	fvec4 normal;
};

#define VEC3_TO_4(i) (FVEC4(i.x, i.y, i.z, 0))

#define GET_STATIC_SETTINGS() get_buffer_object(in_static_settings, 0)

#define GET_RUN_SETTINGS(inst_index) get_buffer_object(in_run_settings, inst_index)

#define RUN_SETTINGS_STRIDE 1
#define RUN_SETTINGS_IDX(i) ((i) * RUN_SETTINGS_STRIDE)
#define RUN_SETTINGS_LOCATION(inst_index) get_buffer_vector(in_run_settings, (RUN_SETTINGS_IDX(inst_index) + 0))

STRUCT Polygon New_Polygon(fvec4 v1, fvec4 v2, fvec4 v3){
	STRUCT Polygon res;
	res.vertices[0] = v1;
	res.vertices[1] = v2;
	res.vertices[2] = v3;
	res.normal = VEC3_TO_4(TRIANGLE_NORMAL(v1, v2, v3));
	return res;
}

STRUCT Materials New_Materials(){
	STRUCT Materials mat;
	mat.material_types = IVEC4_ZERO;
	mat.material_ratios = FVEC4_ZERO;
	return mat;
}

STRUCT ISO_Material New_ISO_Material(){
	STRUCT ISO_Material mat;
	mat.final_iso = FVEC4_ZERO;
	mat.material_types = IVEC4_ZERO;
	mat.material_ratios = FVEC4_ZERO;
	return mat;
}

STRUCT GridPoint New_GridPoint(){
	STRUCT GridPoint res;
	res.iso = 0;
	res.localPosition = FVEC4_ZERO;
	res.globalPosition = FVEC4_ZERO;
	res.normal = FVEC4_ZERO;
	res.type = IVEC4_ZERO;
	res.type_ratio = FVEC4_ZERO;
	return res;
}

float Scale(float value, float oldMin, float oldMax, float newMin, float newMax)
{
    return newMin + (value - oldMin) * (newMax - newMin) / (oldMax - oldMin);
}

uint GetVoxelIndex(uint batch, uint g_index, uint size){
    return (g_index % size);
}

uint GetBatchIndex(uint batch, uint v_index, uint size){
    return (batch * size) + v_index;
}

ivec3 C_1D_to_3D(uint i, uint width, uint height) {
	int z = INT(i / (width * height));
	int y = INT((i % (width * height)) / width);
	int x = INT(i % width);

	return IVEC3(x, y, z);
}

uint C_3D_to_1D(int x, int y, int z, uint width, uint height) {
	return UINT(z * width * height + y * width + x);
}

uint C_2D_to_1D(int x, int y, uint width) {
    return UINT(y * width + x);
}

ivec2 C_1D_to_2D(uint i, uint width) {
    int y = INT(i / width);
    int x = INT(i % width);

    return IVEC2(x, y);
}

#if USL_OPENCL == 1

#define Print(str) \
printf(str);

#define Print_1(str, val) \
printf(str, val);

#define Print_2(str, val1, val2) \
printf(str, val1, val2);

#define Print_3(str, val1, val2, val3) \
printf(str, val1, val2, val3);

#define Print_4(str, val1, val2, val3, val4) \
printf(str, val1, val2, val3, val4);

#define Print_5(str, val1, val2, val3, val4, val5) \
printf(str, val1, val2, val3, val4, val5);

#define Print_Vec_1(str, val1) \
printf(str, val1.x, val1.y, val1.z, val1.w);

#define Print_Vec_2(str, val1, val2) \
printf(str, val1.x, val1.y, val1.z, val1.w, val2.x, val2.y, val2.z, val2.w);

#define Print_Vec_3(str, c, val1, val2, val3) \
printf(str, c, val1.x, val1.y, val1.z, val1.w, val2.x, val2.y, val2.z, val2.w, val3.x, val3.y, val3.z, val3.w);

/*#else

#define Print(str) 
#define Print_1(str, val)
#define Print_2(str, val, val)
#define Print_3(str, val, val, val)
#define Print_4(str, val, val, val, val)*/

#endif

#endif // _VOXEL_LIB_CORE_INC_