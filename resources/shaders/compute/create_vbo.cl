#define USL_DIRECTX_UNITY3D 0
#define USL_VULKAN 0
#define USL_OPENCL 1
#include "libUSL/USL.inc"

#define VBO_ELEMENTS 3
#define STRIDE (VBO_ELEMENTS * 4)

readwrite_buffer_float(0, out_vbo)

readwrite_buffer(fvec4, 1, in_vertices)
readwrite_buffer(fvec4, 2, in_normals)
readwrite_buffer(fvec4, 3, in_mat)
//read_buffer(fvec4, 0, in_colors)
//read_buffer(fvec4, 0, in_texcoords)

void insert(uvec3 id)
{
	uint i = id.x;
	
	uint vert_ptr =  ((STRIDE * i) + 0);
	//uint norm_ptr =  ((STRIDE * i) + 4);
	//uint color_ptr = ((STRIDE * i) + 6);
	//uint tex_ptr =  ((STRIDE * i) + 9);
	
	fvec4 vertex = get_buffer_vector(in_vertices, i);
	fvec4 normal = get_buffer_vector(in_normals, i);
	fvec4 mat = get_buffer_vector(in_mat, i);
	//fvec4 color = get_buffer_vector(in_colors, i);
	
	

	set_buffer_scalar(out_vbo, vert_ptr + 0,  vertex.x);
	set_buffer_scalar(out_vbo, vert_ptr + 1,  vertex.y);
	set_buffer_scalar(out_vbo, vert_ptr + 2 , vertex.z);
	set_buffer_scalar(out_vbo, vert_ptr + 3 , vertex.w);
	
	set_buffer_scalar(out_vbo, vert_ptr + 4 , normal.x);
	set_buffer_scalar(out_vbo, vert_ptr + 5 , normal.y);
	set_buffer_scalar(out_vbo, vert_ptr + 6 , normal.z);
	set_buffer_scalar(out_vbo, vert_ptr + 7 , normal.w);
	
	set_buffer_scalar(out_vbo, vert_ptr + 8 , mat.x);  
	set_buffer_scalar(out_vbo, vert_ptr + 9,  mat.y);
	set_buffer_scalar(out_vbo, vert_ptr + 10, mat.z);
	set_buffer_scalar(out_vbo, vert_ptr + 11, mat.w);
	
	//set_buffer_scalar(out_vbo, vert_ptr + 6 , color.x);     
	//set_buffer_scalar(out_vbo, vert_ptr + 7 , color.y);     
	//set_buffer_scalar(out_vbo, vert_ptr + 8 , color.z);
	
	//set_buffer_scalar(out_vbo, vert_ptr + 9 , texcoord.x);  
	//set_buffer_scalar(out_vbo, vert_ptr + 10, texcoord.y);
	
}


//layout (local_size_x = 16, local_size_y = 1, local_size_z = 1) in;
//void main()
//{
	//insert(gl_GlobalInvocationID);
//}

void kernel main_cl(
	global float* p_out_vbo, 
	global fvec4* p_in_vertices,
	global fvec4* p_in_normals,
	global fvec4* p_in_mat
)
{
	out_vbo = p_out_vbo;
	in_vertices = p_in_vertices;
	in_normals = p_in_normals;
	in_mat = p_in_mat;
	
	insert(get_global_id(0));
}
