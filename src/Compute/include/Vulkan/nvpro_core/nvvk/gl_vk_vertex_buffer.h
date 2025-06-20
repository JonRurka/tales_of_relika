#pragma once


#ifdef WIN32
#include <accctrl.h>
#include <aclapi.h>
#endif

#include <array>
#include <chrono>
#include <iostream>

//#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
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

namespace gl_vk_buffer {

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


void prepare();


BufferVkGL createBufferVK(int size);
BufferVkGL createBufferVK(int elements, int stride);

void destroyBufferVk(BufferVkGL& buffer);

VkDeviceMemory get_buffer_mem(nvvk::MemHandle handle);

// Get the Vulkan buffer and create the OpenGL equivalent using the memory allocated in Vulkan
inline void doCreateBufferGL(nvvk::ResourceAllocator& alloc, nvvk::gl_vk_buffer::BufferVkGL& bufGl);

void poll_events();

int run_test();

int run_test(GLFWwindow* window, vk::Instance instance, vk::Device device, vk::PhysicalDevice physicalDevice);

}  // namespace gl_vk_buffer
}


