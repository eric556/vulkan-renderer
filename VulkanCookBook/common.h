#ifndef COMMON_H
#define COMMON_H
#include <vulkan\vulkan.hpp>
#include <vector>
#include <iostream>
#include <cstring>

namespace vkTut {
	namespace common {

		struct Version {
			int major;
			int minor;
			int patch;
		};

		struct QueueInfo {
			uint32_t familyIndex;
			std::vector<float> priorities;
		};

		bool CheckAvailableInstanceExtensions(std::vector<VkExtensionProperties>& extensions) {
			uint32_t extensionCount;
			VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			if (result != VK_SUCCESS || extensionCount == 0) {
				return false;
			}
			extensions.resize(extensionCount);
			result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
			if (result != VK_SUCCESS) {
				return false;
			}

			std::cout << "--------- Instance Exension Properties ---------" << std::endl;
			for (VkExtensionProperties extension : extensions) {
				std::cout << extension.extensionName << std::endl;
			}
			std::cout << "------------------------------------------------" << std::endl;

			return true;
		}

		bool IsExtensionSupported(std::vector<VkExtensionProperties>& properties, const char* extension) {
			auto iter = std::find_if(properties.begin(), properties.end(), [&extension](VkExtensionProperties v) { return std::strcmp(v.extensionName, extension) == 0; });
			return iter != properties.end();
		}

		bool CreateInstance(const char* applicationName, Version applicationVersion, const char* engineName, Version engineVersion, std::vector<const char *>& desired_extensions, VkInstance& instance) {
			std::vector<VkExtensionProperties> properties;
			if (!CheckAvailableInstanceExtensions(properties)) {
				std::cout << "Was not able to get available extensions" << std::endl;
				return false;
			}

			bool found = true;
			for (const char* str : desired_extensions) {
				if (!IsExtensionSupported(properties, str)) {
					std::cout << "Was not able to find all the desired extensions" << std::endl;
					return false;
				}
			}

			VkApplicationInfo appInfo = VkApplicationInfo();
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pNext = nullptr;
			appInfo.pApplicationName = applicationName;
			appInfo.applicationVersion = VK_MAKE_VERSION(applicationVersion.major, applicationVersion.minor, applicationVersion.patch);
			appInfo.pEngineName = engineName;
			appInfo.engineVersion = VK_MAKE_VERSION(engineVersion.major, engineVersion.minor, engineVersion.patch);
			appInfo.apiVersion = VK_VERSION_1_0;

			VkInstanceCreateInfo instanceInfo = VkInstanceCreateInfo();
			instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceInfo.pNext = nullptr;
			instanceInfo.flags = 0;
			instanceInfo.pApplicationInfo = &appInfo;
			instanceInfo.enabledLayerCount = 0;
			instanceInfo.ppEnabledLayerNames = nullptr;
			instanceInfo.enabledExtensionCount = desired_extensions.size();
			instanceInfo.ppEnabledExtensionNames = (desired_extensions.size() > 0)? desired_extensions.data() : nullptr;

			VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
			if (result != VK_SUCCESS || instance == VK_NULL_HANDLE) {
				std::cout << "Was not able to create the instance" << std::endl;
				return false;
			}

			return true;
		}

		bool CheckAvailablePhysicalDevices(const VkInstance& instance, std::vector<VkPhysicalDevice>& devices) {
			uint32_t devicesCount;
			VkResult result = vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);
			
			if (result != VK_SUCCESS || devicesCount == 0) {
				std::cout << "Was not able to enumerate the physical devices" << std::endl;
				return false;
			}

			devices.resize(devicesCount);
			result = vkEnumeratePhysicalDevices(instance, &devicesCount, devices.data());

			if (result != VK_SUCCESS) {
				std::cout << "Was not able to populate the available devices" << std::endl;
				return false;
			}

			return true;
		}

