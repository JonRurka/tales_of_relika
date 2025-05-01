#include "ComputeEngine_VK.h"

#include "nvgl/extensions_gl.hpp"

#include "vulkan_utils.h"
#include "vk_mem_alloc.h"

//#include "../../Engine/include/opengl.h"



#ifdef WIN32
#include <vulkan/vulkan_win32.h>
#endif

#include "../../Engine/include/window.h"

#include "nvvk/gl_vk_vertex_buffer.h"

#include "window.h"
#include "Logger.h"

//VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

using namespace DynamicCompute::Compute::VK;

#define WAIT_IDLE 1

std::vector<VkExtensionProperties> ComputeEngine::mExtensions;
VkInstance ComputeEngine::mInstance;
nvvk::Context ComputeEngine::mVkctx;
VkDebugUtilsMessengerEXT ComputeEngine::mDebugMessenger;
bool ComputeEngine::mEnableValidationLayers;

bool ComputeEngine::mInitialized = false;

std::string ComputeEngine::mApp_dir = "";

std::list<ComputeContext*> ComputeEngine::mContexts;

const std::vector<const char*> ComputeEngine::validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

// ComputeEngine

int ComputeEngine::Init(std::string dir)
{
#ifdef NDEBUG
	mEnableValidationLayers = false;
#else
	mEnableValidationLayers = true;
#endif

	mApp_dir = dir;

	VkResult res = createInstance();

	if (res == VK_SUCCESS) {

		setupDebugMessenger();

		mInitialized = true;
	}

	return (int)res;
}

ComputeContext* ComputeEngine::GetNewContext(Vulkan_Device_Info device)
{
	mContexts.emplace_back(new ComputeContext(&mInstance, device));
	auto buf = mContexts.back();
	buf->mCanCallDispose = true;
	return buf;
}

std::string ComputeEngine::Get_VK_Version()
{
	return "1.3";
}

