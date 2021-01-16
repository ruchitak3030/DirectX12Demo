#include "HeaderHelper.h"
#include "Game.h"


Game::Game(HINSTANCE hInstance) :
	DXSample(
        hInstance,
        "DirectX Game",
        1280,
        720,
        true
    ),
	m_pCbvDataBegin(nullptr),
	m_constantBufferData{}
{
}

Game::~Game()
{
    delete triangleMesh;
}

void Game::Init()
{
    LoadShaders();
    CreateMatrices();
    CreateBasicGeometry();
    CreateRootSignature();
    CreatePSO();
}

void Game::Update(float deltaTime, float totalTime)
{
    /*const float translationSpeed = 0.005f;
    const float offsetBounds = 1.25f;
    m_constantBufferData.offset.x += translationSpeed;
    if (m_constantBufferData.offset.x > offsetBounds)
    {
        m_constantBufferData.offset.x = -offsetBounds;
    }*/

    //Build MVP and upload to constant Buffer
    XMMATRIX worldViewProj = XMLoadFloat4x4(&m_worldMatrix) * XMLoadFloat4x4(&m_viewMatrix) * XMLoadFloat4x4(&m_projMatrix);

    //Update constant buffer with latest world-view-proj matrix.
    XMStoreFloat4x4(&m_constantBufferData.WorldViewProj, XMMatrixTranspose(worldViewProj));
    memcpy(m_pCbvDataBegin, &m_constantBufferData, sizeof(m_constantBufferData));
}

void Game::Render(float deltaTime, float totalTime)
{
    PopulateCommandList();

    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame
    ThrowIfFailed(m_swapChain->Present(1, 0));

    WaitForPreviousFrame();
}

void Game::Destroy()
{
    WaitForPreviousFrame();
    CloseHandle(m_fenceEvent);
}

void Game::OnResize()
{
    DXSample::OnResize();

    XMMATRIX proj = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,
        m_aspectRatio,
        1.0f,
        1000.0f);

    XMStoreFloat4x4(&m_projMatrix, proj);
}

void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
    m_prevMousePos.x = x;
    m_prevMousePos.y = y;

    SetCapture(m_hWnd);
}

void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
    ReleaseCapture();
}

void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
    m_prevMousePos.x = x;
    m_prevMousePos.y = y;
}

void Game::OnMouseWheel(WPARAM wheelData, int x, int y)
{
}

//void Game::LoadPipeline()
//{
//    UINT dxgiFactoryFlags = 0;
//
//#if defined(_DEBUG)
//    // Enable the debug layer
//    {
//        ComPtr<ID3D12Debug> debugController;
//        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
//        {
//            debugController->EnableDebugLayer();
//
//            // Enable additional debug layers.
//            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
//        }
//    }
//#endif
//
//    ComPtr<IDXGIFactory4> factory;
//    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
//
//    if (m_useWarpDevice)
//    {
//        ComPtr<IDXGIAdapter> warpAdapter;
//        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
//
//        ThrowIfFailed(D3D12CreateDevice(
//            warpAdapter.Get(),
//            D3D_FEATURE_LEVEL_11_0,
//            IID_PPV_ARGS(&m_device)
//        ));
//    }
//    else
//    {
//        ComPtr<IDXGIAdapter1> hardwareAdapter;
//        GetHardwareAdapter(factory.Get(), &hardwareAdapter);
//
//        ThrowIfFailed(D3D12CreateDevice(
//            hardwareAdapter.Get(),
//            D3D_FEATURE_LEVEL_11_0,
//            IID_PPV_ARGS(&m_device)
//        ));
//    }
//
//    // Describe and create the command queue.
//    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
//    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//
//    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
//
//    // Describe and create the swap chain.
//    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
//    swapChainDesc.BufferCount = FrameCount;
//    swapChainDesc.Width = m_width;
//    swapChainDesc.Height = m_height;
//    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//    swapChainDesc.SampleDesc.Count = 1;
//
//    ComPtr<IDXGISwapChain1> swapChain;
//    ThrowIfFailed(factory->CreateSwapChainForHwnd(
//        m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
//        Application::GetHwnd(),
//        &swapChainDesc,
//        nullptr,
//        nullptr,
//        &swapChain
//    ));
//
//    // This sample does not support fullscreen transitions.
//    ThrowIfFailed(factory->MakeWindowAssociation(Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));
//
//    ThrowIfFailed(swapChain.As(&m_swapChain));
//    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
//
//    // Create descriptor heaps.
//    {
//        // Describe and create a render target view (RTV) descriptor heap.
//        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
//        rtvHeapDesc.NumDescriptors = FrameCount;
//        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//        ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
//
//        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//
//        // Describe and create a depth stencil view descriptor heap;
//        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
//        dsvHeapDesc.NumDescriptors = 1;
//        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
//        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//        ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));
//
//        m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
//
//        // Describe and create a constant buffer view (CBV) descriptor heap.
//        D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
//        cbvHeapDesc.NumDescriptors = 1;
//        cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//        cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//        ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap)));
//    }
//
//    // Create frame resources.
//    {
//        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
//
//        // Create a RTV for each frame.
//        for (UINT n = 0; n < FrameCount; n++)
//        {
//            ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
//            m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
//            rtvHandle.Offset(1, m_rtvDescriptorSize);
//        }
//    }
//
//    // Create depth stencil buffer and view
//    {
//
//        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
//
//        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
//        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
//        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
//        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
//
//        D3D12_CLEAR_VALUE optClear = {};
//        optClear.Format = DXGI_FORMAT_D32_FLOAT;
//        optClear.DepthStencil.Depth = 1.0f;
//        optClear.DepthStencil.Stencil = 0;
//
//        ThrowIfFailed(m_device->CreateCommittedResource(
//            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
//            D3D12_HEAP_FLAG_NONE,
//            &CD3DX12_RESOURCE_DESC::Tex2D(
//                DXGI_FORMAT_D32_FLOAT,
//                m_width,
//                m_height,
//                1,
//                1,
//                1,
//                0,
//                D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
//            D3D12_RESOURCE_STATE_DEPTH_WRITE,
//            &optClear,
//            IID_PPV_ARGS(&m_depthStencilBuffer)));
//
//        D3D12_DEPTH_STENCIL_DESC depthDesc = {};
//        depthDesc.DepthEnable = TRUE;
//        depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
//        depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
//        depthDesc.StencilEnable = FALSE;
//
//        m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &dsvDesc, dsvHandle);
//    }
//
//    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
//
//}


