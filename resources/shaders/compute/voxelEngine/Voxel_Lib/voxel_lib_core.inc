#ifndef _VOXEL_LIB_CORE_INC_
#define _VOXEL_LIB_CORE_INC_

#include "../../libUSL/USL.inc"
#include "../../libnoise/libnoise.inc"

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
    ivec4 material_ratios;
};

struct ISO_Material{
    fvec4 final_iso;
    ivec4 material_types;
    fvec4 material_ratios;
};

struct GridPoint{
    fvec4 position;
    fvec4 normal;
    ivec4 type;
    fvec4 type_ratio;
};

struct Polygon{
	fvec4 vertices[3];
	fvec4 normal;
};

#define VEC3_TO_4(i) (FVEC4(i.x, i.y, i.z, 0))

#define GET_STATIC_SETTINGS() get_buffer_object(in_static_settings, 0)

#define GET_RUN_SETTINGS(inst_index) get_buffer_object(in_run_settings, inst_index)

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
	mat.material_ratios = IVEC4_ZERO;
	return mat;
}

STRUCT ISO_Material New_ISO_Material(){
	STRUCT ISO_Material mat;
	mat.final_iso = FVEC4_ZERO;
	mat.material_types = IVEC4_ZERO;
	mat.material_ratios = IVEC4_ZERO;
	return mat;
}

STRUCT GridPoint New_GridPoint(){
	STRUCT GridPoint res;
	res.position = FVEC4_ZERO;
	res.normal = FVEC4_ZERO;
	res.type = IVEC4_ZERO;
	res.type_ratio = IVEC4_ZERO;
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


#endif // _VOXEL_LIB_CORE_INC_