#include <Windows.h>

#include "types.h"
#include "Render2D.h"

wchar_t wndClass [] = L"some_app_class";
wchar_t wndTitle [] = L"Application 1";

HINSTANCE hInstance;

DWORD __stdcall RenderThreadMain(void* args)
{
	using namespace Render2D;

	HWND hWnd = (HWND) args;
	RECT wndRect;
	GetClientRect(hWnd, &wndRect);
	uint32x2 wndSize(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);

	Render::Init();
	SwapChain swapChain;
	swapChain.Create(hWnd, wndSize);
	Render::SetTarget(&swapChain);
	TextFormat textFormat;
	textFormat.Create();
	//Render::SetTransform(matrix3x2::verticalReflection());

	float f = 0.0f;
	for (;;)
	{
		GetClientRect(hWnd, &wndRect);
		if (wndSize.x != wndRect.right - wndRect.left || wndSize.y != wndRect.bottom - wndRect.top)
		{
			wndSize.set(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);
			swapChain.Resize(wndSize);
		}

		f += 0.02f;
		Render::Clear(colors::cornflowerBlue);
		Render::DrawLine(float32x2(50.0f, 50.0f), float32x2(sinf(f) * 200.0f, cosf(f) * 200.0f), colors::yellow, 50.0f);
		//Render::DrawEllipse(&rectf32(-100.0f, -100.0f, 2000.0f, 2000.0f), 0xffff0000, 0.5f);
		Render::DrawFillRect(&rectf32(10.0f, 10.0f, 100.0f, 100.0f), colors::red);
		float32x2 v [] =
		{
			{ 210.0f, 220.0f },
			{ 280.0f, 320.0f },
			{ 290.0f, 200.0f },
			{ 250.0f, 120.0f },
		};
		Render::DrawQuadrangle(v, colors::cyan);
		char* str = "ןנטגוע ובףקטו רנטפעû כמכ!!!ְױְױְױְױְ ENG: Fuck #include<striod.gh>";
		Render::DrawText(float32x2(10.0f, 10.0f), 32.0f, str, strlen(str), &textFormat);
		Render::Flush();
		swapChain.Present();
	}

	return 0;
}

LRESULT __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
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

int __stdcall WinMain(HINSTANCE _hInstance, HINSTANCE, char* lpCmdLine, int nCmdShow)
{
	hInstance = _hInstance;

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

	RECT wndRect = { 0, 0, 800, 600 };
	AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, false);

	HWND hWnd = CreateWindow(wndClass, wndTitle, WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION, CW_USEDEFAULT, CW_USEDEFAULT,
		wndRect.right - wndRect.left, wndRect.bottom - wndRect.top,
		nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	DWORD threadid = 0;
	CreateThread(nullptr, 0, RenderThreadMain, hWnd, 0, &threadid);

	MSG msg = { 0 };

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
