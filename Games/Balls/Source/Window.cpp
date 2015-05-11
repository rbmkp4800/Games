#include <Windows.h>

#include "BallsGame.h"

using namespace BallsGame;

Game game;

inline void SetGameControlsStateByKeyCode(uint32 keycode, bool state)
{
	switch (keycode)
	{
	/*case 'Q':
		game.SetPlayerControlState(PlayerControl ::PositiveCharge, state);
		break;
	case 'E':
		Game::SetControlState(Control::NegativeCharge, state);
		break;*/

	case VK_LEFT:
	case 'A':
		game.SetPlayerControlState(PlayerControl::Left, state);
		break;
	case VK_RIGHT:
	case 'D':
		game.SetPlayerControlState(PlayerControl::Right, state);
		break;
	case VK_UP:
	case 'W':
		game.SetPlayerControlState(PlayerControl::Up, state);
		break;
	case VK_DOWN:
	case 'S':
		game.SetPlayerControlState(PlayerControl::Down, state);
		break;

	case 'R':
		//Game::Restart();
		break;
	}
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
		600, 1024, nullptr, nullptr, hInstance, nullptr);

	RECT clientRect = { 0 };
	GetClientRect(hWnd, &clientRect);
	game.Create(hWnd, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	uint64 counterFrequencyU64 = 0, lastTick = 0;
	QueryPerformanceFrequency(PLARGE_INTEGER(&counterFrequencyU64));
	QueryPerformanceCounter(PLARGE_INTEGER(&lastTick));
	float counterFrequency = float(counterFrequencyU64);

	MSG msg = { 0 };
	do
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			uint64 tick = 0;
			QueryPerformanceCounter(PLARGE_INTEGER(&tick));
			//game.Update(float(tick - lastTick) / counterFrequency);
			game.Update(0.016f);
			//game.Update(float((rand() % 40) + 1) / 1000.0f);
			lastTick = tick;
		}
	} while (msg.message != WM_QUIT);

	return 0;
}