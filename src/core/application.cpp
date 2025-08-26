#include "application.h"
#include "window.h"

#include "engine/device.h"
#include "engine/command_queue.h"
#include "engine/swapchain.h"
#include "engine/mesh.h"
#include "engine/buffer/constant.h"
#include "engine/shader.h"
#include "engine/pipeline.h"
#include "engine/scene/camera.h"

#include "utils/events.h"
#include "utils/frame_timer.h"

Application::Application(
    HINSTANCE hInstance, 
    WindowConfig &config
) :  
    config(config) 
{
    window = std::make_unique<Window>(hInstance, config, this);

    init();
}

Application::~Application() {
    cleanUp();
}

void Application::init() {
    LOG_INFO(L"Application -> Initializing...");

    scissorRect = CD3DX12_RECT(
        0, 
        0, 
        static_cast<LONG>(config.width), 
        static_cast<LONG>(config.height)
    );

    viewport = CD3DX12_VIEWPORT(
        0.0f, 
        0.0f, 
        static_cast<float>(config.width), 
        static_cast<float>(config.height)
    );

    device = std::make_unique<Device>(config.useWarp);
    LOG_INFO(L"Application -> device initialized!");

    directCommandQueue = std::make_unique<CommandQueue>(
        device->getDevice(),
        D3D12_COMMAND_LIST_TYPE_DIRECT
    );
    LOG_INFO(L"Application -> directCommandQueue initialized!");

    // computeCommandQueue = std::make_unique<CommandQueue>(
    //     device->getDevice(),
    //     D3D12_COMMAND_LIST_TYPE_COMPUTE
    // );
    // LOG_INFO(L"Engine->DirectX 12 computeCommandQueue initialized.");

    // copyCommandQueue = std::make_unique<CommandQueue>(
    //     device->getDevice(),
    //     D3D12_COMMAND_LIST_TYPE_COPY
    // );
    // LOG_INFO(L"Engine->DirectX 12 copyCommandQueue initialized.");

    swapchain = std::make_unique<Swapchain>(
        window->getHwnd(),
        device->getDevice(),
        directCommandQueue->getCommandQueue(),
        config.width,
        config.height,
        FRAMEBUFFERCOUNT,
        device->getSupportTearingState()
    );
    LOG_INFO(L"Application -> swapchain initialized!");

    currentBackBufferIndex = swapchain->getSwapchain()->GetCurrentBackBufferIndex();

    LOG_INFO(L"Application Class initialized!");
    LOG_INFO(L"-- Resources --");

    std::vector<VertexStruct> vertices = {
        { XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) }, // 0
        { XMFLOAT4(-1.0f,  1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }, // 1
        { XMFLOAT4( 1.0f,  1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) }, // 2
        { XMFLOAT4( 1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // 3
        { XMFLOAT4(-1.0f, -1.0f,  1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, // 4
        { XMFLOAT4(-1.0f,  1.0f,  1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }, // 5
        { XMFLOAT4( 1.0f,  1.0f,  1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }, // 6
        { XMFLOAT4( 1.0f, -1.0f,  1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) }  // 7
    };

    // 36 indices for cube (12 triangles)
    std::vector<uint32_t> indices =
    {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        4, 5, 1, 4, 1, 0,
        3, 2, 6, 3, 6, 7,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7
    };

    // create buffers
    mesh = std::make_unique<Mesh>(
        device->getDevice(),
        vertices,
        indices
    );
    LOG_INFO(L"Mesh Resource initialized!");

    // mvpBuffer?
    constantBuffer1 = std::make_unique<ConstantBuffer>(
        device->getDevice(),
        static_cast<UINT>(sizeof(ConstantMVP))
    );
    LOG_INFO(L"ConstantBuffer1 Resource initialized!");

    camera1 = std::make_unique<Camera>(
        45.0f,
        static_cast<float>(config.width) / static_cast<float>(config.height),
        0.1f,
        100.0f
    );
    LOG_INFO(L"Camera initialized!");

    XMMATRIX view = camera1->getViewMatrix();
    XMMATRIX proj = camera1->getProjectionMatrix();
    LOG_INFO(L"Camera View matrix[0][0]: %f", view.r[0].m128_f32[0]);
    LOG_INFO(L"Camera Projection matrix[0][0]: %f", proj.r[0].m128_f32[0]);

    // pipeline
    // Root parameter for CBV
    // CD3DX12_ROOT_PARAMETER cbvRootParam;
    // cbvRootParam.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

    CD3DX12_ROOT_PARAMETER cbvRootParam;
    cbvRootParam.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    std::vector<D3D12_ROOT_PARAMETER> rootParams = { cbvRootParam };

    auto vertexShader = Shader(L"assets/shaders/vertex.cso");
    auto pixelShader = Shader(L"assets/shaders/pixel.cso");

    std::vector<D3D12_STATIC_SAMPLER_DESC> samplers{};

    pipeline1 = std::make_unique<Pipeline>(
        device->getDevice(),
        vertexShader,
        pixelShader,
        inputLayout,
        rootParams,
        samplers,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D24_UNORM_S8_UINT
    );
}

int Application::run() {
    MSG msg = {};
    Timer timer;

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            timer.tick();

            UpdateEventArgs updateArgs(
                timer.getDeltaSeconds(), 
                timer.getTotalSeconds()
            );
            onUpdate(updateArgs);

            RenderEventArgs renderArgs(
                timer.getDeltaSeconds(), 
                timer.getTotalSeconds()
            );
            onRender(renderArgs);
            
            std::wstring title = std::wstring(config.appName) + L" - " + timer.getFPSString();
            if (window) {
                SetWindowTextW(window->getHwnd(), title.c_str());
            } else {
                LOG_ERROR(L"Window is null!");
            }
        }
    }

    return static_cast<int>(msg.wParam);
}

void Application::onUpdate(UpdateEventArgs& args)
{
    camera1->update(static_cast<float>(args.totalTime));

    // Rotate the cube over time
    XMMATRIX model = XMMatrixIdentity();
    XMMATRIX view = camera1->getViewMatrix();
    XMMATRIX projection = camera1->getProjectionMatrix();

    // Update Constant Buffer In GPU
    ConstantMVP mvpData;
    mvpData.mvp = XMMatrixTranspose(model * view * projection);
    constantBuffer1->update(&mvpData, sizeof(mvpData)); // Upload to GPU
}

void Application::onRender(RenderEventArgs& args)
{
    // Get a command list from the queue; allocator is managed internally
    auto commandList = directCommandQueue->getCommandList();
    auto commandQueue = directCommandQueue->getCommandQueue();

    auto pipelineState = pipeline1->getPipelineState();
    auto rootSignature = pipeline1->getRootSignature();
    auto rtvHeap = swapchain->getRTVHeap();
    auto dsvHeap = swapchain->getDSVHeap();
    auto vertex = mesh->getVertex();
    auto index = mesh->getIndex();
    auto vsync = device->getSupportTearingState();

    // Reset command list with current pipeline
    commandList->SetPipelineState(pipelineState.Get());
    commandList->SetGraphicsRootSignature(rootSignature.Get());

    // Set constant buffer (MVP updated in onUpdate)
    commandList->SetGraphicsRootConstantBufferView(0, constantBuffer1->getGPUAddress());

    // Set viewport and scissor
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    // Transition back buffer to render target
    auto backBuffer = swapchain->getBackBuffer(currentBackBufferIndex);
    transitionResource(commandList, backBuffer.Get(),
                       D3D12_RESOURCE_STATE_PRESENT,
                       D3D12_RESOURCE_STATE_RENDER_TARGET);

    // Set render target and depth-stencil
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->getHeap()->GetCPUDescriptorHandleForHeapStart(),
                                            currentBackBufferIndex, rtvHeap->getDescriptorSize());
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsvHeap->getHeap()->GetCPUDescriptorHandleForHeapStart());
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // Clear render target and depth-stencil
    const float clearColor[] = {0.1f, 0.1f, 0.1f, 1.0f};
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // Draw the cube
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    auto vbView = vertex->getView();
    auto ibView = index->getView();
    commandList->IASetVertexBuffers(0, 1, &vbView);
    commandList->IASetIndexBuffer(&ibView);
    commandList->DrawIndexedInstanced(index->getCount(), 1, 0, 0, 0);

    // Transition back buffer to present
    transitionResource(commandList, backBuffer.Get(),
                       D3D12_RESOURCE_STATE_RENDER_TARGET,
                       D3D12_RESOURCE_STATE_PRESENT);

    // Execute command list
    fenceValues[currentBackBufferIndex] = directCommandQueue->executeCommandList(commandList);

    // Present
    INT syncInterval = vsync ? 1 : 0;
    UINT presentFlags = !vsync ? DXGI_PRESENT_ALLOW_TEARING : 0;
    throwFailed(swapchain->getSwapchain()->Present(syncInterval, presentFlags));

    currentBackBufferIndex = swapchain->getSwapchain()->GetCurrentBackBufferIndex();

    // Wait for GPU to finish frame
    directCommandQueue->fenceWait(fenceValues[currentBackBufferIndex]);
}

void Application::transitionResource(
    ComPtr<ID3D12GraphicsCommandList2> commandList,
    ComPtr<ID3D12Resource> resource,
    D3D12_RESOURCE_STATES beforeState,
    D3D12_RESOURCE_STATES afterState
) {
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        resource.Get(),
        beforeState,
        afterState
    );

    commandList->ResourceBarrier(1, &barrier);
}

void Application::onResize(ResizeEventArgs& args)
{
    if (!device || !swapchain)
        return;

    // Skip if minimized
    if (args.width == 0 || args.height == 0)
    {
        LOG_INFO(L"Resize skipped (window minimized: %dx%d)", args.width, args.height);
        return;
    }

    // Only resize if size actually changed
    if (config.width != args.width || config.height != args.height) {
        config.width = std::max(1u, static_cast<unsigned int>(args.width));
        config.height = std::max(1u, static_cast<unsigned int>(args.height));

        // Wait for GPU to finish any in-flight commands
        directCommandQueue->flush();

        // Resize swap chain buffers
        swapchain->resize(config.width, config.height);

        // Reset back buffer index after resize
        currentBackBufferIndex = swapchain->getSwapchain()->GetCurrentBackBufferIndex();

        // Update camera projection
        camera1->setProjection(
            45.0f,
            float(config.width) / float(config.height),
            0.1f,
            100.0f
        );

        // Update viewport + scissor rect
        viewport = { 0.0f, 0.0f, float(config.width), float(config.height), 0.0f, 1.0f };
        scissorRect = { 0, 0, static_cast<LONG>(config.width), static_cast<LONG>(config.height) };

        LOG_INFO(L"Application resized to %dx%d", config.width, config.height);
    }
}

void Application::onMouseWheel(MouseWheelEventArgs& args)
{
    // the zoom could be smoother in my opinion but it'd evolve over time...
    if (args.control) {
        // Ctrl + wheel → lens zoom (FOV)
        camera1->setFov(camera1->getFov() - args.wheelDelta * 0.05f);
    } else {
        // Normal wheel → dolly zoom (radius)
        camera1->zoom(args.wheelDelta * 0.25f);
    }
}

void Application::onMouseMoved(MouseMotionEventArgs& args) {
    if (args.leftButton)
    {
        camera1->orbit(args.relX * 0.01f, args.relY * 0.01f);
    }
}

void Application::cleanUp() {
    LOG_INFO(L"Application cleanup started.");

    if (directCommandQueue) {
        LOG_INFO(L"Flushing GPU commands before releasing resources...");
        directCommandQueue->flush(); // ensure GPU has finished all work
    }

    // Reset resources in reverse creation order
    if (mesh) {
        mesh.reset();
        LOG_INFO(L"Mesh released.");
    }

    if (constantBuffer1) {
        constantBuffer1.reset();
        LOG_INFO(L"Constant buffer released.");
    }

    if (pipeline1) {
        pipeline1.reset();
        LOG_INFO(L"Pipeline released.");
    }

    if (camera1) {
        camera1.reset();
        LOG_INFO(L"Camera released.");
    }

    if (swapchain) {
        // Wait for GPU to finish using back buffers before releasing
        directCommandQueue->flush();

        swapchain.reset();
        LOG_INFO(L"Swapchain released.");
    }

    if (directCommandQueue) {
        directCommandQueue.reset();
        LOG_INFO(L"Command queue released.");
    }

    if (device) {
        device.reset();
        LOG_INFO(L"Device released.");
    }

    if (window) {
        window.reset();
        LOG_INFO(L"Window released.");
    }

    LOG_INFO(L"Application cleanup finished.");
}