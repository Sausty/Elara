/*
** EPITECH PROJECT, 2022
** elara_platform.h
** File description:
** Platform layer for Elara
*/

#ifndef ELARA_PLATFORM_H_
#define ELARA_PLATFORM_H_

#include "elara_common.h"

typedef struct window_dimensions {
    u32 Width;
    u32 Height;
} window_dimensions;

typedef struct platform_state {
    window_dimensions Dimensions;
    b8 Quit;
} platform_state;

extern platform_state PlatformState;

void InitPlatform();
void ExitPlatform();

char* PlatformReadFile(const char* Path, u32* OutSize);

#endif