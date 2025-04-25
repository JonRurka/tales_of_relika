#pragma once

#include "game_engine.h"


namespace DynamicCompute {
	namespace Compute {
		class IComputeBuffer;
		class IComputeController;
		class IComputeProgram;
	}
}

using namespace DynamicCompute::Compute;

class Stitch_VBO {
public:

	void Init(IComputeController* controller, int elements);
	void Stitch(int elements);

	IComputeBuffer* Input_Vertex_Buffer() { return vertex_buffer; }
	IComputeBuffer* Input_Normal_Buffer() { return normal_buffer; }
	IComputeBuffer* Output_VBO_Buffer() { return normal_buffer; }

	static int Stride();

	static Mesh::VertexAttributeList Get_Vertex_Attributes();

private:
	std::string kernel_name = "main";
	int m_elements{ 0 };
	IComputeController* m_controller{nullptr};
	IComputeProgram* m_program{ nullptr };

	IComputeBuffer* vertex_buffer{ nullptr };
	IComputeBuffer* normal_buffer{ nullptr };
	IComputeBuffer* vbo_buffer{ nullptr };

	inline static const std::string LOG_LOC{ "STITCH_VBO" };
};