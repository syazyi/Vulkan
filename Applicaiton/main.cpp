#include <fstream>
#include <iostream>
#include "window/include/window.h"
#include "vulkanInstance/include/vkInstance.h"
#include "PhysicalDevice/physicaldevice.h"
#include "LogicDevice/logicdevice.h"
#include "WindowSurface/windowsurface.h"
#include "SwapChain/swapchain.h"
#include "ImageView/imageview.h"

#include "shaderc/shaderc.hpp"
#include "GraphicPipeline/graphicpipeline.h"
#include "CommandBuffer/commandbuffer.h"
#include "framework/app.h"
#include "framework/Vertex/vertex.h"
#include "framework/Uniform/uniform.h"
#include "framework/Descriptor/descriptor.h"
#include "framework/Image/image.h"

std::vector<uint32_t> compile_file(const std::string& source_name,
                                   shaderc_shader_kind kind,
                                   const std::string& source,
                                   bool optimize = false) {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  // Like -DMY_DEFINE=1
  options.AddMacroDefinition("MY_DEFINE", "1");
  if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

  shaderc::SpvCompilationResult module =
      compiler.CompileGlslToSpv(source, kind, source_name.c_str(), options);

  if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::cerr << module.GetErrorMessage();
        throw std::runtime_error("");
  }

  return {module.cbegin(), module.cend()};
}

std::string readFile(const char* file_path) {
    std::ifstream file(file_path, std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("failed to read file ,please check your path");
    }
    auto size = (size_t)file.tellg();
    file.seekg(0);

    std::vector<char> buffer(size);
    file.read(buffer.data(), size);
    file.close();
    return buffer.data();
}


int main(int argc, char** argv){
    kvs::Window glfw_window(1600, 900);
    
    kvs::KInstance instance;
    
    kvs::WindowSurface window_surface(instance, glfw_window);

    //set device
    kvs::PhysicalDevice::AddPhysicalDeviceExtensionSupport(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    kvs::PhysicalDevice physical_device(instance, window_surface);

    kvs::LogicDevice logic_device(physical_device);
    
    //set swapchain
    kvs::SwapChain swap_chain(logic_device, physical_device, window_surface);
    swap_chain.Create();

    kvs::ImageView image_view(logic_device, swap_chain);
    image_view.Create();

    //set vertex info
    auto vert_shader_string = readFile("../../ShaderSrc/tutorialShader/vert.vert");
    auto frag_shader_string = readFile("../../ShaderSrc/tutorialShader/frag.frag");
    auto vertexShader = compile_file("main", shaderc_shader_kind::shaderc_vertex_shader, vert_shader_string);
    auto fragmentShader = compile_file("main", shaderc_shader_kind::shaderc_fragment_shader, frag_shader_string);
    
    std::vector<kvs::VertexInfo> vertices = {
        {{0.5f, -0.5f}, {1.0f, 1.0f, 1.0f},{1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f},{0.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},{0.0f, 1.0f}},
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}
    };
    std::vector<kvs::VertexIndexInfo> indices = {
        0, 1, 2, 0, 2, 3
    };
    kvs::Vertex test_vertex(vertices, indices);

    //load texture
    kvs::Texture nahida("../../Asset/Texture/NahidaClip.png");

    //set command
    kvs::Command commandSystem(logic_device);
    commandSystem.CreateCommand(physical_device.m_hasFindQueueFamily);
    //commandSystem.RecordDrawCommand(0, pipeline, swap_chain, vertex_buffer);


    //set vertex buffer and texture image
    kvs::VertexBuffer vertex_buffer(logic_device, physical_device, test_vertex);
    vertex_buffer.AllocateVertexBuffer(commandSystem, logic_device.m_GraphicsQueue);
    vertex_buffer.AllocateIndexBuffer(commandSystem, logic_device.m_GraphicsQueue);

    kvs::TextureImage texture_image(logic_device);
    texture_image.CreateTextureImage(physical_device, commandSystem, nahida);
    texture_image.CreateImageView(VK_FORMAT_R8G8B8A8_SRGB);
    texture_image.CreateSampler(physical_device);

    //set unifrom
    kvs::Uniform uniform(logic_device);
    uniform.CreateUniformBuffer(physical_device.GetPhysicalDevice());
    
    kvs::Descriptor descroptor(logic_device);
    descroptor.CreateSetLayout(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
    descroptor.CreateDescriptor(uniform, texture_image);

    //set pipeline
    kvs::GraphicPipeline pipeline(logic_device, swap_chain, vertexShader, fragmentShader);
    pipeline.CreatePipeline(vertex_buffer, descroptor);
    pipeline.CreateFrameBuffer(image_view.m_imageViews, pipeline.RequestVkRect2D());


    kvs::App app(logic_device, commandSystem);
    app.CreateSyncObject();


    //command record , submit command
    while (!glfwWindowShouldClose(glfw_window.GetWindow())) {
        glfwPollEvents();
        app.DrawFrame(pipeline, swap_chain, image_view, vertex_buffer, uniform, descroptor);
    }
    vkDeviceWaitIdle(logic_device.GetLogicDevice());

    //clean up
    swap_chain.CleanUpSwapChain(image_view, pipeline);

    texture_image.CleanUp();
    descroptor.CleanUpDescriptor();
    uniform.CleanUpUniform();
    vertex_buffer.FreeIndexBuffer();
    vertex_buffer.FreeVertexBuffer();

    app.DestroySyncObject();
    commandSystem.DestroyCommand();

    pipeline.DestroyPipeline();


}