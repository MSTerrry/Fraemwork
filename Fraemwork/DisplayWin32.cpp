#include "DisplayWin32.h"


DisplayWin32::DisplayWin32(){}

DisplayWin32::DisplayWin32(HINSTANCE hInstance, WNDPROC WndProc) {

	int posX, posY;
	_applicationName = L"Game";
	ScreenWidth = 1024;
	ScreenHeight = 800;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = _applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Call to RegisterClassEx failed!", L"Windows Desktop Guided Tour", NULL);
		return;
	}

	/*ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);*/	

	posX = (GetSystemMetrics(SM_CXSCREEN) - ScreenWidth) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - ScreenHeight) / 2;

	RECT windowRect = { 0, 0, static_cast<LONG>(ScreenWidth), static_cast<LONG>(ScreenHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;
	// Create the window with the screen settings and get the handle to it.
	hWnd = CreateWindowEx(WS_EX_APPWINDOW, _applicationName, _applicationName,
		dwStyle,
		posX, posY,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
	{
		MessageBox(NULL, L"Call to CreateWindow failed!", L"Windows Desktop Guided Tour", NULL);
		return;
	}

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	ShowCursor(true);
	return;
}

