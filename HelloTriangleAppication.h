
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3>
    getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3>
            attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }
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
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createDepthResources();
    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();
    void loadModel();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();

private:
    void drawFrame();

    void updateUniformBuffer(int currentImage);

private:
    void recreateSwapChain();
    void cleanupSwapChain();
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void recordCommandBuffer(VkCommandBuffer commandBuffer,
                             uint32_t imageIndex);
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
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void createImage(int width, int height, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties, VkImage &image,
                     VkDeviceMemory &imageMemory);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                           uint32_t height);
    void transitionImageLayout(VkImage image, VkFormat format,
                               VkImageLayout oldLayout,
                               VkImageLayout newLayout);

    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                                 VkImageTiling tiling,
                                 VkFormatFeatureFlags features);

    VkFormat findDepthFormat();

    bool hasStencilComponent(VkFormat format);

    VkImageView createImageView(VkImage image, VkFormat format,
                                VkImageAspectFlags aspectFlags);

private:
    bool checkValidationLayerSupport();

private:
    const int WIDTH{800};
    const int HEIGHT{600};
    const std::string MODEL_PATH = "models/viking_room.obj";
    const std::string TEXTURE_PATH = "textures/viking_room.png";

    bool framebufferResized = false;
    GLFWwindow *window;

    VkInstance instance;

    VkCommandPool commandPool;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VkImageView textureImageView;
    VkSampler textureSampler;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    uint32_t currentFrame = 0;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void *> uniformBuffersMapped;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    VkSwapchainKHR swapChain;

    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    VkRenderPass renderPass;
    VkPipeline graphicsPipeline;
    VkDescriptorSetLayout descriptorSetLayout;
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