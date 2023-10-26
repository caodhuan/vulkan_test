#include "ApplicationBase.h"

#include <iostream>

using namespace std;

ApplicationBase::ApplicationBase(const std::string &title, int width,
                                 int height)
    : width(width),
      height(height),
      framebufferResized(false),
      instance(nullptr),
      physicalDevice(nullptr),
      device(nullptr),
      graphicsQueue(nullptr) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

ApplicationBase::~ApplicationBase() {
  if (device) {
    vkDestroyDevice(device, nullptr);
    device = nullptr;
  }

  if (surface) {
    vkDestroySurfaceKHR(instance, surface, nullptr);
  }

  if (instance) {
    vkDestroyInstance(instance, nullptr);
    physicalDevice = nullptr;
    instance = nullptr;
  }
  glfwDestroyWindow(window);
  glfwTerminate();
}

void ApplicationBase::framebufferResizeCallback(GLFWwindow *window, int width,
                                                int height) {
  auto app =
      reinterpret_cast<ApplicationBase *>(glfwGetWindowUserPointer(window));
  app->framebufferResized = true;
}

bool ApplicationBase::InitApplication() {
  if (enableValidationLayers && !checkValidationLayerSupport()) {
    return false;
  }

  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> requiredExtensions;
  for (uint32_t i = 0; i < glfwExtensionCount; i++) {
    requiredExtensions.emplace_back(glfwExtensions[i]);
  }
  requiredExtensions.emplace_back(
      VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  requiredExtensions.emplace_back("VK_KHR_get_physical_device_properties2");

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Hello Triangle";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();
  if (enableValidationLayers) {  // debug 信息
    createInfo.enabledLayerCount = validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
  if (result != VK_SUCCESS) {
    return false;
  }
  if (!pickPhysicalDevice()) {
    return false;
  }

  if (!createLogicalDevice()) {
    return false;
  }

  if (glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
      VK_SUCCESS) {
    return false;
  }
}

bool ApplicationBase::checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);

  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : validationLayers) {
    bool layerFound = false;
    for (const auto &layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
}

bool ApplicationBase::pickPhysicalDevice() {
  unsigned deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  std::cout << "total dievice count = " << deviceCount << endl;
  if (deviceCount == 0) {
    // there is no device
    return false;
  }
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
  for (const auto &device : devices) {
    QueueFamilyIndices indices = findQueueFamilies(device);

    if (indices.isComplete()) {
      physicalDevice = device;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    return false;
  }

  return true;
}

QueueFamilyIndices ApplicationBase::findQueueFamilies(VkPhysicalDevice device) {
  unsigned queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  cout << "queue family count = " << queueFamilyCount << endl;

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());
  QueueFamilyIndices indices;

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    cout << "queue count = " << queueFamily.queueCount
         << ",\tqueue flags = " << queueFamily.queueFlags << endl;
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }
    ++i;
  }

  // Logic to find queue family indices to populate struct with
  return indices;
}

bool ApplicationBase::createLogicalDevice() {
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex =
      indices.graphicsFamily.value();  // chose the queue which support graphics
  queueCreateInfo.queueCount = 1;
  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures{};
  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;

  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = 0;

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
      VK_SUCCESS) {
    return false;
  }

  // init graphics queue
  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);

  return true;
}