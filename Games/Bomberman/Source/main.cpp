#include <Windows.h>

#include "types.h"
#include "Bomberman.h"

#define OUTPUT_SIZE_X	1440
#define OUTPUT_SIZE_Y	900

using namespace Render2D;
using namespace Bomberman;

DWORD __stdcall GameThreadMain(void* args)
{
	HWND hWnd = (HWND) args;

	Render::Init();
	SwapChain swapChain;
	swapChain.Create(hWnd, OUTPUT_SIZE_X, OUTPUT_SIZE_Y);

	Core::Load();
	Core::SetSwapChain(&swapChain);

	for (;;)
	{
		Core::Update();
		swapChain.Present();
	}
	return 0;
}

inline void SetControlsStateByKeyCode(uint32 keycode, bool state)
{
	switch (keycode)
	{
	case 'A':
	case VK_LEFT:
		GameBase::playerControlsState.Left(state);
		break;

	case 'D':
	case VK_RIGHT:
		GameBase::playerControlsState.Right(state);
		break;

	case 'W':
	case VK_UP:
		GameBase::playerControlsState.Up(state);
		break;

	case 'S':
	case VK_DOWN:
		GameBase::playerControlsState.Down(state);
		break;

	case VK_SPACE:
		if (state)
			GameBase::playerControlsState.DropBomb(true);
		break;
	}
}

LRESULT __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		SetControlsStateByKeyCode(wParam, true);
		break;

	case WM_KEYUP:
		SetControlsStateByKeyCode(wParam, false);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, char*, int)
{
	WNDCLASSEX wcex;

	const wchar_t wndClassName[] = L"BombermanGameWnd";

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(0);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = wndClassName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	RegisterClassEx(&wcex);

	RECT rect = { 0, 0, OUTPUT_SIZE_X, OUTPUT_SIZE_Y };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	HWND hWnd = CreateWindowW(wndClassName, L"Bomberman", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
		nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	HANDLE hGameThread = CreateThread(nullptr, 0, GameThreadMain, hWnd, 0, nullptr);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return  0;
}