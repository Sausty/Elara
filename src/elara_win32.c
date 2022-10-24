/*
** EPITECH PROJECT, 2022
** elara_win32.c
** File description:
** Win32 entry point and platform layer for Elara
*/

#include "elara_common.h"
#include "elara_platform.h"

#if defined(ELARA_PLATFORM_WINDOWS)

#include <stdio.h>

typedef struct win32_state {
    HINSTANCE Instance;
    WNDCLASSA WindowClass;
    HWND Window;
} win32_state;

win32_state WinState;
platform_state PlatformState;

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    switch (Message)
    {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            PlatformState.Quit = true;
        }
    }
    
    return DefWindowProc(Window, Message, WParam, LParam);
}

void InitPlatform()
{
    WinState.WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WinState.WindowClass.hInstance = WinState.Instance;
    WinState.WindowClass.lpfnWndProc = WindowProc;
    WinState.WindowClass.lpszClassName = "ElaraClass";
    
    if (!RegisterClass(&WinState.WindowClass))
    {
        printf("Failed to register Win32 window class!");
        return;
    }
    
    WinState.Window = CreateWindowEx(0,
                                     "ElaraClass",
                                     "Elara",
                                     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     0,
                                     0,
                                     WinState.Instance,
                                     0);
    if (!WinState.Window)
    {
        printf("Failed to create Win32 window!");
        return;
    }
    SetWindowLong(WinState.Window, GWL_STYLE, GetWindowLong(WinState.Window, GWL_STYLE) & ~WS_MINIMIZEBOX);
}

void ExitPlatform()
{
	DestroyWindow(WinState.Window);
    UnregisterClassA("ElaraClass", WinState.Instance);
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCommand)
{
    WinState.Instance = Instance;
    InitPlatform();
	
    while (!PlatformState.Quit)
    {
        MSG Message;
        while (PeekMessageA(&Message, WinState.Window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
    }
    
    ExitPlatform();
    return (0);
}

#endif