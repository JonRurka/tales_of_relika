# 0 "5-smoothrender_mark.cl"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "5-smoothrender_mark.cl"



# 1 "../libUSL/USL.inc" 1



# 1 "../libUSL/Types.inc" 1
# 5 "../libUSL/USL.inc" 2
# 1 "../libUSL/Buffers.inc" 1
# 6 "../libUSL/USL.inc" 2
# 1 "../libUSL/compat.inc" 1
# 16 "../libUSL/compat.inc"
float sqrMagnitude(float3 val) {

 return dot(val, val);
}
# 33 "../libUSL/compat.inc"
float vector_angle(float3 from, float3 to){
    float num = sqrt(dot(from, from) * dot(to, to));
    if (num < 0.0000000000000001)
    {
        return 0;
    }
 float dot_val2 = dot(from, to);
    float num2 = clamp(dot_val2 / num, -1.0f, 1.0f);
    return acos(num2) * 57.29578f;
}
# 7 "../libUSL/USL.inc" 2
# 5 "5-smoothrender_mark.cl" 2
# 1 "Voxel_Lib/smoothrender_mark_lib.inc" 1



# 1 "Voxel_Lib/voxel_lib_core.inc" 1



# 1 "Voxel_Lib/../../libUSL/USL.inc" 1
# 5 "Voxel_Lib/voxel_lib_core.inc" 2





struct Static_Settings{
    float4 VoxelsPerMeter;
    uint4 ChunkMeterSize;
    uint4 ChunkSize;
    uint4 FullChunkSize;
    float4 skipDist;
    float4 half_;
    float4 SideLength;
    uint4 batches;
};

struct Run_Settings{
    int4 Location;
};

struct Materials{
    int4 material_types;
    float4 material_ratios;
};

struct ISO_Material{
    float4 final_iso;
    int4 material_types;
    float4 material_ratios;
};

struct GridPoint{
 float iso;
    float4 localPosition;
 float4 globalPosition;
 int3 voxelIndex;
    float4 normal;
    int4 type;
    float4 type_ratio;
};
# 134 "Voxel_Lib/voxel_lib_core.inc"
struct Polygon{
 float4 vertices[3];
 float4 normal;
};







struct Polygon New_Polygon(float4 v1, float4 v2, float4 v3){
 struct Polygon res;
 res.vertices[0] = v1;
 res.vertices[1] = v2;
 res.vertices[2] = v3;
 res.normal = ((float4)(normalize(cross((v2 - v1).xyz, (v3 - v1).xyz)).x, normalize(cross((v2 - v1).xyz, (v3 - v1).xyz)).y, normalize(cross((v2 - v1).xyz, (v3 - v1).xyz)).z, 0));
 return res;
}

struct Materials New_Materials(){
 struct Materials mat;
 mat.material_types = (int4)(0, 0, 0, 0);
 mat.material_ratios = (float4)(0, 0, 0, 0);
 return mat;
}

struct ISO_Material New_ISO_Material(){
 struct ISO_Material mat;
 mat.final_iso = (float4)(0, 0, 0, 0);
 mat.material_types = (int4)(0, 0, 0, 0);
 mat.material_ratios = (float4)(0, 0, 0, 0);
 return mat;
}

struct GridPoint New_GridPoint(){
 struct GridPoint res;
 res.iso = 0;
 res.localPosition = (float4)(0, 0, 0, 0);
 res.globalPosition = (float4)(0, 0, 0, 0);
 res.normal = (float4)(0, 0, 0, 0);
 res.type = (int4)(0, 0, 0, 0);
 res.type_ratio = (float4)(0, 0, 0, 0);
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

int3 C_1D_to_3D(uint i, uint width, uint height) {
 int z = (int)(i / (width * height));
 int y = (int)((i % (width * height)) / width);
 int x = (int)(i % width);

 return (int3)(x, y, z);
}

uint C_3D_to_1D(int x, int y, int z, uint width, uint height) {
 return (uint)(z * width * height + y * width + x);
}

uint C_2D_to_1D(int x, int y, uint width) {
    return (uint)(y * width + x);
}

int2 C_1D_to_2D(uint i, uint width) {
    int y = (int)(i / width);
    int x = (int)(i % width);

    return (int2)(x, y);
}
# 5 "Voxel_Lib/smoothrender_mark_lib.inc" 2

__global struct Static_Settings* in_static_settings;


__constant int* in_counts_data;





__global int* out_stitch_map_data;
__global int4* out_counts_data;

__global float4* out_debug_data;


void SetStitchMap(uint voxel_index, int poly_buffer_index){
 out_stitch_map_data[voxel_index] = poly_buffer_index;
}

int GetCount(uint index){
 return in_counts_data[index];
}

void Set_Debug_Data(uint index, float4 data){
 out_debug_data[index] = data;
}

void CreateStitchMap(uint batch_index, uint batchesPerGroup, uint fullChunkSize){

    uint batch_offset = (batch_index) * fullChunkSize;

    int buffer_index = 0;
 uint v_index = 0;
    for (v_index = 0; v_index < fullChunkSize; ++v_index){

        int poly_count = GetCount(batch_offset + v_index);

        if (poly_count == 0){
            continue;
        }

        SetStitchMap(batch_offset + v_index, buffer_index);

        buffer_index += poly_count;
    }

 out_counts_data[batch_index] = (int4)(buffer_index, 0, batch_index, 0);

}

void smoothrender_mark_main(uint batch_index){

    struct Static_Settings static_settings = in_static_settings[0];



    CreateStitchMap(batch_index, static_settings.batches[0], (uint)(static_settings.FullChunkSize[0]));



}
# 6 "5-smoothrender_mark.cl" 2

void kernel main_cl(
 global struct Static_Settings* p_in_static_settings,

 global int* p_in_counts_data,

 global int* p_out_stitch_map_data,
 global int4* p_out_counts_data,

 global float4* p_out_debug_data
)
{
 in_static_settings = p_in_static_settings;
 in_counts_data = p_in_counts_data;
 out_stitch_map_data = p_out_stitch_map_data;
 out_counts_data = p_out_counts_data;
 out_debug_data = p_out_debug_data;

 smoothrender_mark_main(get_global_id(0));
}
