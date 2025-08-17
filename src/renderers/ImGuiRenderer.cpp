#include "ImGuiRenderer.h"

ImGuiRenderer::ImGuiRenderer(const VulkanContext &vulkanContext)
{
    device = vulkanContext.getDevice();

    // Yoinked directly from the ImGui samples
    VkDescriptorPoolSize poolSizes[] = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
    };

    VkDescriptorPoolCreateInfo poolInfo{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, .pNext = nullptr};
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 0;
    for (VkDescriptorPoolSize &poolSize : poolSizes)
        poolInfo.maxSets += poolSize.descriptorCount;
    poolInfo.poolSizeCount = (uint32_t)std::size(poolSizes);
    poolInfo.pPoolSizes = poolSizes;

    auto result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &imguiPool);
    VulkanUtils::CheckVkResult(result);

    ImGui_ImplVulkan_InitInfo imguiVkInitInfo{};
    imguiVkInitInfo.ApiVersion = VK_API_VERSION_1_3;
    imguiVkInitInfo.Instance = vulkanContext.getInstance();
    imguiVkInitInfo.PhysicalDevice = vulkanContext.getPhysicalDevice();
    imguiVkInitInfo.Device = device;
    imguiVkInitInfo.QueueFamily = vulkanContext.getQueueFamily(VulkanContext::QueueType::Graphics);
    imguiVkInitInfo.Queue = vulkanContext.getQueue(VulkanContext::QueueType::Graphics);

    // don't have one, and it's literally null in the example too
    // imguiVkInitInfo.PipelineCache = g_PipelineCache;
    imguiVkInitInfo.DescriptorPool = imguiPool;
    // we're using dynamic rendering, no renderpass here
    // imguiVkInitInfo.RenderPass = wd->RenderPass;
    // imguiVkInitInfo.Subpass = 0;
    imguiVkInitInfo.UseDynamicRendering = true;
    imguiVkInitInfo.PipelineRenderingCreateInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
    imguiVkInitInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    imguiVkInitInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &vulkanContext.getSwapchain().imageFormat;

    imguiVkInitInfo.MinImageCount = 3;
    imguiVkInitInfo.ImageCount = 3;
    imguiVkInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    // don't have these
    // imguiVkInitInfo.Allocator = g_Allocator;
    // imguiVkInitInfo.CheckVkResultFn = check_vk_result;

    ImGui_ImplVulkan_Init(&imguiVkInitInfo);
    // no longer needed, a default font is magically provided now
    // ImGui_ImplVulkan_CreateFontsTexture();
}

ImGuiRenderer::~ImGuiRenderer()
{
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(device, imguiPool, nullptr);
}

void ImGuiRenderer::setup(ImageResource finalTarget)
{
    IRenderer::setup(finalTarget);

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();
    ImGui::Render();
}

void ImGuiRenderer::execute(VkCommandBuffer cmd)
{
    VkRenderingAttachmentInfo colorAttachment{.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO, .pNext = nullptr};

    colorAttachment.clearValue = {};
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.imageView = finalTarget.imageView;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo renderingInfo{.sType = VK_STRUCTURE_TYPE_RENDERING_INFO, .pNext = nullptr};
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;
    renderingInfo.pDepthAttachment = nullptr;
    renderingInfo.pStencilAttachment = nullptr;
    renderingInfo.layerCount = 1;
    renderingInfo.renderArea = VkRect2D{VkOffset2D{0, 0}, {finalTarget.imageExtent.width, finalTarget.imageExtent.height}};

    vkCmdBeginRendering(cmd, &renderingInfo);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

    vkCmdEndRendering(cmd);
}
