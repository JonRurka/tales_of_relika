#include "Mesh.h"

#include "opengl.h"
#include "dynamic_compute.h"

#include "Logger.h"

#ifdef WIN32
//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
#endif

#define RECENTER_THRESHOLD 0.1f

#define TRIANGLE_NORMAL(v1, v2, v3) normalize(cross((v2 - v1), (v3 - v1)))

Mesh::Mesh() : Mesh(0)
{
	m_attrib_list = VertexAttributeList::Default();
}

Mesh::Mesh(size_t size)
{
	m_attrib_list = VertexAttributeList::Default();

	m_initial_size = size;

	glGenBuffers(1, &VBO);
	glCheckError();

	glGenBuffers(1, &EBO);
	glCheckError();

	glGenVertexArrays(1, &VAO);
	glCheckError();

	glBindVertexArray(VAO);
	glCheckError();

	if (m_initial_size > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, m_initial_size, NULL, GL_STATIC_COPY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	

	// position attribute
	/*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// color attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// texture coord attribute
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);*/


	/*
	// FOR CUBE
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);*/

	glBindVertexArray(0);
	

	m_num_vertices = 0;
	m_num_indices = 0;
}

void Mesh::Load(DynamicCompute::Compute::IComputeBuffer* buffer, int size)
{
	if (size < 0) {
		size = buffer->GetSize();
	}

	//Logger::LogDebug(LOG_POS("Load"), "Load elements: %i", size / (sizeof(float) * 4));

	//m_num_vertices = size / (sizeof(float) * 11);
	m_num_vertices = size / m_attrib_list.m_stride;
	m_virtual_mesh = true;
	//Logger::LogDebug(LOG_POS("Load"), "Loading %i Vertices.", (int)m_num_vertices);
	//Logger::LogDebug(LOG_POS("Load"), "Load buffer size: %i", (int)size);

	//buffer->FlushExternal();
	GLuint glBuffer = buffer->External_Buffer();

	glBindVertexArray(VAO);
	glCheckError();

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	if (m_initial_size <= 0) {
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_COPY);
	}

	glBindBuffer(GL_COPY_READ_BUFFER, glBuffer);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, size);
	glCheckError();
	glBindBuffer(GL_COPY_READ_BUFFER, 0);

	/*void* data = (float*)malloc(size);
	memset(data, 0, size);
	glGetNamedBufferSubData(VBO, 0, size, data);
	glCheckError();
	if (data) {
		glm::fvec4* vbo_data = (glm::fvec4*)data;
		for (int i = 0; i < m_num_vertices; i++) {
			glm::fvec4 vec1 = glm::fvec4(0);//out_vert[i];
			glm::fvec4 vec2 = vbo_data[i * 2];

			Logger::LogDebug(LOG_POS("Process"), "(%f, %f, %f, %f) == (%f, %f, %f, %f)",
				vec1.x, vec1.y, vec1.z, vec1.w, vec2.x, vec2.y, vec2.z, vec2.w);
		}

	}
	else {
		Logger::LogError(LOG_POS("Load"), "Failed to map buffer.");
	}*/

	/*
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// color attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// texture coord attribute
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);*/
	m_attrib_list.process();

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, m_attrib_list.m_stride);
	glCheckError();

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Mesh::Set_Raw_Vertex_Data(float* data, size_t size)
{
	delete[] raw_vert_data;
	raw_vert_data = data;

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size, raw_vert_data, GL_STATIC_DRAW);

	/*
	// FOR CUBE
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);*/

	// position attribute
	/*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);*/

	m_attrib_list.process();

	glBindVertexArray(0);
}

