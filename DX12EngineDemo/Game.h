#pragma once
#include "DXSample.h"
#include "Vertex.h"
#include "Mesh.h"
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

    // Pipeline objects.   
    ComPtr<ID3D12RootSignature> m_rootSignature;    
    ComPtr<ID3D12PipelineState> m_pipelineState;

    // App resources.
    ComPtr<ID3D12Resource> m_constantBuffer;
    SceneConstantBuffer m_constantBufferData;
    UINT8* m_pCbvDataBegin;

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
    void CreateRootSignature();
    void CreatePSO();
    void PopulateCommandList();

    Mesh* sphereMesh;
    int sphereIndexCount;
    GameEntity* sphereEntity;
    Camera* camera;
};

