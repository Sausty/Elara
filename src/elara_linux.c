/*
** EPITECH PROJECT, 2022
** elara_linux.c
** File description:
** Linux entry point and platform layer
*/

#include "elara_common.h"
#include "elara_platform.h"
#include "elara_vulkan.h"
#include "elara_game.h"

#if defined(ELARA_PLATFORM_LINUX)

#include <xcb/xcb.h>
#include <X11/Xlib-xcb.h>
#include <stdio.h>

typedef struct linux_state {
    Display* Display;
    xcb_connection_t* Connection;
    xcb_window_t Window;
    xcb_screen_t* Screen;
    xcb_atom_t Protocols;
    xcb_atom_t Delete;
} linux_state;

linux_state LinuxState;
platform_state PlatformState;

void PlatformCreateSurface(VkInstance Instance, VkSurfaceKHR* Surface)
{
    VkXcbSurfaceCreateInfoKHR SurfaceCreateInfo = {0};
    SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    SurfaceCreateInfo.connection = LinuxState.Connection;
    SurfaceCreateInfo.window = LinuxState.Window;
    
    VkResult Result = vkCreateXcbSurfaceKHR(Instance, &SurfaceCreateInfo, NULL, Surface);
    CheckVk(Result, "Failed to create XCB surface!");
}

void InitPlatform()
{
    PlatformState.Dimensions.Width = 1280;
    PlatformState.Dimensions.Height = 720;
    PlatformState.Quit = false;

    LinuxState.Display = XOpenDisplay(NULL);
    if (!LinuxState.Display) 
    {
        printf("Failed to open X11 display!");
        return;
    }
    
    LinuxState.Connection = XGetXCBConnection(LinuxState.Display);
    if (xcb_connection_has_error(LinuxState.Connection)) 
    {
        XCloseDisplay(LinuxState.Display);
        printf("Failed to connect to X server via XCB");
        return;
    }

    const struct xcb_setup_t* Setup = xcb_get_setup(LinuxState.Connection);
    xcb_screen_iterator_t Iterator = xcb_setup_roots_iterator(Setup);
    i32 Screen = 0;
    for (i32 ScreenNumber = Screen; ScreenNumber > 0; ScreenNumber--)
    {   
        xcb_screen_next(&Iterator);
    }
    LinuxState.Screen = Iterator.data;

    LinuxState.Window = xcb_generate_id(LinuxState.Connection);
    u32 EventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    u32 EventListeners = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                       XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                       XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                       XCB_EVENT_MASK_STRUCTURE_NOTIFY;
    u32 ValueList[] = { LinuxState.Screen->black_pixel, EventListeners };

    xcb_create_window(LinuxState.Connection, 
                      XCB_COPY_FROM_PARENT, 
                      LinuxState.Window, 
                      LinuxState.Screen->root, 
                      0, 0, 
                      1280, 720, 
                      0, 
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, 
                      LinuxState.Screen->root_visual, 
                      EventMask, 
                      ValueList);
    xcb_change_property(LinuxState.Connection, 
                        XCB_PROP_MODE_REPLACE, 
                        LinuxState.Window, 
                        XCB_ATOM_WM_NAME, 
                        XCB_ATOM_STRING, 
                        8, 5, 
                        "Elara");

    xcb_intern_atom_cookie_t DeleteCookie = xcb_intern_atom(LinuxState.Connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t ProtocolsCookie = xcb_intern_atom(LinuxState.Connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* DeleteReply = xcb_intern_atom_reply(LinuxState.Connection, DeleteCookie, NULL);
    xcb_intern_atom_reply_t* ProtocolsReply = xcb_intern_atom_reply(LinuxState.Connection, ProtocolsCookie, NULL);
    LinuxState.Protocols = ProtocolsReply->atom;
    LinuxState.Delete = DeleteReply->atom;

    xcb_change_property(LinuxState.Connection, XCB_PROP_MODE_REPLACE, LinuxState.Window, ProtocolsReply->atom, 4, 32, 1, &DeleteReply->atom);
    xcb_map_window(LinuxState.Connection, LinuxState.Window);
    xcb_flush(LinuxState.Connection);
}

void ExitPlatform()
{
    xcb_destroy_window(LinuxState.Connection, LinuxState.Window);
    XCloseDisplay(LinuxState.Display);
}

int main(void)
{
    InitPlatform();

    xcb_generic_event_t* Event;
    xcb_client_message_event_t* CustomClientMessage;
    while (!PlatformState.Quit)
    {
        while ((Event = xcb_poll_for_event(LinuxState.Connection)))
        {
            switch (Event->response_type & ~0x80)
            {
            case XCB_CLIENT_MESSAGE:
            {
                CustomClientMessage = (xcb_client_message_event_t*)Event;

                // Window close
                if (CustomClientMessage->data.data32[0] == LinuxState.Delete)
                {
                    PlatformState.Quit = true;
                }
            } break;
            default:
            {
                break;
            }
            }
        }
        
        ElaraUpdate();
    }
    ExitPlatform();
    return (0);
}

#endif