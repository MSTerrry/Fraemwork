#pragma once
#include <iostream>
#include "windows.h"
class DisplayWin32 
{
public:
	int ScreenHeight;
	int ScreenWidth;
	HWND hWnd = NULL;
	WNDCLASSEX wc;
	LPCWSTR _applicationName;
	DisplayWin32();
	DisplayWin32(HINSTANCE hInstance, WNDPROC WndProc);
};