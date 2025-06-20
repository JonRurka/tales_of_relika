#pragma once

#include "stdafx.h"

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.hpp>

#ifdef WIN32
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif

#include "vk_mem_alloc.h"

#include "nvvk/context_vk.hpp"
#include "nvvk/resourceallocator_vk.hpp"

namespace DynamicCompute {
    namespace Compute {
        namespace VK {
            namespace Utilities {

                
                void Create_VMA_Allocator(VkInstance inst, VkPhysicalDevice physicalDevice, VkDevice device);
                void Create_NVVK_Allocator(VkPhysicalDevice physicalDevice, VkDevice device);

                VmaAllocator* Get_VMA_Allocator();
                nvvk::ExportResourceAllocatorDedicated* Get_NVVK_Allocator();

                // structures

                                // #VKGL Extra for Interop
                struct BufferVkGL
                {
                    nvvk::Buffer bufVk;  // The allocated buffer

#ifdef WIN32
                    void* handle = nullptr;  // The Win32 handle
#else
                    int fd = -1;
#endif
                    unsigned int memoryObject = 0;  // OpenGL memory object
                    unsigned int oglId = 0;  // OpenGL object ID

                    void destroy();
                };


                struct DescriptorBufferInfo {
                    VkBuffer buffer;
                    VkDeviceSize size;
                    int binding;
                };

                struct QueueFamilyIndices {
                    bool shouldIncludeGraphics = false;

                    std::optional<uint32_t> graphicsFamily;
                    std::optional<uint32_t> transferFamily;
                    std::optional<uint32_t> computeFamily;

                    bool isComplete() {
                        if (shouldIncludeGraphics) {
                            return
                                graphicsFamily.has_value() &&
                                computeFamily.has_value() &&
                                transferFamily.has_value();
                        }

                        return
                            computeFamily.has_value() &&
                            transferFamily.has_value();
                    }
                };



                // non-vulkan functions

                std::vector<char> readFile(const std::string& filename);



                // get structures

                VkApplicationInfo getApplicationInfo(const char* applicationName, uint32_t appVersion, const char* EngineName, uint32_t engineVersion, uint32_t apiVersion);

                VkInstanceCreateInfo getInstanceCreateInfo(VkApplicationInfo& appInfo, std::vector<const char*>& extensions);

                VkDeviceQueueCreateInfo getDeviceQueueCreateInfo(uint32_t queueFamilyIndex, uint32_t queueCount, float& queuePriority);

                VkPhysicalDeviceFeatures getPhysicalDeviceFeatures();

                VkDeviceCreateInfo getDeviceCreateInfo(
                    std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
                    VkPhysicalDeviceFeatures& deviceFeatures,
                    std::vector<const char*>& deviceExtensions);

                VkCommandPoolCreateInfo getCommandPoolCreateInfo(VkCommandPoolCreateFlags flag, uint32_t queueFamilyIndex);

                VkCommandBufferAllocateInfo getCommandBufferAllocateInfo(VkCommandPool commandPool, VkCommandBufferLevel level, uint32_t commandBufferCount);

                VkSemaphoreCreateInfo getSemaphoreCreateInfo();

                VkFenceCreateInfo getFenceCreateInfo(bool isSignaled);

                VkBufferCreateInfo getBufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode, VkBufferCreateFlags flags, std::vector<uint32_t>& queueFamilies);

                VkImageCreateInfo getImageCreateInfo(
                    VkImageType imageType,
                    VkImageUsageFlags usage,
                    VkSharingMode sharingMode,
                    std::vector<uint32_t>& queueFamilies,
                    uint32_t width,
                    uint32_t height,
                    uint32_t depth,
                    uint32_t mipLevels,
                    uint32_t arrayLayers,
                    VkFormat format,
                    VkImageTiling tiling,
                    bool preinitialized);

                VkPipelineShaderStageCreateInfo getPipelineShaderStageCreateInfo(VkShaderModule& smodule, const char* name);

                VkPipelineLayoutCreateInfo getPipelineLayoutCreateInfo(VkDescriptorSetLayout& descriptorSetLayout);

                VkComputePipelineCreateInfo getComputePipelineCreateInfo(VkPipelineLayout& pipelineLayout, VkPipelineShaderStageCreateInfo& shaderStageInfo);

                VkDescriptorSetLayoutBinding getDescriptorSetLayoutBinding_ComputeStorageBuffer(uint32_t binding);

                VkDescriptorSetLayoutCreateInfo getDescriptorSetLayoutCreateInfo(std::vector<VkDescriptorSetLayoutBinding>& layoutBindings);
                     
                VkDescriptorPoolSize getDescriptorPoolSize_Storagebuffer(uint32_t descriptorCount);

                VkDescriptorPoolCreateInfo getDescriptorPoolCreateInfo(std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets);

                VkDescriptorBufferInfo getDescriptorBufferInfo(VkBuffer& buffer, VkDeviceSize offset, VkDeviceSize range);

