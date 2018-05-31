#define DomTest 0

#if DomTest
#include <Windows.h>

LRESULT WndProc(HWND hwnd, UINT uMSG, WPARAM wParam, LPARAM lParam)
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
    case WM_MOUSEFIRST:
        hdc = BeginPaint(hwnd, &ps);
        TextOut(hdc, 200, 5, L"REEEEEEEEEEEEEEEE", 4);
        EndPaint(hwnd, &ps);
        break;
    default:
        return DefWindowProc(hwnd, uMSG, wParam, lParam);
        break;
    }

    return 0;
}
#endif

#include "DominusEngine.h"
#include <stdexcept>
#include <iostream>
#include <fstream>

void logToFile(std::string aLog) 
{
	std::ofstream logFile("log.txt", std::ios::out | std::ios::trunc);

	if (!logFile.is_open()) 
	{
		std::cout << "Couldnt not create or open log file" << std::endl;
		return;
	}
	
	logFile << aLog;
	logFile.close();
}


int main() 
{
#if DomTest
    HINSTANCE hInstance;
    HWND hWindow;
    WNDCLASSEX wcex;

    //ShowWindow(GetConsoleWindow(), SW_HIDE);

    auto szClassName = L"DominusClass";
    hInstance = GetModuleHandle(0);

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szClassName;
    wcex.hIconSm = NULL;

    auto reg = RegisterClassEx(&wcex);

    if (!reg)
    {
        std::cout << "Failed to register: " << reg << std::endl;
        std::cout << "Error: " << GetLastError() << std::endl;
    }

    hWindow = CreateWindowEx(
        WS_EX_TOPMOST,
        szClassName,
        L"TestWindow",
        WS_OVERLAPPEDWINDOW,
        0, 0,
        500, 100,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hWindow)
    {
        MessageBox(NULL, L"Call to create window failed", L"Dominus window manager", NULL);
        throw std::runtime_error("Failed to create window!");
    }

    ShowWindow(hWindow, SW_SHOWNORMAL);
    UpdateWindow(hWindow);
    SetFocus(hWindow);

    MSG msg;
    DominusEngine de;

    try {
        de.run();
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        std::string errorString("Error: ");
        errorString.append(e.what());
        logToFile(errorString);
        return EXIT_FAILURE;
    }

   /* while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }*/

   // return (int)msg.wParam;
#else
    DominusEngine de;

    try {
        de.run();
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        std::string errorString("Error: ");
        errorString.append(e.what());
        logToFile(errorString);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
#endif // window
}