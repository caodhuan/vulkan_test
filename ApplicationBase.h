#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
// 对 vulkan 使用的封装
class ApplicationBase {
 public:
  ApplicationBase(const std::string &title, int width = 600, int height = 800);
  virtual ~ApplicationBase();

 public:
  bool InitApplication();

 private:
  bool checkValidationLayerSupport();

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
};