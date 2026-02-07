#include "Stitch_VBO.h"

#include "Resources.h"
#include "Game_Resources.h"
#include "dynamic_compute.h"
#include "Logger.h"

#include <chrono>

#include <glm/gtx/vector_angle.hpp>

#define VBO_ELEMENTS 3
#define FLOAT_STRIDE (VBO_ELEMENTS * 4)
#define BYTE_STRIDE (FLOAT_STRIDE * 4)
#define DEBUG_DRAW_NORMALS false



namespace {

	inline const glm::vec4 get_vert(float* vbo, int i) {
		int vert_i = ((FLOAT_STRIDE * i) + 0);
		return glm::vec4(
			vbo[vert_i + 0],
			vbo[vert_i + 1],
			vbo[vert_i + 2],
			vbo[vert_i + 3]
		);
	}
	inline const glm::vec4 get_norm(float* vbo, int i) {
		int norm_i = ((FLOAT_STRIDE * i) + 4);
		return glm::vec4(
			vbo[norm_i + 0],
			vbo[norm_i + 1],
			vbo[norm_i + 2], 
			vbo[norm_i + 3]
		);
	}
	inline void set_norm(float* vbo, int i, const glm::vec4& val) {
		int norm_i = ((FLOAT_STRIDE * i) + 4);
		vbo[norm_i + 0] = val.x;
		vbo[norm_i + 1] = val.y;
		vbo[norm_i + 2] = val.z;
		vbo[norm_i + 3] = val.w;
	}
	inline int hash_vert(const glm::vec4& v) {
		auto sv = glm::ivec3(
			std::round(v.x * 10000),
			std::round(v.x * 10000),
			std::round(v.x * 10000)
		);
		return Utilities::Hash_Chunk_Coord(sv);
	}


	void smooth_normals(float* vbo, int num) {
		const bool area_weighting = true;
		std::unordered_map<int, std::vector<glm::vec4>> vert_norms;
		for (int t = 0; t < num / 3; t++)
		{
			glm::vec4 n = get_norm(vbo, (t * 3) + 0);
			const glm::vec4 p1 = get_vert(vbo, (t * 3) + 0);
			const glm::vec4 p2 = get_vert(vbo, (t * 3) + 1);
			const glm::vec4 p3 = get_vert(vbo, (t * 3) + 2);


			//a1 = (p2 - p1).Angle(p3 - p1);    // p1 is the 'base' here
			//a2 = (p3 - p2).Angle(p1 - p2);    // p2 is the 'base' here
			//a3 = (p1 - p3).Angle(p2 - p3);    // p3 is the 'base' here

			float a1 = glm::angle((p2 - p1), (p3 - p1));
			float a2 = glm::angle((p3 - p2), (p1 - p2));
			float a3 = glm::angle((p1 - p3), (p2 - p3));

			if (!area_weighting) {
				n = glm::normalize(n);
			}

			int key_vec_1 = hash_vert(p1);
			int key_vec_2 = hash_vert(p1);
			int key_vec_3 = hash_vert(p1);

			if (!vert_norms.contains(key_vec_1)) {
				vert_norms[key_vec_1] = std::vector<glm::vec4>();
				vert_norms[key_vec_1].reserve(8);
			}
			if (!vert_norms.contains(key_vec_2)) {
				vert_norms[key_vec_2] = std::vector<glm::vec4>();
				vert_norms[key_vec_2].reserve(8);
			}
			if (!vert_norms.contains(key_vec_3)) {
				vert_norms[key_vec_3] = std::vector<glm::vec4>();
				vert_norms[key_vec_3].reserve(8);
			}
			vert_norms[key_vec_1].push_back(n);
			vert_norms[key_vec_2].push_back(n);
			vert_norms[key_vec_3].push_back(n);

		}
		for (int v_i = 0; v_i < num; v_i++) {
			const glm::vec4 v = get_vert(vbo, v_i);
			int key_vec = hash_vert(v);
			glm::vec4 N_sum;

			const auto& list = vert_norms[key_vec];
			for (const auto& n : list) {
				N_sum += n;
			}

			N_sum = glm::normalize(N_sum);
			set_norm(vbo, v_i, N_sum);
		}

	}
}

