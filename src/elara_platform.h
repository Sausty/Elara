/*
** EPITECH PROJECT, 2022
** elara_platform.h
** File description:
** Platform layer for Elara
*/

#ifndef ELARA_PLATFORM_H_
#define ELARA_PLATFORM_H_

#include "elara_common.h"

typedef struct platform_state {
    u32 WindowWidth;
    u32 WindowHeight;
    b8 Quit;
} platform_state;

extern platform_state PlatformState;

void InitPlatform();
void ExitPlatform();

#endif