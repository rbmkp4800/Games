#include <Windows.h>

#include "BallsGame.h"

inline void SetGameControlsStateByKeyCode(uint32 keycode, bool state)
{
	/*switch (keycode)
	{
	case 'Q':
		Game::SetControlState(Control::PositiveCharge, state);
		break;
	case 'E':
		Game::SetControlState(Control::NegativeCharge, state);
		break;

	case VK_UP:
	case 'W':
		Game::SetControlState(Control::Up, state);
		break;
	case VK_DOWN:
	case 'S':
		Game::SetControlState(Control::Down, state);
		break;
	case VK_LEFT:
	case 'A':
		Game::SetControlState(Control::Left, state);
		break;
	case VK_RIGHT:
	case 'D':
		Game::SetControlState(Control::Right, state);
		break;

	case 'R':
		Game::Restart();
		break;
	}*/
}

LRESULT __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		SetGameControlsStateByKeyCode(wParam, true);
		break;

	case WM_KEYUP:
		SetGameControlsStateByKeyCode(wParam, false);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

DWORD __stdcall GameThreadMain(void* args)
{
	HWND hWnd = (HWND) args;
	RECT clientRect = { 0 };
	GetClientRect(hWnd, &clientRect);

	BallsGame::Game game;
	game.Create(hWnd, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
	for (;;)
		game.Update(0.0f);

	return 0;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, char* lpCmdLine, int nCmdShow)
{
	wchar_t wndClass [] = L"BallsGame.WndClass";

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = wndClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow(wndClass, L"Balls Game", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		800, 800, nullptr, nullptr, hInstance, nullptr);

	CreateThread(nullptr, 0, GameThreadMain, hWnd, 0, nullptr);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg = { 0 };
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}