#include "vulkan_utils.h"

#define WIN32_LEAN_AND_MEAN
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

using namespace DynamicCompute::Compute::VK;

void DynamicCompute::Compute::VK::Utilities::Create_VMA_Allocator(VkInstance inst, VkPhysicalDevice physicalDevice, VkDevice device)
{
    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_KHR_EXTERNAL_MEMORY_WIN32_BIT;
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    allocatorCreateInfo.physicalDevice = physicalDevice;
    allocatorCreateInfo.device = device;
    allocatorCreateInfo.instance = inst;
    allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

    VmaAllocator allocator;
    vmaCreateAllocator(&allocatorCreateInfo, &allocator);
}

std::vector<char> Utilities::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    //printf("VK Utilities::readFile: %s\n", filename.c_str());

    if (!file.is_open()) {
        printf("Failed to open file: %s\n", filename.c_str());
        throw std::runtime_error("Failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkApplicationInfo Utilities::getApplicationInfo(const char* applicationName, uint32_t appVersion, const char* EngineName, uint32_t engineVersion, uint32_t apiVersion)
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = applicationName;
    appInfo.applicationVersion = appVersion;
    appInfo.pEngineName = EngineName;
    appInfo.engineVersion = engineVersion;
    appInfo.apiVersion = apiVersion;
	return appInfo;
}

VkInstanceCreateInfo Utilities::getInstanceCreateInfo(VkApplicationInfo& appInfo, std::vector<const char*>& extensions)
{
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    return createInfo;
}

VkDeviceQueueCreateInfo Utilities::getDeviceQueueCreateInfo(uint32_t queueFamilyIndex, uint32_t queueCount, float& queuePriority)
{
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = queueCount;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    return queueCreateInfo;
}

VkPhysicalDeviceFeatures Utilities::getPhysicalDeviceFeatures()
{
    VkPhysicalDeviceFeatures deviceFeatures{};
    return deviceFeatures;
}

VkDeviceCreateInfo Utilities::getDeviceCreateInfo(
    std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
    VkPhysicalDeviceFeatures& deviceFeatures,
    std::vector<const char*>& deviceExtensions)
{
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    return createInfo;
}

VkCommandPoolCreateInfo Utilities::getCommandPoolCreateInfo(VkCommandPoolCreateFlags flag, uint32_t queueFamilyIndex)
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = flag;
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    return poolInfo;
}

VkCommandBufferAllocateInfo Utilities::getCommandBufferAllocateInfo(VkCommandPool commandPool, VkCommandBufferLevel level, uint32_t commandBufferCount)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = commandBufferCount;

    return allocInfo;
}

VkSemaphoreCreateInfo Utilities::getSemaphoreCreateInfo()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    return semaphoreInfo;
}

VkFenceCreateInfo Utilities::getFenceCreateInfo(bool isSignaled)
{
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (isSignaled)
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    return fenceInfo;
}

VkBufferCreateInfo Utilities::getBufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode, VkBufferCreateFlags flags, std::vector<uint32_t>& queueFamilies)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = sharingMode;
    bufferInfo.flags = flags;
    if (queueFamilies.size() > 0)
    {
        bufferInfo.queueFamilyIndexCount = queueFamilies.size();
        bufferInfo.pQueueFamilyIndices = queueFamilies.data();
    }
    return bufferInfo;
}

VkImageCreateInfo Utilities::getImageCreateInfo(
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
    bool preinitialized)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = imageType;
    imageInfo.usage = usage;
    imageInfo.sharingMode = sharingMode;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = depth;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = preinitialized ? VK_IMAGE_LAYOUT_PREINITIALIZED : VK_IMAGE_LAYOUT_UNDEFINED;
    
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;

    if (queueFamilies.size() > 0)
    {
        imageInfo.queueFamilyIndexCount = queueFamilies.size();
        imageInfo.pQueueFamilyIndices = queueFamilies.data();
    }
    
    return imageInfo;
}

VkPipelineShaderStageCreateInfo Utilities::getPipelineShaderStageCreateInfo(VkShaderModule& smodule, const char* name)
{
    VkPipelineShaderStageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    info.module = smodule;
    info.pName = name;
    return info;
}

