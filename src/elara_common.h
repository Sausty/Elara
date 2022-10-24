/*
** EPITECH PROJECT, 2022
** elara_common.h
** File description:
** File included by every source file in Elara.
*/

#ifndef ELARA_COMMON_H_
#define ELARA_COMMON_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef char i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef i8 b8;

#define false 0
#define true 1

#if defined(_WIN32)
    #define ELARA_PLATFORM_WINDOWS
    #include <Windows.h>
#elif defined(__linux__)
    #define ELARA_PLATFORM_LINUX
#endif

#endif