VkResult ComputeEngine::createInstance()
{

	nvvk::ContextCreateInfo deviceInfo;

	deviceInfo.verboseAvailable = false;
	deviceInfo.verboseUsed = false;

	deviceInfo.addInstanceExtension(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
	deviceInfo.addInstanceExtension(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME);
	deviceInfo.addDeviceExtension(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);
	deviceInfo.addDeviceExtension(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME);
#ifdef WIN32
	deviceInfo.addDeviceExtension(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME);
	deviceInfo.addDeviceExtension(VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME);
#else
	deviceInfo.addDeviceExtension(VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME);
	deviceInfo.addDeviceExtension(VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME);
#endif

	if (mEnableValidationLayers && !Utilities::checkValidationLayerSupport(validationLayers))
	{
		mEnableValidationLayers = false;
	}

	// Creating the Vulkan instance and device
	if (!mVkctx.init(deviceInfo))
	{
		printf("Could not initialize the Vulkan instance and device! See the above messages for more info.\n");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	// Initialize function pointers
#if VK_HEADER_VERSION >= 304
	vk::detail::DynamicLoader dl;
#else
	vk::DynamicLoader dl;
#endif
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(mVkctx.m_instance, mVkctx.m_device);

	mInstance = mVkctx.m_instance;

	mExtensions = Utilities::EnumerateInstanceExtensionProperties();

	return VK_SUCCESS;

	/*
	if (mEnableValidationLayers && !Utilities::checkValidationLayerSupport(validationLayers))
	{
		mEnableValidationLayers = false;
	}

	VkApplicationInfo appInfo = Utilities::getApplicationInfo(
		"Dynamics_IO_VK",
		VK_MAKE_VERSION(1, 0, 0),
		"Dynamics_IO",
		VK_MAKE_VERSION(1, 0, 0),
		VK_API_VERSION_1_3);

	std::vector<const char*> requiredExtensions = getRequiredExtensions();

	VkInstanceCreateInfo createInfo = Utilities::getInstanceCreateInfo(appInfo, requiredExtensions);

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (mEnableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	// TODO: Add MacOS support

	mExtensions = Utilities::EnumerateInstanceExtensionProperties();

	VkResult res;
	res = vkCreateInstance(&createInfo, nullptr, &mInstance);
	if (res != VK_SUCCESS) {
		printf("vkCreateInstance Failed: %i\n", res);
	}
	*/
	//nvvk::run_test();

	//return res;
}

void ComputeEngine::setupDebugMessenger()
{
	if (!mEnableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);

	VkResult res = Extensions::CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger);
	if (res != VK_SUCCESS) {
		printf("CreateDebugUtilsMessengerEXT Failed: %i\n", res);
	}
}

std::vector<const char*> ComputeEngine::getRequiredExtensions()
{
	std::vector<const char*> extensions;

	if (mEnableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	extensions.insert(extensions.end(), ComputeEngine::InstanceExtensions.begin(), ComputeEngine::InstanceExtensions.end());

	return extensions;
}

void ComputeEngine::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

const std::vector<const char*> ComputeEngine::GetValidationLayers() {
	return validationLayers;
}

void ComputeEngine::Dispose()
{
	//printf("ComputeEngine Dispose called.\n");
	if (!ComputeEngine::mInitialized)
		return;

	for (auto contxt : mContexts) {
		contxt->Dispose();
	}

	mContexts.clear();

	if (mEnableValidationLayers) {
		Extensions::DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
	}

	vkDestroyInstance(mInstance, nullptr);

	ComputeEngine::mInitialized = false;
	//printf("ComputeEngine Dispose completed.\n");
}

VKAPI_ATTR VkBool32 VKAPI_CALL ComputeEngine::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT ||
		messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		std::cerr << "validation layer ERROR: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
	//std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}


// Compute Context

ComputeContext::ComputeContext(VkInstance* instance, Vulkan_Device_Info device) {
	mVkctx = ComputeEngine::NVVK_Context();

	bool deviceFound = false;

	mInstance = instance;
	mPhysicalDevice = mVkctx->m_physicalDevice;
	deviceFound = true;

	/*std::vector<VkPhysicalDevice> devices = Utilities::EnumeratePhysicalDevices(*instance);
	
	bool deviceFound = false;
	for (VkPhysicalDevice dvs : devices) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(dvs, &deviceProperties);

		// TODO: Implement better way to match device.
		if (deviceProperties.deviceID == device.Device_ID) {
			mPhysicalDevice = dvs;
			deviceFound = true;
			break;
		}
	}*/

	if (!deviceFound) {
		printf("Physical device not found!\n");
		return;
	}
	else {
		printf("Found device '%s'\n", device.Name);
	}

	//mIndices = Utilities::findQueueFamilies(mPhysicalDevice);

	Utilities::QueueFamilyIndices indices;
	indices.shouldIncludeGraphics = false;
	indices.computeFamily = mVkctx->m_queueGCT.familyIndex;
	indices.transferFamily = mVkctx->m_queueGCT.familyIndex;
	indices.graphicsFamily = mVkctx->m_queueGCT.familyIndex;
	mIndices = indices;

	VkResult res = createLogicalDevice();

	getQueueInstances();

	bool success = createCommandPools();

	if (success)
		createCommandBuffers();

	//mBuffers.resize(4);
	//printf("ComputeContext parameter constructor.\n");

}

VkQueue* ComputeContext::GetPreferedComputeQueue()
{
	// TODO: Better logic to determine this
	return &mComputeQueue;
}

VkQueue* ComputeContext::GetPreferedTransferQueue()
{
	// TODO: Better logic to determine this
	return &mTransferQueue;
}

VkCommandBuffer* ComputeContext::GetPreferedComputeCmdBuffer()
{
	// TODO: Better logic to determine this
	return &mComputeCmdBuffer;
}

VkCommandBuffer* ComputeContext::GetPreferedTransferCmdBuffer()
{
	// TODO: Better logic to determine this
	return &mTransferCmdBuffer;
}

ComputeProgram* ComputeContext::Add_Program(std::string name) {
	programs[name] = new ComputeProgram(name, this);
	programs[name]->mCanCallDispose = true;
	return programs[name];
}

ComputeProgram* ComputeContext::Add_Program_Source(std::string name, const char* source) {
	if (programs.count(name) == 0)
	{
		Add_Program(name);
	}
	int res = programs[name]->Set_Source(source);
	return programs[name];
}

ComputeProgram* ComputeContext::Add_Program_SPIRV(std::string name, const void* binary, size_t length) {
	if (programs.count(name) == 0)
	{
		Add_Program(name);
	}
	int res = programs[name]->Set_Binary(binary, length);
	return programs[name];
}

ComputeProgram* ComputeContext::Add_Program_SPIRV_File(std::string name, std::string file_path)
{
	if (programs.count(name) == 0)
	{
		Add_Program(name);
	}
	int res = programs[name]->Set_Binary_File(file_path);
	return programs[name];
}

ComputeProgram* ComputeContext::Programs(std::string name) {
	if (programs.count(name) > 0) {
		return programs[name];
	}
	return nullptr;
}

ComputeKernel* ComputeContext::GetKernel(std::string p_name, std::string name) {
	return Programs(p_name)->GetKernel(name);
}

ComputeBuffer* ComputeContext::CreateBuffer(ComputeBuffer::Buffer_Type type, size_t size, bool external) {

	mBuffers.emplace_back(new ComputeBuffer(this, type, size, external));
	auto& buf = mBuffers.back();
	buf->mCanCallDispose = true;
	return buf;
}

VkResult ComputeContext::createLogicalDevice()
{

	mDevice = mVkctx->m_device;

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	if (vkCreateFence(mDevice, &fence_info, nullptr, &mWaitFence) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create fence!");
	}

#ifdef WIN32
	vkGetMemoryWin32HandleKHR = (PFN_vkGetMemoryWin32HandleKHR)vkGetDeviceProcAddr(mDevice, "vkGetMemoryWin32HandleKHR");
#endif

	//Utilities::Create_NVVK_Allocator(mPhysicalDevice, mDevice);
	//load_GL(window::sysGetProcAddress);


	//nvvk::gl_vk_buffer::run_test(window::glfw_window(), *mInstance, mDevice, mPhysicalDevice);
	//nvvk::gl_vk_buffer::run_test();
	//printf("executed glCreateBuffers\n");

	VkResult res = VK_SUCCESS;
	return res;

	/*
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;


	std::vector<uint32_t> uniqueQueueFamilies;
	if (mIndices.computeFamily.value() != mIndices.transferFamily.value())
	{
		if (IncludeGraphics)
			uniqueQueueFamilies.push_back(mIndices.graphicsFamily.value());
		uniqueQueueFamilies.push_back(mIndices.transferFamily.value());
		uniqueQueueFamilies.push_back(mIndices.computeFamily.value());
		//printf("ComputeContext::createLogicalDevice(): Seperate Queue Families!\n");
	}
	else 
	{
		if (IncludeGraphics)
			uniqueQueueFamilies.push_back(mIndices.graphicsFamily.value());
		uniqueQueueFamilies.push_back(mIndices.computeFamily.value());
		//printf("ComputeContext::createLogicalDevice(): Shared Queue Family!\n");
	}

	float queuePriority = 1.0f;
	for (uint32_t queueFam : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = Utilities::getDeviceQueueCreateInfo(queueFam, 1, queuePriority);
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = Utilities::getPhysicalDeviceFeatures();

	VkDeviceCreateInfo createInfo = Utilities::getDeviceCreateInfo(queueCreateInfos, deviceFeatures, (std::vector<const char*>&)DeviceExtensions);

	const std::vector<const char*> layers = ComputeEngine::GetValidationLayers();
	if (ComputeEngine::ValidationEnabled()) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		createInfo.ppEnabledLayerNames = layers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	VkResult res;
	res = vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice);
	if (res != VK_SUCCESS) {
		printf("vkCreateDevice Failed: %i\n", res);
	}

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	if (vkCreateFence(mDevice, &fence_info, nullptr, &mWaitFence) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create fence!");
	}

#ifdef WIN32
	vkGetMemoryWin32HandleKHR = (PFN_vkGetMemoryWin32HandleKHR)vkGetDeviceProcAddr(mDevice, "vkGetMemoryWin32HandleKHR");
#endif

	//Utilities::Create_VMA_Allocator(*mInstance, mPhysicalDevice, mDevice);
	//Utilities::Create_NVVK_Allocator(mPhysicalDevice, mDevice);
	//load_GL(window::sysGetProcAddress);

	//nvvk::run_test();
	nvvk::run_test(*mInstance, mDevice, mPhysicalDevice);

	return res;*/
}

void ComputeContext::getQueueInstances()
{
	vk::Device device = mVkctx->m_device;
	if (IncludeGraphics)
	{
		mGraphicsQueue = device.getQueue(mIndices.graphicsFamily.value(), 0);
	}
	mComputeQueue = device.getQueue(mIndices.computeFamily.value(), 0);
	mTransferQueue = device.getQueue(mIndices.transferFamily.value(), 0);
	/*
	if (IncludeGraphics)
		vkGetDeviceQueue(mDevice, mIndices.graphicsFamily.value(), 0, &mGraphicsQueue);
	vkGetDeviceQueue(mDevice, mIndices.computeFamily.value(), 0, &mComputeQueue);
	vkGetDeviceQueue(mDevice, mIndices.transferFamily.value(), 0, &mTransferQueue);
	*/
}

bool ComputeContext::createCommandPools()
{
	VkCommandPoolCreateInfo graphics_poolInfo = Utilities::getCommandPoolCreateInfo(
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		IncludeGraphics ? mIndices.graphicsFamily.value() : 0
	);

	VkCommandPoolCreateInfo compute_poolInfo = Utilities::getCommandPoolCreateInfo(
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		mIndices.computeFamily.value()
	);

	VkCommandPoolCreateInfo transfer_poolInfo = Utilities::getCommandPoolCreateInfo(
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		mIndices.transferFamily.value()
	);

	if (IncludeGraphics)
	{
		if (vkCreateCommandPool(mDevice, &graphics_poolInfo, nullptr, &mGraphicsCmdPool) != VK_SUCCESS) {
			printf("Failed to create GraphicsCmdPool.\n");
			return false;
		}
	}

	if (vkCreateCommandPool(mDevice, &compute_poolInfo, nullptr, &mComputeCmdPool) != VK_SUCCESS) {
		printf("Failed to create ComputeCmdPool.\n");
		return false;
	}

	if (vkCreateCommandPool(mDevice, &transfer_poolInfo, nullptr, &mTransferCmdPool) != VK_SUCCESS) {
		printf("Failed to create TransferCmdPool.\n");
		return false;
	}

	return true;
}

void ComputeContext::createCommandBuffers()
{
	if (IncludeGraphics)
		Utilities::CreateCommandBuffer(mGraphicsCmdPool, mDevice, mGraphicsCmdBuffer);
	Utilities::CreateCommandBuffer(mComputeCmdPool, mDevice, mComputeCmdBuffer);
	Utilities::CreateCommandBuffer(mTransferCmdPool, mDevice, mTransferCmdBuffer);
}

void ComputeContext::Dispose() {
	//printf("ComputeContext Dispose called.\n");
	if (mDestroyed || !mCanCallDispose)
		return;

	//printf("Dispose command pools.\n");
	if (IncludeGraphics)
		vkDestroyCommandPool(mDevice, mGraphicsCmdPool, nullptr);
	vkDestroyCommandPool(mDevice, mComputeCmdPool, nullptr);
	vkDestroyCommandPool(mDevice, mTransferCmdPool, nullptr);

	for (auto [key, prog] : programs) {
		prog->Dispose();
	}

	for (auto buffer : mBuffers) {
		buffer->Dispose();
	}

	programs.clear();
	mBuffers.clear();

	//printf("Dispose logical device\n");
	vkDestroyDevice(mDevice, nullptr);

	mDestroyed = true;
	//printf("ComputeContext Dispose completed.\n");
}

ComputeContext::~ComputeContext() {
	Dispose();
}


// Compute Program

ComputeProgram::ComputeProgram(std::string name, ComputeContext* context) {
	mName = name;
	mContext = context;
	mProgramModule = nullptr;
	mDevice = context->GetLogicalDevice();
	//printf("ComputeProgram parameter constructor.\n");
}

int ComputeProgram::Set_Source(const char* source) {
	// TODO: implement SPIRV generation from source.
	return 0;
}

int ComputeProgram::Set_Binary(const void* binary, size_t length) {

	char* char_binary = (char*) binary;
	std::vector<char> shader_bin (char_binary, char_binary + length);

	mProgramModule = Utilities::createShaderModule(*mDevice, shader_bin);
	mInitialized = true;

	return 0;
}

int ComputeProgram::Set_Binary_File(std::string file_path) {
	//printf("ComputeProgram Set_Binary_File called.\n");
	auto computeShaderBin = Utilities::readFile(file_path);

	mProgramModule = Utilities::createShaderModule(*mDevice, computeShaderBin);
	mInitialized = true;

	return 0;
}

ComputeKernel* ComputeProgram::GetKernel(std::string name) {
	if (kernels.count(name) > 0)
	{
		return kernels[name];
	}

	kernels[name] = new ComputeKernel(name, this);
	kernels[name]->SetWorkGroupSize(mKernelsWorkGroupSize);
	kernels[name]->mCanCallDispose = true;
	return kernels[name];
}

void ComputeProgram::Dispose() {
	//printf("ComputeProgram Dispose called for '%s'.\n", mName.c_str());
	if (mDestroyed || !mInitialized || !mCanCallDispose)
		return;

	for (auto [key, krn] : kernels) {
		krn->Dispose();
	}

	kernels.clear();

	mDestroyed = true;
	mInitialized = false;
	//printf("ComputeProgram Dispose completed.\n");
}

int ComputeProgram::Buildkernels() {
	VkResult res = VK_ERROR_UNKNOWN;

	for (auto& [key, value] : kernels) {
		res = value->BuildKernel();

		if (res != VK_SUCCESS)
			break;
	}

	//printf("Dispose ProgramModule for program '%s'.\n", mName.c_str());
	vkDestroyShaderModule(*mDevice, mProgramModule, nullptr);

	return res;
}

ComputeProgram::~ComputeProgram() {
	Dispose();
}


// Compute Kernel

ComputeKernel::ComputeKernel(std::string name, ComputeProgram* program) {
	mName = name;
	mProgram = program;
	mProgramModule = program->GetProgramModule();
	mDevice = program->GetLogicalDevice();

	ComputeContext* context = program->GetContext();

	mComputeQueue = context->GetPreferedComputeQueue();
	mTransferQueue = context->GetPreferedTransferQueue();

	mComputeCmdBuffer = context->GetPreferedComputeCmdBuffer();
	mTransferCmdBuffer = context->GetPreferedTransferCmdBuffer();

	mWorkGroupSize = glm::uvec3(DEFAULT_WORK_GROUP_SIZE);
}

int ComputeKernel::SetBuffer(ComputeBuffer* buffer, int arg)
{
	BoundBuffer buffer_info{};
	buffer_info.Buffer = buffer;
	buffer_info.BindIndex = arg;

	mBoundBuffers.push_back(buffer_info);

	return 0;
}

VkResult ComputeKernel::BuildKernel()
{
	//printf("createDescriptorSetLayout()\n");
	VkResult res = createDescriptorSetLayout();
	if (res != VK_SUCCESS)
		return res;

	//printf("createComputePipeline()\n");
	res = createComputePipeline();
	if (res != VK_SUCCESS){
		printf("createComputePipeline Failed: %i\n", res);
		return res;
	}

	//printf("createDescriptorPool()\n");
	res = createDescriptorPool();
	if (res != VK_SUCCESS) {
		printf("createDescriptorPool Failed: %i\n", res);
		return res;
	}

	//printf("createDescriptorSets()\n");
	res = createDescriptorSets();
	if (res != VK_SUCCESS) {
		printf("createDescriptorSets Failed: %i\n", res);
		return res;
	}

	mBoundBuffers.clear();

	/*VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	if (vkCreateFence(*mDevice, &fence_info, nullptr, &mFinished_fence) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create fence!");
	}*/
	mFinished_fence = *mProgram->GetContext()->GetWaitFence();

	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	for (int i = 0; i < DEFAULT_AVAILABLE_SEMAPHORE; i++) {
		VkSemaphore semaphore;
		if (vkCreateSemaphore(*mDevice, &semaphore_info, VK_NULL_HANDLE, &semaphore) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create semaphore!");
		}
		mAvailable_Semaphore.push(semaphore);
	}

	//printf("BuildKernel() complete!\n");

	mInitialized = true;

	return res;
}

int ComputeKernel::Execute(uint32_t x, uint32_t y, uint32_t z)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	//vkQueueWaitIdle(*mComputeQueue);
	if (vkBeginCommandBuffer(*mComputeCmdBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer!");
	}
	//printf("ComputeKernel::Execute: Start execute '%s'\n", mName);

	vkCmdBindPipeline(*mComputeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipeline);

	vkCmdBindDescriptorSets(
		*mComputeCmdBuffer,
		VK_PIPELINE_BIND_POINT_COMPUTE,
		mComputePipelineLayout,
		0, 1,
		&mComputeDescriptorSet,
		0, 0);

	vkCmdDispatch(
		*mComputeCmdBuffer, 
		std::max((int)std::ceilf((float)x / (float)mWorkGroupSize.x), 1),
		std::max((int)std::ceilf((float)y / (float)mWorkGroupSize.y), 1),
		std::max((int)std::ceilf((float)z / (float)mWorkGroupSize.z), 1));

	if (vkEndCommandBuffer(*mComputeCmdBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer!");
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = mComputeCmdBuffer;

	if (vkQueueSubmit(*mComputeQueue, 1, &submitInfo, mFinished_fence) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit compute command buffer!");
	}
	//printf("ComputeKernel::Execute: Finish execute submit '%s'\n", mName);
#if WAIT_IDLE == 1
	vkQueueWaitIdle(*mComputeQueue);
#endif
	vkWaitForFences(*mDevice, 1, &mFinished_fence, VK_TRUE, UINT64_MAX);
	vkResetFences(*mDevice, 1, &mFinished_fence);
	//printf("ComputeKernel::Execute: Finish execute wait '%s'\n", mName);

	return 0;
}

int ComputeKernel::ExecuteBatch(uint32_t num, uint32_t x, uint32_t y, uint32_t z)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	vkQueueWaitIdle(*mComputeQueue);
	if (vkBeginCommandBuffer(*mComputeCmdBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer!");
	}
	//printf("ComputeKernel::Execute: Start execute '%s'\n", mName);

	std::vector<VkSemaphore> semaphores;
	for (int i = 0; i < num; i++) {
		semaphores.push_back(mAvailable_Semaphore.front());
		mAvailable_Semaphore.pop();
	}


	vkCmdBindPipeline(*mComputeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipeline);

	vkCmdBindDescriptorSets(
		*mComputeCmdBuffer,
		VK_PIPELINE_BIND_POINT_COMPUTE,
		mComputePipelineLayout,
		0, 1,
		&mComputeDescriptorSet,
		0, 0);

	vkCmdDispatch(
		*mComputeCmdBuffer,
		std::max((int)std::ceilf((float)x / (float)mWorkGroupSize.x), 1),
		std::max((int)std::ceilf((float)y / (float)mWorkGroupSize.y), 1),
		std::max((int)std::ceilf((float)z / (float)mWorkGroupSize.z), 1));

	if (vkEndCommandBuffer(*mComputeCmdBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer!");
	}

	std::vector<VkSubmitInfo> submits;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };

	for (int i = 0; i < num; i++) {
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = mComputeCmdBuffer;

		if (i != 0) {
			submitInfo.pWaitSemaphores = &semaphores[i - 1];
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitDstStageMask = waitStages;
		}

		submitInfo.pSignalSemaphores = &semaphores[i];
		submitInfo.signalSemaphoreCount = 1;

		submits.push_back(submitInfo);
	}
	
	/*for (int i = 0; i < num; i++) {
		VkFence f_fence = (i == num - 1) ? mFinished_fence : nullptr;
		if (vkQueueSubmit(*mComputeQueue, 1, &submits[i], f_fence) != VK_SUCCESS) {
			throw std::runtime_error("Failed to submit compute command buffer!");
		}
	}*/

	if (vkQueueSubmit(*mComputeQueue, num, submits.data(), mFinished_fence) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit compute command buffer!");
	}


	//printf("ComputeKernel::Execute: Finish execute submit '%s'\n", mName);
	//vkQueueWaitIdle(*mComputeQueue);
	vkWaitForFences(*mDevice, 1, &mFinished_fence, VK_TRUE, UINT64_MAX);
	vkResetFences(*mDevice, 1, &mFinished_fence);
	//printf("ComputeKernel::Execute: Finish execute wait '%s'\n", mName);

	return 0;


	return 0;
}

VkResult ComputeKernel::createDescriptorSetLayout()
{
	int numBuffers = mBoundBuffers.size();
	//printf("Binding %i buffers\n", numBuffers);
	return Utilities::CreateComputeDescriptorSetLayout(*mDevice, numBuffers, mComputeDescriptorSetLayout);
}

VkResult ComputeKernel::createComputePipeline()
{
	//printf("Utilities::getPipelineShaderStageCreateInfo\n");
	VkPipelineShaderStageCreateInfo computeShaderStageInfo = Utilities::getPipelineShaderStageCreateInfo(*mProgramModule, mName.c_str());

	//printf("Utilities::getPipelineLayoutCreateInfo\n");
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = Utilities::getPipelineLayoutCreateInfo(mComputeDescriptorSetLayout);

	//printf("vkCreatePipelineLayout\n");
	VkResult res = vkCreatePipelineLayout(*mDevice, &pipelineLayoutInfo, nullptr, &mComputePipelineLayout);

	if (res != VK_SUCCESS) {
		printf("vkCreatePipelineLayout Failed: %i\n", res);
		return res;
	}

	//printf("Utilities::getComputePipelineCreateInfo\n");
	VkComputePipelineCreateInfo pipelineInfo = Utilities::getComputePipelineCreateInfo(mComputePipelineLayout, computeShaderStageInfo);

	//printf("vkCreateComputePipelines\n");
	res = vkCreateComputePipelines(*mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mComputePipeline);
	if (res != VK_SUCCESS) {
		printf("vkCreateComputePipelines Failed: %i\n", res);
	}

	return res;
}

VkResult ComputeKernel::createDescriptorPool()
{
	int numBuffers = mBoundBuffers.size();

	return Utilities::createDescriptorPool(*mDevice, numBuffers, 2, mDescriptorPool);
}

VkResult ComputeKernel::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(1, mComputeDescriptorSetLayout);

	VkResult res = Utilities::AllocateDescriptorSets(*mDevice, layouts, mDescriptorPool, 1, mComputeDescriptorSet);

	if (res != VK_SUCCESS)
		return res;

	int numBuffers = mBoundBuffers.size();

	std::vector<Utilities::DescriptorBufferInfo> descriptorBufferInfo(numBuffers);

	for (int i = 0; i < numBuffers; i++) {
		Utilities::DescriptorBufferInfo bufferInfo{};

		BoundBuffer bufferBindInfo = mBoundBuffers[i];
		ComputeBuffer* comp_buffer = bufferBindInfo.Buffer;

		bufferInfo.buffer = *comp_buffer->Get_VK_Mem(); // TODO: populate buffer info based on bound buffers
		bufferInfo.size = comp_buffer->GetSize();
		bufferInfo.binding = bufferBindInfo.BindIndex;

		descriptorBufferInfo[i] = bufferInfo;
	}

	Utilities::UpdateDescriptorSets_StorageBuffers(*mDevice, mComputeDescriptorSet, descriptorBufferInfo);

	return res;
}

void ComputeKernel::Dispose() {
	//printf("ComputeKernel Dispose called for '%s'.\n", mName.c_str());
	if (mDestroyed || !mInitialized || !mCanCallDispose)
		return;

	//printf("vkDeviceWaitIdle for kernel '%s'.\n", mName.c_str());
	vkDeviceWaitIdle(*mDevice);
	//printf("vkDeviceWaitIdle for kernel '%s' finished.\n", mName.c_str());

	//printf("Free DescriptorSets for kernel '%s'.\n", mName.c_str());
	vkFreeDescriptorSets(*mDevice, mDescriptorPool, 1, &mComputeDescriptorSet);

	//printf("Dispose ComputeDescriptorSetLayout for kernel '%s'.\n", mName.c_str());
	vkDestroyDescriptorSetLayout(*mDevice, mComputeDescriptorSetLayout, nullptr);

	//printf("Dispose DescriptorPool for kernel '%s'.\n", mName.c_str());
	vkDestroyDescriptorPool(*mDevice, mDescriptorPool, nullptr);

	//printf("Dispose ComputePipeline for kernel '%s'.\n", mName.c_str());
	vkDestroyPipeline(*mDevice, mComputePipeline, nullptr);
	
	//printf("Dispose ComputePipelineLayout for kernel '%s'.\n", mName.c_str());
	vkDestroyPipelineLayout(*mDevice, mComputePipelineLayout, nullptr);
	 

	mDestroyed = true;
	mInitialized = false;

	//printf("ComputeKernel Dispose completed.\n");
}

ComputeKernel::~ComputeKernel() {
	Dispose();
}


// Compute Buffer

ComputeBuffer::ComputeBuffer(ComputeContext* context, Buffer_Type type, VkDeviceSize size, bool external) {
	mContext = context;
	mType = type;
	mSize = size;

	mPhysicalDevice = context->GetPhysicalDevice();
	mLogicalDevice = context->GetLogicalDevice();

	mTransferQueue = context->GetPreferedTransferQueue();
	mTransferCmdBuffer = context->GetPreferedTransferCmdBuffer();

	getAllQueueFamilies();

	mIs_External = external;

	// Marks whether this is tranfering from or to the staging buffer.
	// If it Read Only, it should be the destination (DST) from the staging buffer. (Host -> Buffer_DST)
	// If it is Write Only, it should be the src of the staging buffer. (Buffer_SRC -> Host)
	// If it is Read and Write, it should be both.
	switch (mType) {
	case Buffer_Type::READ:
		mStage_transfer_flag = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		mTransfer_flag = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		break;

	case Buffer_Type::Write:
		mTransfer_flag = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		mStage_transfer_flag = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		break;

	case Buffer_Type::Read_Write:
		mTransfer_flag = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		mStage_transfer_flag = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		break;
	}

	if (external) {
		//nvvk::ExportResourceAllocatorDedicated* alloc = Utilities::Get_NVVK_Allocator();

		// VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT

		/*
		m_gl_vk_buffer.bufVk = alloc->createBuffer(
			mSize, 
			VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		nvvk::MemAllocator::MemInfo info = alloc->getMemoryAllocator()->getMemoryInfo(m_gl_vk_buffer.bufVk.memHandle);
		mBuffer = m_gl_vk_buffer.bufVk.buffer;
		mBufferMemory = info.memory;
		
		initExternalCopy();
		*/
		m_gl_vk_buffer = nvvk::gl_vk_buffer::createBufferVK(mSize);
		mBuffer = m_gl_vk_buffer.bufVk.buffer;
		mBufferMemory = nvvk::gl_vk_buffer::get_buffer_mem(m_gl_vk_buffer.bufVk.memHandle);
		mExternalBuffer = m_gl_vk_buffer.oglId;
		mExternalMemObj = m_gl_vk_buffer.memoryObject;
		//nvvk::gl_vk_buffer::poll_events();

#if WIN32
		mFD = m_gl_vk_buffer.handle;
#else
		mFD = m_gl_vk_buffer.fd;
#endif

	}
	else {
		Utilities::CreateBuffer(
			*mPhysicalDevice,
			*mLogicalDevice,
			mSize,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | mTransfer_flag,
			VK_SHARING_MODE_CONCURRENT,
			external,
			(VkBufferCreateFlags)0,
			(VmaAllocationCreateFlags)0,
			VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, //VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			mAllQueueFamilies,
			mBuffer,
			mBufferMemory
		);
	}

	Utilities::CreateBuffer(
		*mPhysicalDevice,
		*mLogicalDevice,
		mSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | mStage_transfer_flag,
		VK_SHARING_MODE_CONCURRENT, 
		false,
		(VkBufferCreateFlags)0,
		(VmaAllocationCreateFlags)0,
		VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		mAllQueueFamilies,
		stagingBuffer,
		stagingBufferMemory
	);

	//printf("ComputeBuffer parameter constructor called,\n");
}

int ComputeBuffer::SetData(void* src_data) {
	//VkBuffer stagingBuffer;
	//VkDeviceMemory stagingBufferMemory;

	/*Utilities::CreateBuffer(
		*mPhysicalDevice,
		*mLogicalDevice,
		mSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | mStage_transfer_flag,
		VK_SHARING_MODE_CONCURRENT,
		0,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		mAllQueueFamilies,
		stagingBuffer,
		stagingBufferMemory
	);*/

	return SetData(src_data, mSize);
}

int ComputeBuffer::GetData(void* outData) {

	/*Utilities::CreateBuffer(
		*mPhysicalDevice,
		*mLogicalDevice,
		mSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | mStage_transfer_flag,
		VK_SHARING_MODE_CONCURRENT,
		0,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		mAllQueueFamilies,
		stagingBuffer,
		stagingBufferMemory
	);*/

	return GetData(outData, mSize);
}

int ComputeBuffer::SetData(void* src_data, int size)
{
	return SetData(src_data, 0, size);
}

int ComputeBuffer::GetData(void* outData, int size)
{
	return GetData(outData, 0, size);
}

int ComputeBuffer::SetData(void* src_data, int DstStart, int size)
{
	// TODO: implement start offset.

	void* maped_data;

	//printf("ComputeBuffer::SetData: Start: %i\n", size);
	//vkQueueWaitIdle(*mTransferQueue);
	Utilities::FlushToBuffer(*mLogicalDevice, stagingBufferMemory, static_cast<uint64_t>(size), maped_data, src_data, true);

	Utilities::CopyBuffer(*mLogicalDevice, *mTransferQueue, *mTransferCmdBuffer, *mContext->GetWaitFence(), stagingBuffer, mBuffer, 0, DstStart, static_cast<uint64_t>(size));
	//printf("ComputeBuffer::SetData: Finish.\n");
	//vkDestroyBuffer(*mLogicalDevice, stagingBuffer, nullptr);
	//vkFreeMemory(*mLogicalDevice, stagingBufferMemory, nullptr);

	return 0;
}

int ComputeBuffer::GetData(void* outData, int SrcStart, int size)
{
	//printf("ComputeBuffer::GetData: Start: %i'\n", size);
	//vkQueueWaitIdle(*mTransferQueue);
	Utilities::CopyBuffer(*mLogicalDevice, *mTransferQueue, *mTransferCmdBuffer, *mContext->GetWaitFence(), mBuffer, stagingBuffer, SrcStart, 0, static_cast<uint64_t>(size));

	void* maped_data;
	vkMapMemory(*mLogicalDevice, stagingBufferMemory, 0, size, 0, &maped_data);
	memcpy(outData, maped_data, static_cast<uint32_t>(size));
	vkUnmapMemory(*mLogicalDevice, stagingBufferMemory);


	return 0;
}

int ComputeBuffer::CopyTo(ComputeBuffer* other)
{
	int size = std::min(mSize, other->mSize);
	return CopyTo(other, 0, 0, size);
}

int ComputeBuffer::CopyTo(ComputeBuffer* other, int size)
{
	return CopyTo(other, 0, 0, size);
}

int ComputeBuffer::CopyTo(ComputeBuffer* other, int srcStart, int dstStart, int size)
{
	if (mTransferQueue == other->mTransferQueue)
	{
		//vkQueueWaitIdle(*mTransferQueue);
	}
	else {
		vkQueueWaitIdle(*mTransferQueue);
		vkQueueWaitIdle(*other->mTransferQueue);
	}
	Utilities::CopyBuffer(*mLogicalDevice, *mTransferQueue, *mTransferCmdBuffer, *mContext->GetWaitFence(), mBuffer, other->mBuffer, srcStart, dstStart, static_cast<uint64_t>(size));
	return 0;
}

void ComputeBuffer::getAllQueueFamilies()
{
	std::vector<VkQueueFamilyProperties> queueFamilies = Utilities::GetPhysicalDeviceQueueFamilyProperties(*mPhysicalDevice);

	int i = 0;
	for (auto queueFam : queueFamilies) {
		if ((queueFam.queueFlags & VK_QUEUE_TRANSFER_BIT) ||
			(queueFam.queueFlags & VK_QUEUE_COMPUTE_BIT) ||
			(queueFam.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
			
			mAllQueueFamilies.push_back(i);
			//printf("ComputeBuffer: Queue Family: %i\n", i);
			i++;
		}
	}
}

void ComputeBuffer::initExternalCopy()
{
	nvvk::ExportResourceAllocatorDedicated* alloc = Utilities::Get_NVVK_Allocator();
	nvvk::MemAllocator::MemInfo info = alloc->getMemoryAllocator()->getMemoryInfo(m_gl_vk_buffer.bufVk.memHandle);

#ifdef WIN32
	PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR = mContext->get_GetMemoryWin32_func();

	VkMemoryGetWin32HandleInfoKHR handleInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR,
		.memory = info.memory,
		.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR
	};

	VkResult res = vkGetMemoryWin32HandleKHR(*mLogicalDevice, &handleInfo, &m_gl_vk_buffer.handle);
	if (res != VK_SUCCESS) {
		printf("Failed to get external memory handle.\n");
	}
	else {
		//printf("Handle created: %08x\n", mFD);
	}
	mFD = m_gl_vk_buffer.handle;
#else

	VkMemoryGetFdInfoKHR fdInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR,
		.memory = info.memory,
		.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT
	};

	VkResult res = vkGetMemoryFdKHR(*mLogicalDevice, &fdInfo, &m_gl_vk_buffer.fd);
	if (res != VK_SUCCESS) {
		printf("Failed to get external memory handle.\n");
	}
	else {
		//printf("Handle created: %08x\n", mFD);
	}
	mFD = m_gl_vk_buffer.fd;
#endif

	VkMemoryRequirements req{};
	vkGetBufferMemoryRequirements(*mLogicalDevice, m_gl_vk_buffer.bufVk.buffer, &req);

	glCreateBuffers(1, &m_gl_vk_buffer.oglId);
	mExternalBuffer = m_gl_vk_buffer.oglId;
	glCheckError();
	printf("executed glCreateBuffers\n");

	glCreateMemoryObjectsEXT(1, &m_gl_vk_buffer.memoryObject);
	mExternalMemObj = m_gl_vk_buffer.memoryObject;
	glCheckError();
	printf("executed glCreateMemoryObjectsEXT\n");

#ifdef WIN32
	glImportMemoryWin32HandleEXT(m_gl_vk_buffer.memoryObject, req.size, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, m_gl_vk_buffer.handle);
	glCheckError();
	printf("executed glImportMemoryWin32HandleEXT\n");
#else
	glImportMemoryFdEXT(m_gl_vk_buffer.memoryObject, req.size, GL_HANDLE_TYPE_OPAQUE_FD_EXT, m_gl_vk_buffer.fd);
	// fd got consumed
	m_gl_vk_buffer.fd = -1;
#endif

	glNamedBufferStorageMemEXT(m_gl_vk_buffer.oglId, req.size, m_gl_vk_buffer.memoryObject, info.offset);
	glCheckError();
	printf("executed glNamedBufferStorageMemEXT\n");


	printf("Created external buffer copy\n");

	/*
	glCreateMemoryObjectsEXT(1, &mExternalMemObj);

	VkMemoryRequirements req{};
	vkGetBufferMemoryRequirements(*mLogicalDevice, mBuffer, &req);
	printf("Memory: %i\n", (int)req.size);
	
#ifdef WIN32

	PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR = mContext->get_GetMemoryWin32_func();

	VkMemoryGetWin32HandleInfoKHR handleInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR,
		.pNext = NULL,
		.memory = mBufferMemory,
		.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR
	};

	VkResult res = vkGetMemoryWin32HandleKHR(*mLogicalDevice, &handleInfo, &mFD);
	if (res != VK_SUCCESS) {
		printf("Failed to get external memory handle.\n");
	}
	else {
		//printf("Handle created: %08x\n", mFD);
	}

	glImportMemoryWin32HandleEXT(mExternalMemObj, req.size, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, mFD);

#else
	VkMemoryGetFdInfoKHR fdInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR,
		.memory = mBufferMemory,
		.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT
	};

	vkGetMemoryFdKHR(*mLogicalDevice, &fdInfo, &mFD);

	glImportMemoryFdEXT(mExternalMemObj, mSize, GL_HANDLE_TYPE_OPAQUE_FD_EXT, mFD);
#endif

	//glGenBuffers(1, &mExternalBuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, mExternalBuffer);
	//glBufferStorageMemEXT(GL_ARRAY_BUFFER, mSize, mExternalMemObj, 0);
	//glBufferData(GL_ARRAY_BUFFER, mSize, NULL, GL_STATIC_READ);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &mExternalBuffer);
	//glNamedBufferStorage(mExternalBuffer, mSize, NULL, GL_MAP_READ_BIT);
	//glNamedBufferStorageMemEXT(mExternalBuffer, req.size, mExternalMemObj, 0);
	*/
}

VkResult ComputeBuffer::QueryVkExternalMemoryProperties()
{
	VkPhysicalDeviceImageFormatInfo2 format_info_2 = {
	  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,
	  .format = VK_FORMAT_UNDEFINED,
	  .type = VK_IMAGE_TYPE_1D,
	  .tiling = VK_IMAGE_TILING_LINEAR,
	  .usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
	  //.flags = flags,
	};

	VkPhysicalDeviceExternalImageFormatInfo external_info = {
	  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO,
	  .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT,
	};
	format_info_2.pNext = &external_info;

	VkImageFormatProperties2 image_format_properties_2 = {
	  .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2,
	};
	VkExternalImageFormatProperties external_image_format_properties = {
		.sType = VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES,
	};
	VkExternalBufferProperties external_buffer_properties = {
		.sType = VK_STRUCTURE_TYPE_EXTERNAL_BUFFER_PROPERTIES
	};

	image_format_properties_2.pNext = &external_buffer_properties;
	//VkResult result = vkGetPhysicalDeviceImageFormatProperties2(*mContext->GetPhysicalDevice(), &format_info_2, &image_format_properties_2);


	VkPhysicalDeviceExternalBufferInfo external_buffer_info{};
	external_buffer_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_BUFFER_INFO;
	external_buffer_info.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
	//external_buffer_info.flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	external_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	vkGetPhysicalDeviceExternalBufferProperties(*mContext->GetPhysicalDevice(), &external_buffer_info, &external_buffer_properties);

	//if (result != VK_SUCCESS) {

	//}

	printf("External mem feature: %08x\n", external_buffer_properties.externalMemoryProperties.externalMemoryFeatures);

	//external_image_format_properties.externalMemoryProperties
	return VK_SUCCESS;
}

void ComputeBuffer::Dispose() {
	//printf("ComputeBuffer Dispose called,\n");
	if (mDestroyed || !mCanCallDispose)
		return;

	if (mIs_External) {
		nvvk::gl_vk_buffer::destroyBufferVk(m_gl_vk_buffer);
	}
	else {
		//printf("Dispose buffer and buffer memory.\n");
		vkDestroyBuffer(*mLogicalDevice, mBuffer, nullptr);
		vkFreeMemory(*mLogicalDevice, mBufferMemory, nullptr);
	}
	
	//printf("Dispose staging buffer and buffer memory.\n");
	vkDestroyBuffer(*mLogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(*mLogicalDevice, stagingBufferMemory, nullptr);

	mDestroyed = true;

	//printf("ComputeBuffer Dispose completed,\n");
}

ComputeBuffer::~ComputeBuffer() {
	Dispose();
}