VkPipelineLayoutCreateInfo Utilities::getPipelineLayoutCreateInfo(VkDescriptorSetLayout& descriptorSetLayout)
{
    VkPipelineLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.setLayoutCount = 1;
    info.pSetLayouts = &descriptorSetLayout;
    info.pushConstantRangeCount = 0;
    info.pPushConstantRanges = nullptr;
    return info;
}

VkComputePipelineCreateInfo Utilities::getComputePipelineCreateInfo(VkPipelineLayout& pipelineLayout, VkPipelineShaderStageCreateInfo& shaderStageInfo)
{
    VkComputePipelineCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    info.layout = pipelineLayout;
    info.stage = shaderStageInfo;
    return info;
}

VkDescriptorSetLayoutBinding Utilities::getDescriptorSetLayoutBinding_ComputeStorageBuffer(uint32_t binding)
{
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorCount = 1;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBinding.pImmutableSamplers = nullptr;
    layoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    return layoutBinding;
}

VkDescriptorSetLayoutCreateInfo Utilities::getDescriptorSetLayoutCreateInfo(std::vector<VkDescriptorSetLayoutBinding>& layoutBindings)
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
    layoutInfo.pBindings = layoutBindings.data();
    return layoutInfo;
}

VkDescriptorPoolSize Utilities::getDescriptorPoolSize_Storagebuffer(uint32_t descriptorCount)
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize.descriptorCount = descriptorCount;
    return poolSize;
}

VkDescriptorPoolCreateInfo Utilities::getDescriptorPoolCreateInfo(std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets)
{
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxSets;
    return poolInfo;
}

VkDescriptorBufferInfo Utilities::getDescriptorBufferInfo(VkBuffer& buffer, VkDeviceSize offset, VkDeviceSize range)
{
    VkDescriptorBufferInfo storagebufferInfo{};
    storagebufferInfo.buffer = buffer;
    storagebufferInfo.offset = offset;
    storagebufferInfo.range = range;
    return storagebufferInfo;
}

VkWriteDescriptorSet Utilities::getWriteDescriptorSet(VkDescriptorSet& descriptorSet, int binding, VkDescriptorType type, VkDescriptorBufferInfo& bufferInfo)
{
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = type;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    return descriptorWrite;
}

bool Utilities::checkValidationLayerSupport(std::vector<const char*> validationLayers)
{
    uint32_t layerCount;

    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);

    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());


    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            std::cout << "Could not find validation layer: " << layerName << "\n";
            return false;
        }

    }

    return true;
}

Utilities::QueueFamilyIndices Utilities::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;
    indices.shouldIncludeGraphics = false;

    std::vector<VkQueueFamilyProperties> queueFamilies = Utilities::GetPhysicalDeviceQueueFamilyProperties(device);
    //printf("Searching queue families: %i\n", (int)queueFamilies.size());


    // TODO: Better logic to determine different transfer family in
    // the case there is only a dedicated transfer family, and not a
    // dedicated compute family. As a compute family always has a 
    // is also always combined with a transfer family, it is likely
    // that a combination family will be found with a dedicated 
    // transfer family (like in my hardware).

    int i = 0;
    for (const auto& queueFam : queueFamilies) {
        if ((queueFam.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
            !(queueFam.queueFlags & VK_QUEUE_TRANSFER_BIT)) {
            indices.computeFamily = i;
            //printf("Found Seperate compute queue Family: %i\n", i);
        }

        if (!(queueFam.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
            (queueFam.queueFlags & VK_QUEUE_TRANSFER_BIT)) {
            indices.transferFamily = i;
            //printf("Found Seperate transfer queue Family: %i\n", i);
        }
    }

    i = 0;
    if (!indices.computeFamily.has_value() || !indices.transferFamily.has_value()) {
        for (const auto& queueFam : queueFamilies) {
            if ((queueFam.queueFlags & VK_QUEUE_TRANSFER_BIT) &&
                (queueFam.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
                indices.transferFamily = i;
                indices.computeFamily = i;
                //printf("Found combined queue Family: %i\n", i);
                break;
            }
        }
    }

    return indices;
}


// Load functions

std::vector<VkExtensionProperties> Utilities::EnumerateInstanceExtensionProperties()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);

    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    return extensions;
}

