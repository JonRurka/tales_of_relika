#pragma once


#ifdef WIN32
#include <accctrl.h>
#include <aclapi.h>
#endif

#include <array>
#include <chrono>
#include <iostream>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

#include "nvgl/contextwindow_gl.hpp"
#include "nvpsystem.hpp"
#include "nvvk/commands_vk.hpp"
#include "nvvk/context_vk.hpp"


#include "nvvk/error_vk.hpp"
#include "nvvk/resourceallocator_vk.hpp"

#ifdef WIN32
#include <handleapi.h>
#else
#include <unistd.h>
#endif

namespace nvvk {

namespace test {

// #VKGL Extra for Interop
struct BufferVkGL
{
  nvvk::Buffer bufVk;  // The allocated buffer

#ifdef WIN32
  HANDLE handle = nullptr;  // The Win32 handle
#else
  int fd = -1;
#endif
  unsigned int memoryObject = 0;  // OpenGL memory object
  unsigned int oglId        = 0;  // OpenGL object ID

  void destroy(nvvk::ResourceAllocator& alloc);
};
}  // namespace test

void prepare();

void createBufferVK();

// Get the Vulkan buffer and create the OpenGL equivalent using the memory allocated in Vulkan
inline void doCreateBufferGL(nvvk::ResourceAllocator& alloc, nvvk::test::BufferVkGL& bufGl);

int run_test();

int run_test(vk::Instance instance, vk::Device device, vk::PhysicalDevice physicalDevice);

}