void Mesh::Recenter()
{
	m_min = glm::vec3(1000000.0f, 1000000.0f, 1000000.0f);
	m_max = glm::vec3(-1000000.0f, -1000000.0f, -1000000.0f);
	glm::vec4 sum(0.0f, 0.0f, 0.0f, 0.0f);
	//printf("orig sum: (%f, %f, %f)\n", sum.x, sum.y, sum.z);
	for (const auto& v : m_vertices)
	{
		sum += v;

		m_min.x = std::min(m_min.x, v.x);
		m_min.y = std::min(m_min.y, v.y);
		m_min.z = std::min(m_min.z, v.z);

		m_max.x = std::max(m_max.x, v.x);
		m_max.y = std::max(m_max.y, v.y);
		m_max.z = std::max(m_max.z, v.z);
	}

	glm::vec4 m_center = sum / (float)m_vertices.size();

	//glm::vec3 size = m_max - m_min;

	//float biggest_size = std::max(size.x, std::max(size.y, size.z));
	//float scale = 1.0f / biggest_size;

	//printf("Mesh min: (%f, %f, %f)\n", m_min.x, m_min.y, m_min.z);
	//printf("Mesh max: (%f, %f, %f)\n", m_max.x, m_max.y, m_max.z);
	//printf("Mesh size: (%f, %f, %f), biggest_size: %f\n", size.x, size.y, size.z, biggest_size);
	//printf("Mesh center: (%f, %f, %f), %f\n", center.x, center.y, center.z, (float)m_vertices.size());



	if (glm::length(m_center) < RECENTER_THRESHOLD) {
		return;
	}

	for (int i = 0; i < m_vertices.size(); ++i) 
	{
		m_vertices[i] -= m_center;
		//m_vertices[i] *= scale;
	}
}

void Mesh::Generate_Normals()
{
	if (!m_active) {
		Logger::LogWarning(LOG_POS("Generate_Normals"), "Cannot generate normals for inactive mesh.");
		return;
	}

	if (m_indices.size() <= 0)
	{
		Logger::LogDebug(LOG_POS("Generate_Normals"), "Generate normals without indices.");
		m_normals.clear();
		m_normals.reserve(m_num_vertices);
		int num_poly = m_num_vertices / 3;
		for (int i = 0; i < num_poly; i++) {
			glm::vec3 v1 = m_vertices[(i * 3) + 0];
			glm::vec3 v2 = m_vertices[(i * 3) + 1];
			glm::vec3 v3 = m_vertices[(i * 3) + 2];
			glm::vec4 norm = glm::vec4(TRIANGLE_NORMAL(v1, v2, v3), 0);
			m_normals.push_back(norm);
			m_normals.push_back(norm);
			m_normals.push_back(norm);
		}
	}
	else {
		Logger::LogDebug(LOG_POS("Generate_Normals"), "Generate normals with indices (%i).", (int)m_indices.size());
		m_normals.clear();
		m_normals.reserve(m_num_vertices);
		for (int i = 0; i < m_indices.size(); i += 3) {
			glm::vec3 vert1 = m_vertices[m_indices[i + 0]];
			glm::vec3 vert2 = m_vertices[m_indices[i + 1]];
			glm::vec3 vert3 = m_vertices[m_indices[i + 2]];
			glm::vec4 norm = glm::vec4(TRIANGLE_NORMAL(vert1, vert2, vert3), 0);
			m_normals[m_indices[i + 0]] = norm;
			m_normals[m_indices[i + 1]] = norm;
			m_normals[m_indices[i + 2]] = norm;
		}
	}
	sync_vertices(Mesh::Vert_Update_Mode::NORMALS);
}

void Mesh::Draw(GLenum mode)
{
	glBindVertexArray(VAO);

	//printf("Process mesh: %s, verts: %i, indices: %i\n", m_name.c_str(), (int)m_num_vertices, (int)m_num_indices);
	//printf("num indices: %i vs %i\n", (int)m_num_indices, (int)m_indices.size());
	//printf("num verts: %i vs %i\n", (int)m_num_vertices, (int)m_vertices.size());

	if (Has_Indices()) {
		
		//printf("Process mesh: %s, verts: %i, indices: %i\n", m_name.c_str(), (int)m_num_vertices, (int)m_num_indices);

		glDrawElements(mode, m_num_indices, GL_UNSIGNED_INT, 0);
	}
	else {
		//printf("Process mesh: %s, verts: %i, indices: %i\n", m_name.c_str(), (int)m_num_vertices, (int)m_num_indices);
		
		
		if (mode == GL_LINES) {
			glDrawArrays(mode, 0, m_num_vertices / 2);
		}
		else {
			glDrawArrays(mode, 0, m_num_vertices);
		}
	}

	glBindVertexArray(0);
}

void Mesh::Dispose()
{
	delete[] raw_vert_data;
}

