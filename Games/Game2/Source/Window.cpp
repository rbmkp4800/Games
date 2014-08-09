#include <Windows.h>
#include <WindowsX.h>

#include "types.h"
#include "Game2.h"

#define MOUSE_SENS	0.005f

LRESULT __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int16x2 lastMousePos;
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			cameraSphericPosition.x += (float) (lastMousePos.x - GET_X_LPARAM(lParam)) * MOUSE_SENS;
			cameraSphericPosition.y += (float) (lastMousePos.y - GET_Y_LPARAM(lParam)) * MOUSE_SENS;
		}
		lastMousePos.set(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_MOUSEWHEEL:
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0.0f)
			cameraSphericPosition.z *= 1.1f;
		else
			cameraSphericPosition.z /= 1.1f;
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, char*, int)
{
	WNDCLASSEX wcex;

	const wchar_t wndClassName[] = L"wndCls";

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

	HWND hWnd = CreateWindowW(wndClassName, L"Game2", WS_OVERLAPPEDWINDOW,
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