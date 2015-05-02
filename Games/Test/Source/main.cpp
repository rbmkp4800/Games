#include <Windows.h>
#include <Render2D.h>
#include <extypes.matrix3x2.h>

wchar_t wndClass [] = L"some_app_class";
wchar_t wndTitle [] = L"Application 1";

HINSTANCE hInstance;

DWORD __stdcall RenderThreadMain(void* args)
{
	using namespace Render2D;

	HWND hWnd = (HWND) args;
	RECT wndRect;
	GetClientRect(hWnd, &wndRect);
	uint32 wndSizeX = wndRect.right - wndRect.left;
	uint32 wndSizeY = wndRect.bottom - wndRect.top;

	Device device;
	device.Create();

	SwapChain swapChain;
	swapChain.CreateForHWnd(&device, hWnd, wndSizeX, wndSizeY);
	device.SetTarget(&swapChain);

	float rotation = 0.0f;

	for (;;)
	{
		LocalMemoryBuffer<1024> buffer;
		Batch batch(&device, buffer.GetPointer(), buffer.GetSize());

		device.Clear(colors::cornflowerBlue);
		device.SetTransform(matrix3x2::scale(600.0f / 800.0f, 1.0f) * matrix3x2::rotation(rotation += 0.01f));
		batch.PushRectangle(rectf32(-0.5f, -0.5f, 0.5f, 0.5f), colors::black);
		batch.PushGradientEllipse(rectf32(-0.5f, -0.5f, 0.5f, 0.5f), colors::red, coloru32(colors::white, 0.5f), 0.25f, 0.75f);
		batch.PushGradientEllipse(rectf32(-0.5f, -0.5f, 0.5f, 0.5f), colors::white, colors::yellow, 0.75f, 1.0f);
		batch.PushRectangle(rectf32(-0.1f, -0.1f, 0.1f, 0.1f), colors::blue);
		batch.Flush();
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