void Mesh::sync_vertices(Vert_Update_Mode mode)
{
	//printf("sync_vertices: %i\n", (int)mode);

	if (m_virtual_mesh) {
		return;
	}

	unsigned int num_elements = m_vertices.size();

	if (m_num_vertices != num_elements) {
		delete[] raw_vert_data;
		raw_vert_data = new float[num_elements * stride];
		mode = Vert_Update_Mode::ALL;
		m_has_verts = true;
		//printf("Refresh mesh buffer \n");
	}

	m_num_vertices = num_elements;

	bool colors_empty = false;
	bool normals_empty = false;
	bool texcoord_empty = false;

	if (mode == Vert_Update_Mode::VERTICES || 
		mode == Vert_Update_Mode::ALL) {
		//Recenter();
	}

	if (mode == Vert_Update_Mode::COLORS ||
		mode == Vert_Update_Mode::ALL) {
		if (m_colors.size() == 0) {
			colors_empty = true;
			//printf("Set empty color.\n");
		}
		else if (m_colors.size() < num_elements) {
			Logger::LogError(LOG_POS("sync_vertices"), "colors size must match vertices.");
			return;
		}
	}

	if (mode == Vert_Update_Mode::NORMALS ||
		mode == Vert_Update_Mode::ALL) {
		if (m_normals.size() == 0) {
			normals_empty = true;
			//printf("Set empty color.\n");
		}
		else if (m_normals.size() < num_elements) {
			Logger::LogError(LOG_POS("sync_vertices"), "normals size must match vertices.");
			return;
		}
	}

	if (mode == Vert_Update_Mode::TEXCORDS ||
		mode == Vert_Update_Mode::ALL) {
		if (m_texcoords.size() == 0) {
			texcoord_empty = true;
			//printf("Set empty tex coord.\n");
		}
		else if (m_texcoords.size() < num_elements) {
			Logger::LogError(LOG_POS("sync_vertices"), "text coords size must match vertices.");
			return;
		}
	}

	for (int i = 0; i < num_elements; i++) {
		float* vert_ptr = raw_vert_data + ((stride * i) + 0);
		float* norm_ptr = raw_vert_data + ((stride * i) + 3);
		float* color_ptr = raw_vert_data + ((stride * i) + 6);
		float* tex_ptr = raw_vert_data + ((stride * i) + 9);

		if (mode == Vert_Update_Mode::VERTICES ||
			mode == Vert_Update_Mode::ALL)
		{
			vert_ptr[0] = m_vertices[i].x;
			vert_ptr[1] = m_vertices[i].y;
			vert_ptr[2] = m_vertices[i].z;
		}

		if (mode == Vert_Update_Mode::NORMALS ||
			mode == Vert_Update_Mode::ALL)
		{
			if (!normals_empty) {
				norm_ptr[0] = m_normals[i].x;
				norm_ptr[1] = m_normals[i].y;
				norm_ptr[2] = m_normals[i].z;
			}
			else {
				glm::vec3 n_vert = m_vertices[i];
				glm::vec3 default_normal = glm::normalize(n_vert - glm::vec3(0.0f));
				norm_ptr[0] = default_normal.x;
				norm_ptr[1] = default_normal.y;
				norm_ptr[2] = default_normal.z;
			}
		}

		if (mode == Vert_Update_Mode::COLORS ||
			mode == Vert_Update_Mode::ALL)
		{
			if (!colors_empty) {
				color_ptr[0] = m_colors[i].x;
				color_ptr[1] = m_colors[i].y;
				color_ptr[2] = m_colors[i].z;
			}
			else {
				color_ptr[0] = 1;
				color_ptr[1] = 1;
				color_ptr[2] = 1;
			}
		}

		if (mode == Vert_Update_Mode::TEXCORDS ||
			mode == Vert_Update_Mode::ALL) {
			if (!texcoord_empty) {
				tex_ptr[0] = m_texcoords[i].x;
				tex_ptr[1] = m_texcoords[i].y;
			}
			else {
				tex_ptr[0] = 0;
				tex_ptr[1] = 0;
			}
		}
	}

	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, num_elements * sizeof(float) * stride, raw_vert_data, GL_STATIC_DRAW);
	/*
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// color attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// texture coord attribute
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);*/
	m_attrib_list.process();

	glBindVertexArray(0);
}

void Mesh::sync_indices()
{
	m_num_indices = m_indices.size();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_num_indices * sizeof(int), m_indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void Mesh::VertexAttributeList::process()
{
	int attrib_ptr = 0;
	for (const auto& attrib : m_attributes)
	{
		glVertexAttribPointer(attrib_ptr, attrib.x, GL_FLOAT, GL_FALSE, m_stride, (void*)attrib.y);
		glEnableVertexAttribArray(attrib_ptr);
		attrib_ptr++;
	}
}
