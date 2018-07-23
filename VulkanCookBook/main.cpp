#include "common.h"
#include <iostream>

int main() {

	//std::vector<VkExtensionProperties> properties;
	//vkTut::common::CheckAvailableInstanceExtensions(properties);

	std::vector<const char*> desiredExtensions = {
		"VK_KHR_surface",
		"VK_KHR_win32_surface"
	};

	VkInstance instance;
	vkTut::common::CreateInstance("Vulkan Test", { 1,0,0 }, "Test Engine", { 1,0,0 }, desiredExtensions, instance);

	std::vector<VkPhysicalDevice> devices;
	vkTut::common::CheckAvailablePhysicalDevices(instance, devices);
	std::cout << "Physical Device Count: " << devices.size() << std::endl;

	for (VkPhysicalDevice device : devices) {
		VkPhysicalDeviceFeatures features;
		VkPhysicalDeviceProperties properties;
		vkTut::common::CheckDeviceFeaturesAndProperties(device, features, properties);

		std::cout << "-------------- " << properties.deviceName << " -------------" << std::endl;
		std::cout << "Properties: " << std::endl;
		std::cout << "\t" << properties.apiVersion << std::endl;
		std::cout << "\t" << properties.deviceID << std::endl;
		std::cout << "\t" << properties.deviceName << std::endl;
		std::cout << "\t" << properties.deviceType << std::endl;
		std::cout << "\t" << properties.driverVersion << std::endl;
		std::cout << "\t" << properties.pipelineCacheUUID << std::endl;
		std::cout << "\t" << properties.vendorID << std::endl;

		std::vector<VkExtensionProperties> props;
		vkTut::common::CheckDeviceExtensions(device, props);
		std::cout << "Supported Extensions: " << std::endl;
		for (VkExtensionProperties prop : props) {
			std::cout << "\t" << prop.extensionName << std::endl;
		}

		std::cout << "Family Queue Properties: " << std::endl;
		std::vector<VkQueueFamilyProperties> queueFamilyProps;
		vkTut::common::CheckDeviceQueueFamilies(device, queueFamilyProps);
		for (VkQueueFamilyProperties queueFamilyProp : queueFamilyProps) {
			std::cout << "\tQueue Flags: " << queueFamilyProp.queueFlags << std::endl;
			std::cout << "\t\tQueue Count: " << queueFamilyProp.queueCount << std::endl;
			std::cout << "\t\tTimestamp Valid Bits: " << queueFamilyProp.timestampValidBits << std::endl;
			std::cout << "\t\tMin Image Transfer Granularity: " << std::endl;
			std::cout << "\t\t\tDepth: " << queueFamilyProp.minImageTransferGranularity.depth << std::endl;
			std::cout << "\t\t\tHeight: " << queueFamilyProp.minImageTransferGranularity.height << std::endl;
			std::cout << "\t\t\tWidth: " << queueFamilyProp.minImageTransferGranularity.width << std::endl;
		}
		uint32_t index;
		vkTut::common::GetDeviceQueueFamilyIndex(device, VK_QUEUE_GRAPHICS_BIT, index);
		std::cout << "\tFamily Index: " << index << std::endl;
		std::cout << "------------------------------------------------" << std::endl;
	}


	int a;
	std::cin >> a;

	return 0;
}