		bool CheckDeviceExtensions(const VkPhysicalDevice& device, std::vector<VkExtensionProperties>& extensions) {
			uint32_t extensionCount;
			VkResult result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
			if (result != VK_SUCCESS || extensionCount == 0) {
				std::cout << "Was not able to get physical device extension count" << std::endl;
				return false;
			}

			extensions.resize(extensionCount);
			result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());
			if (result != VK_SUCCESS || extensionCount == 0) {
				std::cout << "Was not able to enumerate physical devices" << std::endl;
				return false;
			}

			return true;
		}

		void CheckDeviceFeaturesAndProperties(const VkPhysicalDevice& device, VkPhysicalDeviceFeatures& features ,VkPhysicalDeviceProperties& properties) {
			vkGetPhysicalDeviceFeatures(device, &features);
			vkGetPhysicalDeviceProperties(device, &properties);
		}


		bool CheckDeviceQueueFamilies(const VkPhysicalDevice& device, std::vector<VkQueueFamilyProperties>& familyProperties) {
			uint32_t propertyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &propertyCount, nullptr);
			if (propertyCount == 0) {
				std::cout << "Wasn't able to get Queue Family Properties count" << std::endl;
				return false;
			}

			familyProperties.resize(propertyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &propertyCount, familyProperties.data());
			if (propertyCount == 0) {
				std::cout << "Was not able to get the queue family properties" << std::endl;
				return false;
			}

			return true;
		}

		bool GetDeviceQueueFamilyIndex(const VkPhysicalDevice& device, VkQueueFlags flags, uint32_t& index) {
			std::vector<VkQueueFamilyProperties> queueFamilyProperties;
			if (!CheckDeviceQueueFamilies(device, queueFamilyProperties)) {
				return false;
			}

			for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
				if (queueFamilyProperties[i].queueCount > 0 && (queueFamilyProperties[i].queueFlags & flags)) {
					index = i;
					return true;
				}
			}

			return false;
		}

		bool CreateLogicalDevice(const VkPhysicalDevice& device, 
			std::vector<const char*> desiredExtensions, 
			VkPhysicalDeviceFeatures* desiredFeatures, 
			const std::vector<QueueInfo>& queueInfos, 
			VkDevice& logicalDevice) {

			std::vector<VkExtensionProperties> availableDeviceExtensions;
			if (!CheckDeviceExtensions(device, availableDeviceExtensions)) {
				return false;
			}

			for (const char* str : desiredExtensions) {
				if (!IsExtensionSupported(availableDeviceExtensions, str)) {
					return false;
				}
			}

			std::vector<VkDeviceQueueCreateInfo> queueCreateInfo;

			for (auto& queueInfo : queueInfos) {
				queueCreateInfo.push_back({
					VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					nullptr,
					0,
					queueInfo.familyIndex,
					queueInfo.priorities.size(),
					queueInfo.priorities.data()
				});
			}

			VkDeviceCreateInfo deviceCreateInfo;
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.pNext = nullptr;
			deviceCreateInfo.flags = 0;
			deviceCreateInfo.queueCreateInfoCount = queueCreateInfo.size();
			deviceCreateInfo.pQueueCreateInfos = (queueCreateInfo.size() > 0)? queueCreateInfo.data() : nullptr;
			deviceCreateInfo.enabledLayerCount = 0;
			deviceCreateInfo.ppEnabledLayerNames = nullptr;
			deviceCreateInfo.enabledExtensionCount = desiredExtensions.size();
			deviceCreateInfo.ppEnabledExtensionNames = (desiredExtensions.size() > 0)? desiredExtensions.data() : nullptr;
			deviceCreateInfo.pEnabledFeatures = desiredFeatures;

			VkResult result = vkCreateDevice(device, &deviceCreateInfo, nullptr, &logicalDevice);
			if (result != VK_SUCCESS) {
				std::cout << "Could not create logical device" << std::endl;
				return false;
			}

			return true;
		}

		bool GetDeviceQueue(const VkDevice& device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue& queue) {
			vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &queue);

		}
	}
}


#endif // !COMMON_H


