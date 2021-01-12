#include <Windows.h>
#include "Game.h"

int WINAPI WinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Create the Game object using the 
	// app handle we got from WinMain
	Game dxGame(hInstance);

	//Result variable for function calls below
	HRESULT hr = S_OK;

	// Attempt to create the window for our program, and
	// exit early if something failed
	hr = dxGame.InitWindow();
	if (FAILED(hr))
		return hr;

	// Attempt to Initialize DirectX, and 
	// exit early if something failed
	hr = dxGame.InitDirectX();
	if(FAILED(hr))
			return hr;

	return dxGame.Run();
}