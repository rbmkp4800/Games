#include <Windows.h>
#include <d2d1.h>
#include <extypes.h>
//#include "Render2D.h"

wchar_t wndClass [] = L"some_app_class";
wchar_t wndTitle [] = L"Application 1";

HINSTANCE hInstance;

DWORD __stdcall RenderThreadMain(void* args)
{
	HWND hWnd = (HWND) args;
	RECT wndRect;
	GetClientRect(hWnd, &wndRect);
	uint32 wndSizeX = wndRect.right - wndRect.left;
	uint32 wndSizeY = wndRect.bottom - wndRect.top;

	ID2D1Factory *d2dFactory = nullptr;
	ID2D1HwndRenderTarget *d2dRT = nullptr;
	ID2D1SolidColorBrush *d2dBrush = nullptr;
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);
	d2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(wndSizeX, wndSizeY)), &d2dRT);
	d2dRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &d2dBrush);

	for (;;)
	{
		d2dRT->BeginDraw();
		d2dRT->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));
		d2dRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(200.0f, 200.0f), 100.0f, 100.0f), d2dBrush, 10.0f);
		d2dRT->FillRectangle(D2D1::RectF(10.0f, 10.0f, 20.0f, 20.0f), d2dBrush);
		d2dRT->FillRectangle(D2D1::RectF(30.0f, 30.0f, 20.0f, 20.0f), d2dBrush);
		d2dRT->EndDraw();
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