void Stitch_VBO::Init(IVoxelBuilder_private::Shared builder, int elements)
{
	v_builder = builder;
	m_controller = builder->Get_Compute_Controller();
	m_elements = elements;

	m_attribute_list = Get_Vertex_Attributes();

	m_vertices = new glm::vec4[Max_Verts];
	m_normals = new glm::vec4[Max_Verts];
	m_raw_vert_data = new float[Max_Verts * m_attribute_list.Float_Stride()];

	compute_triangles();

	std::string resource = Game_Resources::Shaders::Compute::CREATE_VBO;

	std::vector<char> shader_data_tmp = Resources::Get_Shader_bin(resource);
	std::vector<uint8_t> shader_data(shader_data_tmp.begin(), shader_data_tmp.end());

	//IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(m_name, IComputeProgram::FileType::Binary);
	IComputeProgram::ProgramInfo program_info = IComputeProgram::ProgramInfo(resource, IComputeProgram::FileType::Text_Data);
	program_info.SetData(shader_data);

	program_info.AddKernel(kernel_name);

	m_program = m_controller->AddProgram(program_info);
	m_program->Build();
	//Logger::LogDebug(LOG_POS("Init"), "Build program");


	vertex_buffer = m_controller->NewReadWriteBuffer(elements, sizeof(float) * 4);
	normal_buffer = m_controller->NewReadWriteBuffer(elements, sizeof(float) * 4);
	mat_buffer = m_controller->NewReadWriteBuffer(elements, sizeof(float) * 4);
	//Logger::LogDebug(LOG_POS("Init"), "Create Extern VBO");
	vbo_buffer = m_controller->NewReadWriteBuffer(elements, BYTE_STRIDE, true);
	//Logger::LogDebug(LOG_POS("Init"), "Max vert capacity: %i, Max VBO capacity: %i", elements, elements * STRIDE);

	//float* zero_data = new float[elements * STRIDE];

	IComputeProgram::BindIndex ind{};
	ind.GlobalIndex = 0;
	ind.ParameterIndex = 0;
	m_program->KernelSetBuffer(kernel_name, vbo_buffer, ind);

	ind = {};
	ind.GlobalIndex = 1;
	ind.ParameterIndex = 1;
	m_program->KernelSetBuffer(kernel_name, vertex_buffer, ind);

	ind = {};
	ind.GlobalIndex = 2;
	ind.ParameterIndex = 2;
	m_program->KernelSetBuffer(kernel_name, normal_buffer, ind);

	ind = {};
	ind.GlobalIndex = 3;
	ind.ParameterIndex = 3;
	m_program->KernelSetBuffer(kernel_name, mat_buffer, ind);

	m_program->FinishBuild();
	//Logger::LogDebug(LOG_POS("Init"), "Finish Build program");
}

void Stitch_VBO::Stitch(int elements)
{
	m_program->RunKernel(kernel_name, elements, 1, 1);
	//Logger::LogDebug(LOG_POS("Stitch"), "Executed Kernel");
}

