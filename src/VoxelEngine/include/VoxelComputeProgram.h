#pragma once

#include "stdafx.h"
#include "ForwardDeclarations.h"
#include "dynamic_compute.h"

using namespace DynamicCompute::Compute;

namespace VoxelEngine {
	
	class VoxelComputeProgram {
	public:

		VoxelComputeProgram() {};
		VoxelComputeProgram(IComputeController* controller, std::string name, IComputeProgram::FileType type);
		VoxelComputeProgram(IComputeController* controller, std::string name, int workGroupSize, IComputeProgram::FileType type);

		void AddBuffer(int bind, IComputeBuffer* buffer);

		void Finalize();

		void Execute(int size_x, int size_y, int size_z);

	private:

		void Initialize(IComputeController* controller, std::string name, int workGroupSize, IComputeProgram::FileType type);

		bool finalized = false;
		bool initialized = false;

		std::string kernel_name;

		IComputeController* m_controller = nullptr;

		std::string m_name = "";
		IComputeProgram* m_program = nullptr;
	};


}