                VkWriteDescriptorSet getWriteDescriptorSet(VkDescriptorSet& descriptorSet, int binding, VkDescriptorType type, VkDescriptorBufferInfo& bufferInfo);


                // Initialization Functions

                bool checkValidationLayerSupport(std::vector<const char*> validationLayers);

                QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);


                // Load Enumeration functions

                std::vector<VkExtensionProperties> EnumerateInstanceExtensionProperties();

                std::vector<VkPhysicalDevice> EnumeratePhysicalDevices(VkInstance instance);

                std::vector<VkExtensionProperties> EnumerateDeviceExtensionProperties(VkPhysicalDevice device);

                std::vector<VkQueueFamilyProperties> GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device);
            

                // Device utilities

                bool checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> deviceExtensions);


                
                // resource creation abstractions

                VkResult CreateComputeDescriptorSetLayout(VkDevice& device, int numStorageBuffers, VkDescriptorSetLayout& descriptorSetLayout);

                VkResult createDescriptorPool(VkDevice device, int numStorageBuffers, int maxSets, VkDescriptorPool& descriptorPool);

                VkResult AllocateDescriptorSets(VkDevice device, std::vector<VkDescriptorSetLayout>& layouts, VkDescriptorPool& descriptorPool, int descriptorSetCount, VkDescriptorSet& descriptorSet);

                void UpdateDescriptorSets_StorageBuffers(VkDevice& device, VkDescriptorSet& descriptorSet, std::vector<DescriptorBufferInfo>& buffers);

                void CreateBuffer(
                    VkPhysicalDevice& physicalDevice,
                    VkDevice& device,
                    VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkSharingMode sharingMode,
                    bool external,
                    VkBufferCreateFlags flags,
                    VmaAllocationCreateFlags vma_flags,
                    VmaMemoryUsage vma_usage,
                    VkMemoryPropertyFlags properties,
                    std::vector<uint32_t>& queueFamilies,
                    VkBuffer& buffer,
                    VkDeviceMemory& bufferMemory);

                void CreateBuffer(
                    VkPhysicalDevice& physicalDevice,
                    VkDevice& device,
                    size_t stride,
                    size_t num_elements,
                    VkBufferUsageFlags usage,
                    VkSharingMode sharingMode,
                    bool external,
                    VkBufferCreateFlags flags,
                    VmaAllocationCreateFlags vma_flags,
                    VmaMemoryUsage vma_usage,
                    VkMemoryPropertyFlags properties,
                    std::vector<uint32_t>& queueFamilies,
                    VkBuffer& buffer,
                    VkDeviceMemory& bufferMemory);

                void CreateImage(
                    VkPhysicalDevice& physicalDevice,
                    VkDevice& device,
                    uint32_t width, 
                    uint32_t height,
                    VkBufferUsageFlags usage,
                    VkSharingMode sharingMode,
                    VkFormat format,
                    VkImageTiling tiling,
                    bool preinitialized,
                    std::vector<uint32_t>& queueFamilies,
                    VkImage& textureImage,
                    VkDeviceMemory& bufferMemory
                );

                uint32_t findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);


                // Buffer utilities

                void CopyBuffer(VkDevice& device, VkQueue& queue, VkCommandBuffer& commandBuffer, VkFence& wait_fence, VkBuffer& srcBuffer, VkBuffer& dstBuffer, int src_start, int dst_start, VkDeviceSize size);

                void FlushToBuffer(VkDevice& device, VkDeviceMemory& memory, VkDeviceSize size, void*& data, void* src, bool unmap);
            
                void CopyBufferToImage(VkDevice& device, VkQueue& queue, VkCommandPool& cmdPool, VkFence& wait_fence, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
                
                // Command utilities

                void CreateCommandBuffer(VkCommandPool& cmdPool, VkDevice device, VkCommandBuffer& commandBuffer);

                VkCommandBuffer CreateCommandBuffer(VkCommandPool& cmdPool, VkDevice device);

                void beginSingleTimeCommands(VkCommandBuffer& cmdBuffer);

                VkCommandBuffer beginSingleTimeCommands(VkCommandPool& cmdPool, VkDevice device);

                void endSingleTimeCommands(VkDevice& device, VkCommandBuffer& cmdBuffer, VkQueue& p_queue, VkFence& p_fence);
            

                // Shader utilities

                VkShaderModule createShaderModule(VkDevice& device, const std::vector<char>& code);

            }

            namespace Extensions {
                // Extension methods

                VkResult CreateDebugUtilsMessengerEXT(
                    VkInstance instance,
                    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                    const VkAllocationCallbacks* pAllocator,
                    VkDebugUtilsMessengerEXT* pDebugMessenger);

                void DestroyDebugUtilsMessengerEXT(
                    VkInstance instance,
                    VkDebugUtilsMessengerEXT debugMessenger,
                    const VkAllocationCallbacks* pAllocator);

            }
        }
    }
}