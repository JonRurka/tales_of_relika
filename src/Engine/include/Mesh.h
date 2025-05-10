#pragma once

#include "opengl.h"
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

	class VertexAttributeList {
		friend class Mesh;
	public:

		VertexAttributeList() : m_stride{ 11 }
		{}

		VertexAttributeList(int stride) : m_stride{stride}
		{}

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

	private:
		std::vector<glm::ivec4> m_attributes;
		int m_stride{0};

		void process();
	};

	Mesh();
	Mesh(size_t size);

	void Set_Vertex_Attributes(VertexAttributeList list) { m_attrib_list = list; }

	void Name(std::string value) { m_name = value; }
	std::string Name() { return m_name; }

	void Load(DynamicCompute::Compute::IComputeBuffer* buffer, int size = -1);

	void Vertices(std::vector<glm::vec4> value) 
	{ 
		m_vertices = value;
		if (m_active)
			sync_vertices(Vert_Update_Mode::VERTICES);
	}

	std::vector<glm::vec4>& Vertices() { return m_vertices; }

	void Normals(std::vector<glm::vec4> value)
	{
		m_normals = value;
		if (m_active)
			sync_vertices(Vert_Update_Mode::NORMALS);
	}

	std::vector<glm::vec4>& Normals() { return m_normals; }


	void Colors(std::vector<glm::vec4> value) 
	{ 
		m_colors = value;
		if (m_active)
			sync_vertices(Vert_Update_Mode::COLORS);
	}

	std::vector<glm::vec4>& Colors() { return m_colors; }


	void TexCoords(std::vector<glm::vec2> val) 
	{ 
		m_texcoords = val; 
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

	void Set_Raw_Vertex_Data(float* data, size_t size);

	float* Get_Raw_Vertex_Data() { return raw_vert_data; }

	void Recenter();

	void Activate() {
		if (!m_active)
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

	const size_t stride = 11;

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

	float* raw_vert_data { nullptr };

	inline static const std::string LOG_LOC{ "MESH" };
};

