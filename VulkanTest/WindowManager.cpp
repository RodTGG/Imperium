#include "WindowManager.h"
#include <iostream>
#include <stdexcept>

WindowManager::WindowManager()
{
    hInstance = GetModuleHandle(nullptr);
    
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szClassName;
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
}

WindowManager::~WindowManager()
{
}

int WindowManager::initWindow()
{
    if (!regWindow())
    {
        MessageBox(NULL, L"Call to register failed", L"Dominus window manager", NULL);
        throw std::runtime_error("Failed to register window!");
    }

    hWindow = CreateWindow(
        szClassName,
        L"TestWindow",
        WS_OVERLAPPED,
        CW_DEFAULT, CW_DEFAULT,
        500, 100,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hInstance) 
    {
        MessageBox(NULL, L"Call to create window failed", L"Dominus window manager", NULL);
        throw std::runtime_error("Failed to create window!");
    }

    ShowWindow(hWindow, 0);
    UpdateWindow(hWindow);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

bool WindowManager::regWindow()
{
    return RegisterClassEx(&wcex);
}

LRESULT WindowManager::WndProc(HWND hwnd, UINT uMSG, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (uMSG)
    {
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        TextOut(hdc, 5, 5, L"Test", 4);
        EndPaint(hwnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMSG, wParam, lParam);
        break;
    }

    return 0;
}
