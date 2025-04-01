#include "Mesh.h"

#include "opengl.h"
#include "dynamic_compute.h"

#include "Logger.h"

#ifdef WIN32
//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
#endif

#define RECENTER_THRESHOLD 0.1f

Mesh::Mesh()
{
	

	glGenBuffers(1, &VBO);
	glCheckError();

	glGenBuffers(1, &EBO);
	glCheckError();

	glGenVertexArrays(1, &VAO);
	glCheckError();

	glBindVertexArray(VAO);
	glCheckError();
	
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

void Mesh::Load(DynamicCompute::Compute::IComputeBuffer* buffer)
{
	
	size_t size = buffer->GetSize();
	//Logger::LogDebug(LOG_POS("Load"), "Load buffer size: %i", (int)size);

	GLuint glBuffer = buffer->External_Buffer();


	//glBindBuffer(GL_COPY_WRITE_BUFFER, VBO);
	//glBindBuffer(GL_COPY_READ_BUFFER, glBuffer);
	//glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
	//glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	//glBindBuffer(GL_COPY_READ_BUFFER, 0);

	glBindVertexArray(VAO);
	glCheckError();

	//glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glCheckError();

	//glBufferStorageMemEXT(GL_ARRAY_BUFFER, size, buffer->External_Memory(), 0);
	//glCheckError();

	//glBindBuffer(GL_ARRAY_BUFFER, glBuffer);
	//glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER, glBuffer);

	
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
	glEnableVertexAttribArray(3);


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	/*glCreateMemoryObjectsEXT(1, &m_extMemoryObject);

	//GL_HANDLE_TYPE_OPAQUE_FD_EXT

#ifdef WIN32
	void* fd = buffer->External_Descriptor();
	glImportMemoryWin32HandleEXT(m_extMemoryObject, size, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, fd);
#else
	int fd = buffer->External_Descriptor();
	glImportMemoryFdEXT(m_extMemoryObject, size, GL_HANDLE_TYPE_OPAQUE_FD_EXT, fd);
#endif

	// You can now create an OpenGL buffer using this memory object
	//GLuint glBuffer = buffer->External_Buffer();
	glGenBuffers(1, &glBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, glBuffer);
	glBufferStorageMemEXT(GL_ARRAY_BUFFER, size, m_extMemoryObject, 0);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_READ);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Copy data from glBuffer to the new VBO
	//glBindBuffer(GL_ARRAY_BUFFER, glBuffer);   // Source buffer (from Vulkan import)

	//glMapNamedBuffer(glBuffer, GL_READ_ONLY);

	//GLuint tmp_buff;
	//glGenBuffers(1, &tmp_buff);
	//glNamedBufferStorage(tmp_buff, size, NULL, GL_MAP_READ_BIT);

	//glBindBuffer(GL_COPY_WRITE_BUFFER, tmp_buff);
	//glBindBuffer(GL_COPY_READ_BUFFER, glBuffer);
	//glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);

	//glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

	//void* data = glMapNamedBufferRange(glBuffer, 0, size, GL_MAP_READ_BIT);

	//float* data = (float*)malloc(size);
	//memset(data, 0, size);

	//glGetBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	//glGetNamedBufferSubData(glBuffer, 0, size, data);

	if (data) {
		float* floatData = data;
		int numFloats = size / sizeof(float);
		int num_elem = numFloats / 11;
		for (int i = 0; i < num_elem; i++)
		{
			int b = i * 11;
			Logger::LogDebug(LOG_POS("Load"), "\t Vert:(%f, %f, %f), norm:(%f, %f, %f), color:(%f, %f, %f), tex:(%f, %f)",
				floatData[b + 0], floatData[b + 1], floatData[b + 2],
				floatData[b + 3], floatData[b + 4], floatData[b + 5],
				floatData[b + 6], floatData[b + 7], floatData[b + 8],
				floatData[b + 9], floatData[b + 10]
			);
		}
		//glUnmapBuffer(GL_ARRAY_BUFFER);
		//glUnmapNamedBuffer(tmp_buff);
	}
	else {
		Logger::LogError(LOG_POS("Load"), "Failed to map buffer.");
	}

	return;


	glBindVertexArray(VAO);


	//glBindBuffer(GL_COPY_WRITE_BUFFER, VBO);      // Destination buffer (new VBO)
	//glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

	*/

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Mesh::Recenter()
{
	m_min = glm::vec3(1000000.0f, 1000000.0f, 1000000.0f);
	m_max = glm::vec3(-1000000.0f, -1000000.0f, -1000000.0f);
	glm::vec3 sum(0.0f, 0.0f, 0.0f);
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

	glm::vec3 m_center = sum / (float)m_vertices.size();

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

void Mesh::Draw()
{
	glBindVertexArray(VAO);

	//printf("Process mesh: %s, verts: %i, indices: %i\n", m_name.c_str(), (int)m_num_vertices, (int)m_num_indices);
	//printf("num indices: %i vs %i\n", (int)m_num_indices, (int)m_indices.size());
	//printf("num verts: %i vs %i\n", (int)m_num_vertices, (int)m_vertices.size());

	if (Has_Indices()) {
		
		//printf("Process mesh: %s, verts: %i, indices: %i\n", m_name.c_str(), (int)m_num_vertices, (int)m_num_indices);



		glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, 0);
	}
	else {
		//printf("Process mesh: %s, verts: %i, indices: %i\n", m_name.c_str(), (int)m_num_vertices, (int)m_num_indices);
		glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
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
				glm::vec3 default_normal = glm::normalize(m_vertices[i] - glm::vec3(0.0f));
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
	glEnableVertexAttribArray(3);
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