std::vector<VkPhysicalDevice> Utilities::EnumeratePhysicalDevices(VkInstance instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    return devices;
}

std::vector<VkExtensionProperties> Utilities::EnumerateDeviceExtensionProperties(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    return availableExtensions;
}

std::vector<VkQueueFamilyProperties> Utilities::GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    return queueFamilies;
}


// Device utilities

bool Utilities::checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> deviceExtensions)
{
    std::vector<VkExtensionProperties> availableExtensions = Utilities::EnumerateDeviceExtensionProperties(device);

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& ext : availableExtensions) {
        //printf("Available: %s\n", ext.extensionName);
        requiredExtensions.erase(ext.extensionName);
    }

    for (const auto& ext : requiredExtensions) {
        std::cout << "Missing Required Device Extension: " << ext.c_str() << std::endl;
    }

    return requiredExtensions.empty();
}

VkResult Utilities::CreateComputeDescriptorSetLayout(VkDevice& device, int numStorageBuffers, VkDescriptorSetLayout& descriptorSetLayout)
{
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings(numStorageBuffers);

    for (int i = 0; i < numStorageBuffers; i++)
    {
        layoutBindings[i] = Utilities::getDescriptorSetLayoutBinding_ComputeStorageBuffer(i);
    }

    // TODO: Possibly other descriptor types

    VkDescriptorSetLayoutCreateInfo layoutInfo = Utilities::getDescriptorSetLayoutCreateInfo(layoutBindings);

    return vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);
}

VkResult Utilities::createDescriptorPool(VkDevice device, int numStorageBuffers, int maxSets, VkDescriptorPool& descriptorPool)
{
    std::vector<VkDescriptorPoolSize> poolSizes; 

    poolSizes.push_back(Utilities::getDescriptorPoolSize_Storagebuffer(numStorageBuffers));
    // TODO: Possibly other descriptor types

    VkDescriptorPoolCreateInfo poolInfo = Utilities::getDescriptorPoolCreateInfo(poolSizes, maxSets);
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    return vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
}

VkResult Utilities::AllocateDescriptorSets(VkDevice device, std::vector<VkDescriptorSetLayout>& layouts, VkDescriptorPool& descriptorPool, int descriptorSetCount, VkDescriptorSet& descriptorSet)
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetCount);
    allocInfo.pSetLayouts = layouts.data();

    return vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
}

void Utilities::UpdateDescriptorSets_StorageBuffers(VkDevice& device, VkDescriptorSet& descriptorSet, std::vector<DescriptorBufferInfo>& buffers)
{
    std::vector<VkDescriptorBufferInfo> storageBufferInfos(buffers.size());
    std::vector<VkWriteDescriptorSet> descriptorWrites(buffers.size());

    int i = 0;
    for (DescriptorBufferInfo bindInfo : buffers) {
        storageBufferInfos[i] = Utilities::getDescriptorBufferInfo(bindInfo.buffer, 0, bindInfo.size);
        descriptorWrites[i] = Utilities::getWriteDescriptorSet(descriptorSet, bindInfo.binding, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, storageBufferInfos[i]);
        i++;
    }

    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(descriptorWrites.size()),
        descriptorWrites.data(),
        0, nullptr);
}

void Utilities::CreateBuffer(
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
    VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo = Utilities::getBufferCreateInfo(
        size, usage, sharingMode, flags, queueFamilies);

#ifdef WIN32
    VkExternalMemoryHandleTypeFlags type = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR;
#else
    VkExternalMemoryHandleTypeFlags type = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR;
#endif

    VkExternalMemoryBufferCreateInfo externalInfo = {
        .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .handleTypes = type
    };

    if (external)
        bufferInfo.pNext = &externalInfo;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = vma_usage;
    allocInfo.flags = vma_flags;

    VmaAllocation allocation;
    vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);

    /*VkBufferCreateInfo bufferInfo = Utilities::getBufferCreateInfo(
        size, usage, sharingMode, flags, queueFamilies);

#ifdef WIN32
    VkExternalMemoryHandleTypeFlags type = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR;
#else
    VkExternalMemoryHandleTypeFlags type = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR;
#endif

    VkExternalMemoryBufferCreateInfo externalInfo = {
        .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .handleTypes = type
    };

    if (external)
        bufferInfo.pNext = &externalInfo;


    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    //VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT
    //VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT

    VkExportMemoryAllocateInfo exportInfo = {
        .sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO,
        .handleTypes = type
    };

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    if (external)
        allocInfo.pNext = &exportInfo;

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);*/
}

