#pragma once
#include "DXSampleHelper.h"
#include <Windows.h>

class DXSample
{
public:
	DXSample(
		HINSTANCE hInstance,		// The application's handle
		const char* titleBarText,			// Text for the window's title bar
		unsigned int windowWidth,	// Width of the window's client area
		unsigned int windowHeight,	// Height of the window's client area
		bool debugTitleBarStats);
	~DXSample();

	// static requirements for OS-level message processing
	static DXSample* DXSampleInstance;
	static LRESULT CALLBACK WindowProc(
		HWND hWnd,
		UINT uMSg,
		WPARAM wParam,
		LPARAM lParam);

	// Internal method for message handling
	LRESULT ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Initialization and game loop related methods
	HRESULT InitWindow();
	HRESULT InitDirectX();
	HRESULT Run();
	void Quit();
	virtual void OnResize();

	virtual void Init() = 0;
	virtual void Update(float deltaTime, float totalTime) = 0;
	virtual void Render(float deltaTime, float totalTime) = 0;
	virtual void Destroy() = 0;
	

	// Convenience methods for handling mouse input, since we 
	// can easily grab mouse input from OS-level messages
	virtual void OnMouseDown(WPARAM buttonState, int x, int y){}
	virtual void OnMouseUp(WPARAM buttonState, int x, int y){}
	virtual void OnMouseMove(WPARAM buttonState, int x, int y){}
	virtual void OnMouseWheel(WPARAM wheelData, int x, int y){}

	// Accessors
	UINT GetWidth() const { return m_width; }
	UINT GetHeight() const { return m_height; }

protected:
	std::wstring GetAssetFullPath(LPCWSTR assetName);
	

	HINSTANCE m_hInstance;
	HWND m_hWnd;
	std::string m_titleBarText;
	bool m_titleBarStats;

	// Viewport dimensions.
	UINT m_width;
	UINT m_height;
	float m_aspectRatio;

	// Adapter info.
	bool m_useWarpDevice;

	// DirectX related objects and variables
	static const UINT FrameCount = 2;
	ComPtr<ID3D12Device> m_device;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	ComPtr<ID3D12Resource> m_depthStencilBuffer;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
	UINT m_rtvDescriptorSize;
	UINT m_dsvDescriptorSize;

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	// Synchronization objects.
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue = 0;
	UINT m_frameIndex;

	void WaitForPreviousFrame();



private:
	// Root assets path.
	std::wstring m_assetsPath;


	// Timer related stuff
	double m_perfCounterSeconds;
	float m_totalTime;
	float m_deltaTime;
	__int64 m_startTime;
	__int64 m_currentTime;
	__int64 m_previousTime;

	// FPS Calculation
	int m_fpsFrameCount;
	float m_fpsTimeElapsed;

	void UpdateTimer();			// Updates the timer for this frame
	void UpdateTitleBarStats();	// Puts debug info in the title bar

	void GetHardwareAdapter(_In_ IDXGIFactory2* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter);

};

