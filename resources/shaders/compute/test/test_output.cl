# 0 "test_opencl_spirv.cl"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "test_opencl_spirv.cl"
# 1 "test_include.inc" 1
# 2 "test_opencl_spirv.cl" 2

__global float* out_data;
__global float* in_data;

void kernel main_cl(global const float* p_in_data, global float* p_out_data)
{
 in_data = p_in_data;
 out_data = p_out_data;

 out_data[get_global_id(0)] = in_data[get_global_id(0)] + 1;
}