void Utilities::CreateBuffer(
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
    VkDeviceMemory& bufferMemory)
{
    Utilities::CreateBuffer(
        physicalDevice,
        device,
        stride * num_elements,
        usage,
        sharingMode,
        external,
        flags,
        vma_flags,
        vma_usage, 
        properties,
        queueFamilies,
        buffer,
        bufferMemory
    );
}

void Utilities::CreateImage(
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
    VkDeviceMemory& bufferMemory)
{
    VkImageCreateInfo imageInfo = Utilities::getImageCreateInfo(
        VK_IMAGE_TYPE_2D,
        usage,
        sharingMode,
        queueFamilies,
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        1, 1, 1,
        format,
        tiling,
        preinitialized);

    if (vkCreateImage(device, &imageInfo, nullptr, &textureImage) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Utilities::findMemoryType(
        physicalDevice,
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate image memory");
    }

    vkBindImageMemory(device, textureImage, bufferMemory, 0);
}

uint32_t Utilities::findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; memProperties.memoryTypeCount; i++) {
        if (typeFilter & (1 << i) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}

void Utilities::CopyBuffer(VkDevice& device, VkQueue& queue, VkCommandBuffer& commandBuffer, VkFence& wait_fence, VkBuffer& srcBuffer, VkBuffer& dstBuffer, int src_start, int dst_start, VkDeviceSize size)
{
    Utilities::beginSingleTimeCommands(commandBuffer);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = src_start;
    copyRegion.dstOffset = dst_start;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    Utilities::endSingleTimeCommands(device, commandBuffer, queue, wait_fence);
}

void Utilities::FlushToBuffer(VkDevice& device, VkDeviceMemory& memory, VkDeviceSize size, void*& data, void* src, bool unmap)
{
    vkMapMemory(device, memory, 0, size, 0, &data);
    memcpy(data, src, static_cast<size_t>(size));
    if (unmap)
    {
        vkUnmapMemory(device, memory);
    }
}

void Utilities::CopyBufferToImage(VkDevice& device, VkQueue& queue, VkCommandPool& cmdPool, VkFence& wait_fence, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer cmdBuf = Utilities::beginSingleTimeCommands(cmdPool, device);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        cmdBuf,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );


    Utilities::endSingleTimeCommands(device, cmdBuf, queue, wait_fence);
}

void Utilities::CreateCommandBuffer(VkCommandPool& cmdPool, VkDevice device, VkCommandBuffer& commandBuffer)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = cmdPool;
    allocInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
}

VkCommandBuffer Utilities::CreateCommandBuffer(VkCommandPool& cmdPool, VkDevice device)
{
    VkCommandBuffer cmdbuffer;
    Utilities::CreateCommandBuffer(cmdPool, device, cmdbuffer);
    return cmdbuffer;
}

void Utilities::beginSingleTimeCommands(VkCommandBuffer& cmdBuffer)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmdBuffer, &beginInfo);
}

VkCommandBuffer Utilities::beginSingleTimeCommands(VkCommandPool& cmdPool, VkDevice device)
{
    VkCommandBuffer commandBuffer = Utilities::CreateCommandBuffer(cmdPool, device);
    
    Utilities::beginSingleTimeCommands(commandBuffer);

    return commandBuffer;
}

void Utilities::endSingleTimeCommands(VkDevice& device, VkCommandBuffer& cmdBuffer, VkQueue& p_queue, VkFence& p_fence)
{
    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    vkQueueSubmit(p_queue, 1, &submitInfo, p_fence);
    vkWaitForFences(device, 1, &p_fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &p_fence);
    //vkQueueWaitIdle(p_queue);
}

VkShaderModule Utilities::createShaderModule(VkDevice& device, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    VkResult res = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
    if (res != VK_SUCCESS) {
        printf("vkCreateShaderModule Failed: %i\n", res);
    }

    return shaderModule;
}


// Extension methods


VkResult Extensions::CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Extensions::DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

