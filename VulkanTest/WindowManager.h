#pragma once
#include <Windows.h>

class WindowManager
{
public:
    HINSTANCE hInstance;
    HWND hWindow;
    WNDCLASSEX wcex;

    wchar_t* szClassName = L"DominusClass";

    WindowManager();
    ~WindowManager();

    bool regWindow();
    int initWindow();

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMSG, WPARAM wParam, LPARAM lParam);
};

