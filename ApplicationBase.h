#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <algorithm>  // Necessary for std::clamp
#include <cstdint>    // Necessary for uint32_t
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <limits>  // Necessary for std::numeric_limits
#include <optional>
#include <string>
#include <vector>
struct QueueFamilyIndices {
  std::optional<unsigned> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

// 对 vulkan 使用的封装
class ApplicationBase {
 public:
  ApplicationBase();
  virtual ~ApplicationBase();

 public:
  static std::vector<char> readFile(const std::string &filename);

 public:
  bool InitApplication(const std::string &vertShaderPath,
                       const std::string &fragShaderPath,
                       const std::string &title, int width = 600,
                       int height = 800);
  void Run();

 private:
  bool checkValidationLayerSupport();

  //  init physical device
  bool pickPhysicalDevice();

  // init logical device
  bool createLogicalDevice();

  // init swap chain
  bool createSwapChain();

  // init image view
  bool createImageViews();

  // create one render pass
  bool createRenderPass();

  // init graphics pipline
  bool createGraphicsPipeline(const std::string &vertShaderPath,
                              const std::string &fragShaderPath);
  // create frame buffer for earch image view
  bool createFramebuffers();
  // init command pool
  bool createCommadPool();
  // init a command buffer
  bool createCommandBuffer();

  bool createSyncObjects();

  VkShaderModule createShaderModule(const std::vector<char> &code);

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  // check the device swap chain support
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  // draw
  void drawFrame();

  // swap chain
  void recreateSwapChain();

  void cleanupSwapChain();

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

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
  const std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};

  VkInstance instance;
  VkSurfaceKHR surface;             // represent the windows
  VkPhysicalDevice physicalDevice;  // represent one physical graphics card
  VkDevice device;                  // represent a logical device
  VkQueue graphicsQueue;            // represent a command queue
  VkQueue presentQueue;
  VkSwapchainKHR swapChain;  //
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;
  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;  // pipeline layout creation
  VkPipeline graphicsPipeline;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  VkCommandPool commandPool;
  VkCommandBuffer commandBuffer;

  // synchronization
  VkSemaphore imageAvailableSemaphore;
  VkSemaphore renderFinishedSemaphore;
  VkFence inFlightFence;
};