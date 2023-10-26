#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <optional>
#include <string>
#include <vector>

struct QueueFamilyIndices {
  std::optional<unsigned> graphicsFamily;

  bool isComplete() { return graphicsFamily.has_value(); }
};

// 对 vulkan 使用的封装
class ApplicationBase {
 public:
  ApplicationBase(const std::string &title, int width = 600, int height = 800);
  virtual ~ApplicationBase();

 public:
  bool InitApplication();

 private:
  bool checkValidationLayerSupport();

  //  init physical device
  bool pickPhysicalDevice();

  // init logical device
  bool createLogicalDevice();

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

 private:
  static void framebufferResizeCallback(GLFWwindow *window, int width,
                                        int height);

 private:
  int width, height;
  GLFWwindow *window;
  bool framebufferResized;

  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif

  VkInstance instance;
  VkPhysicalDevice physicalDevice;  // represent one physical graphics card
  VkDevice device;                  // represent a logical device
  VkQueue graphicsQueue;            // represent a command queue
  VkSurfaceKHR surface;             // represent the windows
};