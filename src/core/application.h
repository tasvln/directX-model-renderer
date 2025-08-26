#pragma once

#include "utils/pch.h"

class Window;
class Device;
class CommandQueue;
class Swapchain;
class Mesh;
class ConstantBuffer;
class Pipeline;
class Camera;

class UpdateEventArgs;
class RenderEventArgs;
class ResizeEventArgs;
class MouseWheelEventArgs;
class MouseMotionEventArgs;

class Application
{
    public:
        Application(HINSTANCE hInstance, WindowConfig &config);
        ~Application();

        // running the application
        int run();

        void onResize(ResizeEventArgs& args);

        // main functions for rendering / callbacks
        void onUpdate(UpdateEventArgs& args);
        void onRender(RenderEventArgs& args);
        void onMouseWheel(MouseWheelEventArgs& args);
        void onMouseMoved(MouseMotionEventArgs& args);

        void transitionResource(
            ComPtr<ID3D12GraphicsCommandList2> commandList,
            ComPtr<ID3D12Resource> resource,
            D3D12_RESOURCE_STATES beforeState,
            D3D12_RESOURCE_STATES afterState
        );

    private:
        void init();
        void cleanUp();

    private:
        HWND hwnd = nullptr;
        WindowConfig config;
        RECT windowRect = {};

        UINT currentBackBufferIndex;
        uint64_t fenceValues[FRAMEBUFFERCOUNT] {};

        D3D12_VIEWPORT viewport;
        D3D12_RECT scissorRect;

        // unique pttrsssssss -> GPU resources
        std::unique_ptr<Window> window;
        std::unique_ptr<Device> device;
        std::unique_ptr<CommandQueue> directCommandQueue;
        // std::unique_ptr<CommandQueue> computeCommandQueue;
        // std::unique_ptr<CommandQueue> copyCommandQueue;
        std::unique_ptr<Swapchain> swapchain;
        std::unique_ptr<Mesh> mesh;
        std::unique_ptr<ConstantBuffer> constantBuffer1;
        std::unique_ptr<Pipeline> pipeline1;
        std::unique_ptr<Camera> camera1;
};