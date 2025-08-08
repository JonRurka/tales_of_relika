#pragma once

#include "opengl.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

class Model;

namespace DynamicCompute {
	namespace Compute {
		class IComputeBuffer;
	}
}

class Mesh
{
	friend class Model;
public:

	typedef std::shared_ptr<Mesh> Shared;
	typedef std::weak_ptr<Mesh> Weak;

	class VertexAttributeList {
		friend class Mesh;
	public:

		VertexAttributeList() : m_float_stride{ (size_t)11 }, m_byte_stride{ (size_t)(11 * sizeof(float))}
		{
		}

		VertexAttributeList(int stride) : m_float_stride{ (size_t)stride }, m_byte_stride{ (size_t)(stride * sizeof(float)) }
		{
		}

		void add_attribute(int size, int offset) {
			m_attributes.push_back(glm::ivec4(size, offset, 0, 0));
		}

		static VertexAttributeList Default() {
			VertexAttributeList res(11 * sizeof(float));
			res.add_attribute(3, 0);
			res.add_attribute(3, (3 * sizeof(float)));
			res.add_attribute(3, (6 * sizeof(float)));
			res.add_attribute(2, (9 * sizeof(float)));
			return res;
		}

		size_t Byte_Stride() { return m_byte_stride; }
		size_t Float_Stride() { return m_float_stride; }

	private:
		std::vector<glm::ivec4> m_attributes;
		size_t m_byte_stride{ 0 };
		size_t m_float_stride{ 0 };

		void process();
	};

	Mesh();
	Mesh(size_t size);
	~Mesh()
	{
		Dispose();
	}

	static std::shared_ptr<Mesh> Create();
	static std::shared_ptr<Mesh> Create(size_t size);

	void Set_Vertex_Attributes(VertexAttributeList list)
	{
		m_attrib_list = list;
		m_float_stride = m_attrib_list.m_float_stride;
		m_byte_stride = m_attrib_list.m_byte_stride;
	}

	void Name(std::string value) { m_name = value; }
	std::string Name() { return m_name; }

	void Load(DynamicCompute::Compute::IComputeBuffer* buffer, int size = -1);

	void Vertices(std::vector<glm::vec4> value, bool sync = true)
	{
		m_vertices = value;
		if (m_active && sync)
			sync_vertices(Vert_Update_Mode::VERTICES);
	}

	std::vector<glm::vec4>& Vertices() { return m_vertices; }

	void Normals(std::vector<glm::vec4> value, bool sync = true)
	{
		m_normals = value;
		if (m_active && sync)
			sync_vertices(Vert_Update_Mode::NORMALS);
	}

	std::vector<glm::vec4>& Normals() { return m_normals; }


	void Colors(std::vector<glm::vec4> value, bool sync = true)
	{
		m_colors = value;
		if (m_active && sync)
			sync_vertices(Vert_Update_Mode::COLORS);
	}

	std::vector<glm::vec4>& Colors() { return m_colors; }


	void TexCoords(std::vector<glm::vec2> val, bool sync = true)
	{
		m_texcoords = val;
		if (m_active && sync)
			sync_vertices(Vert_Update_Mode::TEXCORDS);
	}

	std::vector<glm::vec2>& TexCoords() { return m_texcoords; }


	void Indices(std::vector<unsigned int> val)
	{
		m_indices = val;
		sync_indices();
	}

	bool Has_Indices() {
		return m_num_indices > 0;
	}

	glm::vec3 Center() { return m_center; }

	std::vector<unsigned int>& Indices() { return m_indices; }

	GLuint Get_VAO() { return VAO; }

	void Set_Raw_Vertex_Data(std::vector<float> data, bool delete_old = true);

	std::vector<float> Get_Raw_Vertex_Data() { return raw_vert_data; }

	void Recenter();

	void Activate(bool force_flush = false) {
		if (!m_active || force_flush)
			sync_vertices(Vert_Update_Mode::ALL);
		m_active = true;
	}

	void Generate_Normals();

	void Draw(GLenum mode = GL_TRIANGLES);

	void Dispose();

private:

	std::string m_name;

	VertexAttributeList m_attrib_list;

	std::vector<glm::vec4> m_vertices;
	std::vector<glm::vec4> m_normals;
	std::vector<glm::vec4> m_colors;
	std::vector<glm::vec2> m_texcoords;
	std::vector<unsigned int> m_indices;

	glm::vec3 m_center;

	glm::vec3 m_min;
	glm::vec3 m_max;

	// Number of floats, not bytes
	size_t m_float_stride {11};
	size_t m_byte_stride{ 11 * sizeof(float) };

	enum class Vert_Update_Mode {
		VERTICES,
		NORMALS,
		COLORS,
		TEXCORDS,
		ALL
	};

	void sync_vertices(Vert_Update_Mode mode);
	void sync_indices();
	
	GLuint VBO{ 0 }; // vertex buffer object
	GLuint EBO{ 0 }; // element buffer object
	GLuint VAO{ 0 }; // vertex array object

	GLuint m_extMemoryObject{ 0 };

	size_t m_num_vertices{ 0 };
	size_t m_num_indices{ 0 };
	bool m_active{ false };
	bool m_virtual_mesh{ false };
	size_t m_initial_size{ 0 };

	bool m_has_verts{ false };

	std::vector<float> raw_vert_data;

	inline static const std::string LOG_LOC{ "MESH" };
};

