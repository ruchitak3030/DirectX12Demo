#include "HeaderHelper.h"
#include "Game.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	Game game(1280, 720, L"D3D12 Engine Demo");
	return Application::Run(&game, hInstance, nCmdShow);
}