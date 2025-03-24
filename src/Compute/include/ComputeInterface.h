#pragma once

#include "std_includes.h"
#include "PlatformStructures.h"

namespace DynamicCompute {
	namespace Compute {

		class ComputeInterface {
		public:

			enum Compute_SDK {
				OpenCL,
				VULKAN,
#ifdef WINDOWS_PLATFROM
				DIRECTX,
				CUDA
#endif
			};

			struct ControllerInfo {
			public:
				Platform platform;
				void* device;
				char program_dir[1000];
				int program_dir_size;

				void SetProgramDir(std::string dir) {
					program_dir_size = dir.size();
					memcpy(program_dir, dir.data(), program_dir_size);
					program_dir[program_dir_size] = '\0';
				}
			};

			static IComputeController* GetComputeController(Compute_SDK implementation, ControllerInfo info);

			static void DisposePlatform(Compute_SDK implementation);

			static std::vector<Platform> GetSupportedPlatforms_OpenCL();
			static std::vector<OpenCL_Device_Info> GetSupportedDevices_OpenCL(Platform pltfrm);

			static std::vector<Vulkan_Device_Info> GetSupportedDevices_Vulkan();

			static std::vector<DirectX_Device_Info> GetSupportedDevices_DirectX();

		private:

		};

	}
}