void Stitch_VBO::Process(Mesh& mesh, glm::ivec4 count, bool gpu_copy, bool apply_mesh)
{
	if (count.x > (int)Utilities::Vertex_Limit_Mode::Chunk_Max) {
		Logger::LogError(LOG_POS("Process"), "Chunk size of %i is greate that chunk max of %i",
			count.x, (int)Utilities::Vertex_Limit_Mode::Chunk_Max);
		return;
	}

	double gframe_start = Utilities::Get_Time();


	auto start = std::chrono::high_resolution_clock::now();
	v_builder->Extract(
		Input_Vertex_Buffer(),
		Input_Normal_Buffer(),
		Input_Mat_Buffer(),
		nullptr,
		count
	);
	auto end = std::chrono::high_resolution_clock::now();
	auto builder_extract_duration = std::chrono::duration<double>(end - start).count() * 1000;

	//glm::vec4* data = new glm::vec4[count.x];
	//Input_Vertex_Buffer()->GetData(data, count.x * sizeof(float) * 4);

	//Logger::LogDebug(LOG_POS("Process"), "Count: %i", count.x);
	//for (int i = 0; i < count.x; i++) {
		//Logger::LogDebug(LOG_POS("Process"), "%i: (%f, %f, %f, %f)", 
		//	i, data[i].x, data[i].y, data[i].z, data[i].w);
	//}

	start = std::chrono::high_resolution_clock::now();
	Stitch(count.x);
	end = std::chrono::high_resolution_clock::now();
	auto stitch_duration = std::chrono::duration<double>(end - start).count() * 1000;

	times.x += builder_extract_duration;
	times.y += stitch_duration;

	if (gpu_copy) {

		

		//glm::fvec4* out_vert = new glm::fvec4[count.x];
		//glm::fvec4* out_vbo = new glm::fvec4[count.x * VBO_ELEMENTS];
		//Input_Vertex_Buffer()->GetData(out_vert, 0, count.x * sizeof(float) * 4);
		//Output_VBO_Buffer()->GetData(out_vbo, 0, count.x * STRIDE);


		/*for (int i = 0; i < count.x; i++) {
			glm::fvec4 vec1 = out_vert[i];
			glm::fvec4 vec2 = out_vbo[i * VBO_ELEMENTS];

			//Logger::LogDebug(LOG_POS("Process"), "(%f, %f, %f, %f) == (%f, %f, %f, %f)",
			//	vec1.x, vec1.y, vec1.z, vec1.w, vec2.x, vec2.y, vec2.z, vec2.w);
		}*/


		//Logger::LogDebug(LOG_POS("Process"), "Process Vert size: %i, Process VBO size: %i", count.x, count.x * STRIDE);
		
		int vbo_size = count.x * BYTE_STRIDE;

		start = std::chrono::high_resolution_clock::now();
		//Output_VBO_Buffer()->FlushExternal(vbo_size);
		end = std::chrono::high_resolution_clock::now();
		auto flush_duration = std::chrono::duration<double>(end - start).count() * 1000;

		start = std::chrono::high_resolution_clock::now();
		if (apply_mesh)
		{
			mesh.Set_Vertex_Attributes(m_attribute_list);
			mesh.Load(Output_VBO_Buffer(), vbo_size);
		}
		end = std::chrono::high_resolution_clock::now();
		auto load_mesh_duration = std::chrono::duration<double>(end - start).count() * 1000;
		
		times.z += flush_duration;
		times.w += load_mesh_duration;

		//Logger::LogDebug(LOG_POS("Process"), "Builder Extract: %f ms, Stitch: %f ms, Flush: %f ms, Load Mesh: %f ms\n",
		//	builder_extract_duration, stitch_duration, flush_duration, load_mesh_duration);
	}
	else {



		/*v_builder->Extract(
			m_vertices,
			m_normals,
			m_mats,
			nullptr,
			count
		);*/

		//Logger::LogDebug(LOG_POS("Process"), "Batch: %i", count.z);
		
		/*Logger::LogDebug(LOG_POS("Process"), "Count: %i", count.x);
		for (int i = 0; i < 6; i++) {
			Logger::LogDebug(LOG_POS("Process"), "%i: (%f, %f, %f, %f)",
				i, m_vertices[i].x, m_vertices[i].y, m_vertices[i].z, m_vertices[i].w);
		}*/
		

		/*
		std::vector<glm::vec4> verts(m_vertices, m_vertices + count.x);
		std::vector<unsigned int> tris(m_triangles, m_triangles + count.x);
		std::vector<glm::vec4> normals(m_normals, m_normals + count.x);

		if (DEBUG_DRAW_NORMALS) {
			for (int i = 0; i < count.x; i++) {
				Graphics::DrawDebugRay(m_vertices[i], m_normals[i], glm::vec3(0, 0, 1), 10000);
			}
		}*/


		//Logger::LogDebug(LOG_POS("Process"), "Applying %i verts out of max %i", count.x, (int)Utilities::Vertex_Limit_Mode::Chunk_Max);

		//mesh->Vertices(verts);
		//mesh->Indices(tris);
		//mesh->Normals(normals);

		int vbo_size = count.x * BYTE_STRIDE;

		start = std::chrono::high_resolution_clock::now();

		if (apply_mesh) {
			mesh.Set_Vertex_Attributes(m_attribute_list);
			Output_VBO_Buffer()->GetData(m_raw_vert_data, vbo_size);

			/*Logger::LogDebug(LOG_POS("Process"), "Sync %i elements", count.x);
			for (int i = 0; i < 6; i++) {
				int j = i * STRIDE;
				Logger::LogDebug(LOG_POS("Process"), "VBO %i: v:(%f, %f, %f, %f), n:(%f, %f, %f, %f), m:(%f, %f, %f, %f)",
					i, m_raw_vert_data[(j + 0)], m_raw_vert_data[j + 1], m_raw_vert_data[j + 2], m_raw_vert_data[j + 3],
					m_raw_vert_data[(j + 4)], m_raw_vert_data[j + 5], m_raw_vert_data[j + 6], m_raw_vert_data[j + 7],
					m_raw_vert_data[(j + 8)], m_raw_vert_data[j + 9], m_raw_vert_data[j + 10], m_raw_vert_data[j + 11]);
			}*/

			// https://stackoverflow.com/questions/45477806/general-method-for-calculating-smooth-vertex-normals-with-100-smoothness
			// TODO: smooth normals;
			smooth_normals(m_raw_vert_data, count.x);

			std::vector<float> raw_data(m_raw_vert_data, m_raw_vert_data + (count.x * FLOAT_STRIDE));
			mesh.Set_Raw_Vertex_Data(raw_data, false);
			mesh.Activate();

			//Logger::LogDebug(LOG_POS("Process"), "actually did render.");
		}

		end = std::chrono::high_resolution_clock::now();
		auto load_mesh_duration = std::chrono::duration<double>(end - start).count() * 1000;

		times.w += load_mesh_duration;
	}


	double gframe_end = Utilities::Get_Time();
	g_frame_time += (gframe_end = gframe_start) / 1000; // ms
}