void Game::LoadShaders()
{
    // Shader compilation
   

#if defined(_DEBUG)
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ThrowIfFailed(D3DCompileFromFile(
        GetAssetFullPath(L"VertexShader.hlsl").c_str(), 
        nullptr, 
        nullptr, 
        "main",
        "vs_5_0", 
        compileFlags, 
        0, 
        &m_vertexShader, 
        nullptr));

    ThrowIfFailed(D3DCompileFromFile(
        GetAssetFullPath(L"PixelShader.hlsl").c_str(), 
        nullptr, 
        nullptr, 
        "main", 
        "ps_5_0", 
        compileFlags,
        0,
        &m_pixelShader,
        nullptr));
}

void Game::CreateMatrices()
{
    XMMATRIX world = XMMatrixIdentity();
    XMStoreFloat4x4(&m_worldMatrix, world);

    // Build view matrix
    XMVECTOR pos = XMVectorSet(0, 0, -5.0f, -1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&m_viewMatrix, view);

    //Build projection Matrix
   // XMMATRIX proj = XMLoadFloat4x4(&m_projMatrix);
    XMMATRIX proj = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,
        m_aspectRatio,
        1.0f,
        1000.0f);

    XMStoreFloat4x4(&m_projMatrix, proj);
}

void Game::CreateBasicGeometry()
{

    Vertex cubeVertices[] =
    {
        { { 0.0f, 0.25f * m_aspectRatio, 0.0f }, XMFLOAT4(DirectX::Colors::Red) },
        { { 0.25f, -0.25f * m_aspectRatio, 0.0f }, XMFLOAT4(DirectX::Colors::Blue) },
        { { -0.25f, -0.25f * m_aspectRatio, 0.0f }, XMFLOAT4(DirectX::Colors::Green) }
        /*{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(DirectX::Colors::White) },
        { XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Black) },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Red) },
        { XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Green) },
        { XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Blue) },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Yellow) },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Cyan) },
        { XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Magenta) }*/
    };

    int numVertices = sizeof(cubeVertices)/sizeof(cubeVertices[0]);

    int indices[] =
    {
        0, 1, 2
        /*// front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 6, 5,
        4, 7, 6,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7*/
    };

    int numIndices = sizeof(indices)/sizeof(indices[0]);

    triangleMesh = new Mesh(cubeVertices, indices, numVertices, numIndices, m_device);
    triangleIndexCount = triangleMesh->m_indexCount;
   
    // Create constant buffer view
    {
        ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_constantBuffer)));

        // Describe and create constant buffer view
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = (sizeof(SceneConstantBuffer) + 255) & ~255;
        m_device->CreateConstantBufferView(&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

        // Map and initialize the constant buffer
        CD3DX12_RANGE readRange(0, 0);
        ThrowIfFailed(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegin)));
        memcpy(m_pCbvDataBegin, &m_constantBufferData, sizeof(m_constantBufferData));
    }

}

void Game::CreateRootSignature()
{
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    // Create single descriptor table of CBVs.
    CD3DX12_DESCRIPTOR_RANGE1 ranges[1] = {};
    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

    CD3DX12_ROOT_PARAMETER1 rootParameters[1] = {};
    rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    // Create a root signature with a single slot which points to the root parameter which consists of single descriptor table as the range.
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
    ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
}

void Game::CreatePSO()
{

    // Input Layout
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    // Describe and create graphics pipeline object (PSO)
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDesc, _countof(inputElementDesc) };
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader.Get());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = TRUE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    psoDesc.SampleDesc.Count = 1;

    ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));

}

void Game::PopulateCommandList()
{
    // Reset Command allocators and command list
    ThrowIfFailed(m_commandAllocator->Reset());
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

    // Set necessary states
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    ID3D12DescriptorHeap* ppHeaps[] = { m_cbvHeap.Get() };
    m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    m_commandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);

    // Indicate the back buffer will be used as render target
    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
    //m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

    m_commandList->ClearRenderTargetView(rtvHandle, Colors::CornflowerBlue, 0, nullptr);
    m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH , 1.0f, 0, 0, nullptr);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_commandList->IASetVertexBuffers(0, 1, &triangleMesh->GetVertexBufferView());
    m_commandList->IASetIndexBuffer(&triangleMesh->GetIndexBufferView());
    m_commandList->DrawIndexedInstanced(triangleIndexCount, 1, 0, 0, 0);

    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
    

    ThrowIfFailed(m_commandList->Close());
}

