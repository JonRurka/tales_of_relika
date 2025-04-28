

__global float* out_data;
__global float* in_data;

void kernel main_cl(global const float* p_in_data, global float* p_out_data)
{
	in_data = p_in_data;
	out_data = p_out_data;
	
out_data[get_global_id(0)] = in_data[get_global_id(0)] + 1;
}