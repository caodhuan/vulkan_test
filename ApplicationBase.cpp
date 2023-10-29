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
      graphicsQueue(nullptr),
      presentQueue(nullptr),
      swapChain(nullptr) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

ApplicationBase::~ApplicationBase() {
  for (auto imageView : swapChainImageViews) {
    vkDestroyImageView(device, imageView, nullptr);
  }
  swapChainImageViews.clear();

  if (swapChain) {
    vkDestroySwapchainKHR(device, swapChain, nullptr);
    swapChain = nullptr;
  }

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

  if (glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
      VK_SUCCESS) {
    return false;
  }

  if (!pickPhysicalDevice()) {
    return false;
  }

  if (!createLogicalDevice()) {
    return false;
  }

  if (!createSwapChain()) {
    return false;
  }
  if (!createImageViews()) {
    return false;
  }
  if (!createGraphicsPipeline()) {
    return false;
  }

  return true;
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
      break;
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
    bool swapChainAdequate = false;

    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
      if (presentSupport) {
        indices.presentFamily = i;
      }
      SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);

      swapChainAdequate = !swapChainSupport.formats.empty() &&
                          !swapChainSupport.presentModes.empty();
    }

    if (indices.isComplete() && swapChainAdequate) {
      break;
    }
    ++i;
  }

  // Logic to find queue family indices to populate struct with
  return indices;
}

SwapChainSupportDetails ApplicationBase::querySwapChainSupport(
    VkPhysicalDevice device) {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.capabilities);
  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
  if (formatCount) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                                            nullptr);
  if (presentModeCount) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &presentModeCount, details.presentModes.data());
  }
  return details;
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
  createInfo.enabledExtensionCount = deviceExtensions.size();
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
      VK_SUCCESS) {
    return false;
  }

  // init graphics queue
  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
  vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
  return true;
}

bool ApplicationBase::createSwapChain() {
  SwapChainSupportDetails swapChainSupport =
      querySwapChainSupport(physicalDevice);
  VkSurfaceFormatKHR surfaceFormat;
  bool hasValue = false;
  for (const auto &availableFormat : swapChainSupport.formats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      surfaceFormat = availableFormat;
      hasValue = false;
      break;
    }
  }
  if (!hasValue) {
    surfaceFormat = swapChainSupport.formats[0];
  }

  hasValue = false;
  VkPresentModeKHR presentMode;
  for (const auto &availablePresentMode : swapChainSupport.presentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      presentMode = availablePresentMode;
      hasValue = true;
      break;
    }
  }
  if (!hasValue) {
    presentMode = VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D extent;
  if (swapChainSupport.capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    std::cout << "capabilities width = "
              << swapChainSupport.capabilities.currentExtent.width
              << "\theight = "
              << swapChainSupport.capabilities.currentExtent.height
              << std::endl;
    extent = swapChainSupport.capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    cout << "frame buffer size width = " << width << "\theight = " << height
         << endl;

    extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    extent.width = std::clamp(
        extent.width, swapChainSupport.capabilities.minImageExtent.width,
        swapChainSupport.capabilities.maxImageExtent.width);
    extent.height = std::clamp(
        extent.height, swapChainSupport.capabilities.minImageExtent.height,
        swapChainSupport.capabilities.maxImageExtent.height);
  }
  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }
  cout << "swap chain min image count = "
       << swapChainSupport.capabilities.minImageCount << endl;
  cout << "swap chain max image count = "
       << swapChainSupport.capabilities.maxImageCount << endl;

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers =
      1;  // this is where i can set up more than 1 image
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) !=
      VK_SUCCESS) {
    return false;
  }
  swapChainImageFormat = surfaceFormat.format;
  swapChainExtent = extent;

  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
  swapChainImages.resize(imageCount);
  cout << "swap chain image cout = " << imageCount << endl;
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount,
                          swapChainImages.data());

  return true;
}

bool ApplicationBase::createImageViews() {
  swapChainImageViews.resize(swapChainImages.size());
  for (size_t i = 0; i < swapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapChainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = swapChainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    if (vkCreateImageView(device, &createInfo, nullptr,
                          &swapChainImageViews[i]) != VK_SUCCESS) {
      return false;
    }
  }

  return true;
}
bool ApplicationBase::createGraphicsPipeline() {}