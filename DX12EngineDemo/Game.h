#pragma once
#include "DXSample.h"
#include "Vertex.h"
#include "Camera.h"
#include "GameEntity.h"

using namespace DirectX;

using namespace Microsoft::WRL;
class Game : public DXSample
{
public:
	Game(HINSTANCE hInstance);
    ~Game();

	virtual void Init();
	void Update(float deltaTime, float totalTime);
	void Render(float deltaTime, float totalTime);
    virtual void Destroy();
    virtual void OnResize()override;

    // Overriden mouse input helper functions
    void OnMouseDown(WPARAM buttonState, int x, int y);
    void OnMouseUp(WPARAM buttonState, int x, int y);
    void OnMouseMove(WPARAM buttonState, int x, int y);
    void OnMouseWheel(WPARAM wheelData, int x, int y);

private:
	
    struct SceneConstantBuffer
    {
        XMFLOAT4X4 worldMatrix;
        XMFLOAT4X4 viewMatrix;
        XMFLOAT4X4 projMatrix;
    };

    struct Light
    {
        XMFLOAT3 Strength /*= { 0.5f, 0.5f, 0.5f }*/;
        float FalloffStart /*= 1.0f*/;                              // point/spot light only
        XMFLOAT3 Direction /*= { 0.0f, -1.0f, 0.0f }*/;    // directional/spot light only
        float FalloffEnd /*= 10.0f*/;                               // point/spot light only
        XMFLOAT3 Position /*= { 0.0f, 0.0f, 0.0f }*/;      // point/spot light only
        float SpotPower/* = 64.0f*/;
    };

#define MaxLights 3

    struct LightConstantBuffer
    {
        XMFLOAT4 AmbientLight;
        XMFLOAT3 EyePos;
        float pad = 0.0f;
        Light lights[MaxLights];
    };

    struct MaterialConstantBuffer
    {
        XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
        XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
        float Roughness = 0.25f;
    };

    // Pipeline objects.   
    ComPtr<ID3D12RootSignature> m_rootSignature;    
    ComPtr<ID3D12PipelineState> m_pipelineState;

    // Constant Buffer data.
    ComPtr<ID3D12Resource> m_constantBuffer;
    SceneConstantBuffer m_constantBufferData;
    UINT8* m_pCbvDataBegin;

    ComPtr<ID3D12Resource> m_lightCB;
    LightConstantBuffer m_lightCBData;
    UINT8* m_plightCbvDataBegin;

    ComPtr<ID3D12Resource> m_materialCB;
    MaterialConstantBuffer m_materialCBData;
    UINT8* m_pmaterialCbvDataBegin;

    UINT m_AlignedSceneCBSize = (sizeof(SceneConstantBuffer) + 255) & ~255;
    UINT m_AlignedLightCBSize = (sizeof(LightConstantBuffer) + 255) & ~255;
    UINT m_AlignedMaterialCBSize = (sizeof(MaterialConstantBuffer) + 255) & ~255;

    // Texture related stuff
    ComPtr<ID3D12Resource> m_textureBuffer;
    ComPtr<ID3D12Resource> m_textureUploadBuffer;
    std::vector<D3D12_SUBRESOURCE_DATA> m_textureSubResourceData;
    ComPtr<ID3D12DescriptorHeap> m_samplerHeap;

    // Shaders
    ComPtr<ID3DBlob> m_vertexShader;
    ComPtr<ID3DBlob> m_pixelShader;

    // Matrices
    XMFLOAT4X4 m_worldMatrix;
    XMFLOAT4X4 m_viewMatrix;
    XMFLOAT4X4 m_projMatrix;

    // Keeps track of the old mouse position.
    // USeful for determining how far the mouse moved in a single frame.
    POINT m_prevMousePos;

    void LoadShaders();
    void CreateMatrices();
    void CreateBasicGeometry();
    void CreateConstantBuffers();
    void LoadTextures();
    void CreateMaterials();
    void CreateRootSignature();
    void CreatePSO();
    void PopulateCommandList();

    //Update the Constant Buffers
    void UpdateSceneConstantBuffer(float deltaTime);
    void UpdateLightConstantBuffer(float deltaTime);
    void UpdateMaterialConstantBuffer();

    // Shader Compile
    HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);

    GameEntity* sphereEntity;
    Camera* camera;

    XMFLOAT3 m_cameraPos = { 0.0f, 5.0f, -30.0f };
};

