#include "VulkanContext.h"

#include "SolanumConstants.h"

#if _DEBUG
constexpr bool useValidationLayers = true;
#else
constexpr bool useValidationLayers = false;
#endif

VulkanContext::VulkanContext(WindowBridge& window)
{
	auto vkbInstance = createInstance();
	surface = window.createSurface(instance);
	createDevice(vkbInstance);
	createSwapchain(window.getExtent());
}

VulkanContext::~VulkanContext()
{
	vkDestroySwapchainKHR(device, swapchain.swapchain, nullptr);
	// Swapchain images get destroyed automatically with the swapchain.
	// Only destroy the image views here.
	for (auto& imageView : swapchain.imageViews)
		vkDestroyImageView(device, imageView, nullptr);

	vkb::destroy_debug_utils_messenger(instance, debugMessenger);
	
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);
}

vkb::Instance VulkanContext::createInstance()
{
	vkb::InstanceBuilder builder;
	auto builderResult = builder
		.set_app_name(SolVK::AppName.c_str())
		.request_validation_layers(useValidationLayers)
		.use_default_debug_messenger()
		.require_api_version(1, 3, 0)
		.build();

	vkb::Instance vkbInstance = builderResult.value();

	instance = vkbInstance.instance;
	debugMessenger = vkbInstance.debug_messenger;

	return vkbInstance;
}

void VulkanContext::createDevice(vkb::Instance vkbInstance)
{
	// VK1.3
	VkPhysicalDeviceVulkan13Features features13{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features13.dynamicRendering = true; // enable dynamic rendering, no explicit render passes
	features13.synchronization2 = true; // enable streamlined sync structures 

	// VK1.2
	VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	features12.bufferDeviceAddress = true; // gpu pointers without binding buffers
	features12.descriptorIndexing = true; // bindless textures

	vkb::PhysicalDeviceSelector deviceSelector{ vkbInstance };
	vkb::PhysicalDevice vkbPhysicalDevice = deviceSelector
		.set_minimum_version(1, 3)
		.set_required_features_12(features12)
		.set_required_features_13(features13)
		.set_surface(surface)
		.prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
		.select()
		.value();

	physicalDevice = vkbPhysicalDevice.physical_device;

	vkb::DeviceBuilder deviceBuilder{ vkbPhysicalDevice };

	device = deviceBuilder.build().value();
}

void VulkanContext::createSwapchain(VkExtent2D windowExtent)
{
	vkb::SwapchainBuilder builder{ physicalDevice, device, surface };

	vkb::Swapchain vkbSwapchain = builder.set_desired_format(VkSurfaceFormatKHR{ .format = VK_FORMAT_R8G8B8A8_UNORM, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(windowExtent.width, windowExtent.height)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build()
		.value();

	swapchain.swapchain = vkbSwapchain.swapchain;
	// The actual extent may not be the same as the one passed in, so we take this one.
	swapchain.extent = vkbSwapchain.extent;
	swapchain.images = vkbSwapchain.get_images().value();
	swapchain.imageViews = vkbSwapchain.get_image_views().value();
}
