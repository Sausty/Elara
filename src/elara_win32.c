/*
** EPITECH PROJECT, 2022
** elara_win32.c
** File description:
** Win32 entry point and platform layer for Elara
*/

#include "elara_common.h"
#include "elara_platform.h"
#include "elara_vulkan.h"
#include "elara_game.h"

#if defined(ELARA_PLATFORM_WINDOWS)

#include <stdio.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

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
            break;
        }
        case WM_SIZE:
    	{
            PlatformState.Dimensions.Width = LOWORD(LParam);
            PlatformState.Dimensions.Height = HIWORD(LParam);
            ResizeVulkan();
            break;
        }
    }
    
    return DefWindowProc(Window, Message, WParam, LParam);
}

void RefreshDimensions()
{
    RECT Rect;
    GetClientRect(WinState.Window, &Rect);
    PlatformState.Dimensions.Width = Rect.right - Rect.left;
    PlatformState.Dimensions.Height = Rect.bottom - Rect.top;
}

char* PlatformReadFile(const char* Path, u32* OutSize)
{
    FILE* File = fopen(Path, "rb");
    
    if (!File)
    {
        assert(0);
        return NULL;
    }
    
    i32 CurrentPos = ftell(File);
    fseek(File, 0, SEEK_END);
    i32 Size = ftell(File);
    fseek(File, CurrentPos, SEEK_SET);
    
    u32 FileSizePadded = (Size % 4 == 0 ? Size * 4 : (Size + 1) * 4) / 4;
    char* Buffer = malloc(FileSizePadded);
    fread(Buffer, Size, sizeof(char), File);
    fclose(File);
    *OutSize = (u32)Size;
    return(Buffer);
}

void InitPlatform()
{
    PlatformState.Dimensions.Width = 1280;
    PlatformState.Dimensions.Height = 720;
    
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
                                     1280,
                                     720,
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
    
    RefreshDimensions();
}

void ExitPlatform()
{
	DestroyWindow(WinState.Window);
    UnregisterClassA("ElaraClass", WinState.Instance);
}

void PlatformCreateSurface(VkInstance Instance, VkSurfaceKHR* Surface)
{
    VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo = {0};
    SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    SurfaceCreateInfo.hinstance = WinState.Instance;
    SurfaceCreateInfo.hwnd = WinState.Window;
    
    VkResult Result = vkCreateWin32SurfaceKHR(Instance, &SurfaceCreateInfo, NULL, Surface);
    CheckVk(Result, "Failed to create Win32 surface!");
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCommand)
{
    WinState.Instance = Instance;
    InitPlatform();
    InitVulkan();
    
    ElaraInit();
	
    while (!PlatformState.Quit)
    {
        MSG Message;
        while (PeekMessageA(&Message, WinState.Window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        
        ElaraUpdate();
    }
    
    ElaraFree();
   
    ExitVulkan();
    ExitPlatform();
    return (0);
}

#endif