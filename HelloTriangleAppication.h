
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

const int MAX_FRAMES_IN_FLIGHT = 2;

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
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

class HelloTriangleApplication {
 public:
  HelloTriangleApplication();
  ~HelloTriangleApplication();

 public:
  void run();

 public:
  static std::vector<char> readFile(const std::string &filename);
  static void framebufferResizeCallback(GLFWwindow *window, int width,
                                        int height);

 private:
  void initWindow();
  void initVulkan();
  void mainLoop();
  void cleanup();

 private:
  void createInstance();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapChain();
  void createImageViews();
  void createRenderPass();
  void createGraphicsPipeline();
  void createFramebuffers();
  void createCommandPool();
  void createVertexBuffer();
  void createCommandBuffers();
  void createSyncObjects();

 private:
  void drawFrame();

 private:
  void recreateSwapChain();
  void cleanupSwapChain();
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  VkShaderModule createShaderModule(const std::vector<char> &code);

  bool isDeviceSuitable(VkPhysicalDevice device);
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);

 private:
  bool checkValidationLayerSupport();

 private:
  const int WIDTH{800};
  const int HEIGHT{600};
  bool framebufferResized = false;
  GLFWwindow *window;

  VkInstance instance;

  VkCommandPool commandPool;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  uint32_t currentFrame = 0;
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;

  std::vector<VkCommandBuffer> commandBuffers;
  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;

  VkSwapchainKHR swapChain;

  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  VkRenderPass renderPass;
  VkPipeline graphicsPipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;
  VkDevice device;

  VkQueue graphicsQueue;

  VkQueue presentQueue;

  VkSurfaceKHR surface;

  const std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};

  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};
#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif
};