void Stitch_VBO::Reset()
{
	//Logger::LogDebug(LOG_POS("Process"), "Builder Extract: %f ms, Stitch: %f ms, Flush: %f ms, Load Mesh: %f ms, Total: %f\n",
	//	times.x, times.y, times.z, times.w, (times.x + times.y + times.z + times.w));

	times = glm::dvec4(0.0);
}

std::vector<float> Stitch_VBO::Output_VBO_Vector(int count, bool do_smooth_normals)
{
	int vbo_size = count * BYTE_STRIDE;
	Output_VBO_Buffer()->GetData(m_raw_vert_data, vbo_size);
	if (do_smooth_normals) {
		smooth_normals(m_raw_vert_data, count);
	}
	return std::vector<float>(m_raw_vert_data, m_raw_vert_data + (count * FLOAT_STRIDE));
}

int Stitch_VBO::Byte_Stride()
{
	return BYTE_STRIDE;
}

int Stitch_VBO::Float_Stride()
{
	return FLOAT_STRIDE;
}

Mesh::VertexAttributeList Stitch_VBO::Get_Vertex_Attributes()
{
	Mesh::VertexAttributeList res{};
	res.add_attribute(4, 0);
	res.add_attribute(4, (4 * sizeof(float)));
	res.add_attribute(4, (8 * sizeof(float)));
	return res;
}

IComputeController* Stitch_VBO::create_controller()
{
	IComputeProgram::FileType type = IComputeProgram::FileType::Text_Data;

	m_device_cl = Utilities::Get_Recommended_Device();
	//Logger::LogDebug(LOG_POS("InitializeComputePrograms"), "Using OpenCL Compute Device: %s", m_device_cl.name);

	ComputeInterface::ControllerInfo controllerInfo{};
	controllerInfo.device = &m_device_cl;
	controllerInfo.platform = m_device_cl.platform;
	controllerInfo.SetProgramDir("");

	return ComputeInterface::GetComputeController(ComputeInterface::OpenCL, controllerInfo);
}

void Stitch_VBO::compute_triangles()
{
	m_triangles.clear();
	m_triangles.resize(Max_Verts);
	bool m_invert_tris = false;
	for (int i = 0; i < (Max_Verts / 3); i += 3) {
		unsigned int tris_start = i;

		if (m_invert_tris) {
			m_triangles[tris_start + 0] = tris_start + 0;
			m_triangles[tris_start + 1] = tris_start + 1;
			m_triangles[tris_start + 2] = tris_start + 2;
		}
		else
		{
			m_triangles[tris_start + 0] = tris_start + 2;
			m_triangles[tris_start + 1] = tris_start + 1;
			m_triangles[tris_start + 2] = tris_start + 0;
		}
	}
}
