/*
** EPITECH PROJECT 2022
** elara_game.c
** File description:
** Implementation for Elara's game code
*/

#include "elara_game.h"
#include "elara_vulkan.h"

const f32 Vertices[] = {
    -0.5f,-0.5f, 0.0f,
     0.0f, 0.5f, 0.0f,
     0.5f,-0.5f, 0.0f
};

typedef struct game_state {
    gpu_buffer VertexBuffer;
} game_state;

game_state GS;

void ElaraInit()
{
    BufferAllocate(&GS.VertexBuffer, sizeof(Vertices), GpuBufferUsage_Vertex);
}

void ElaraUpdate()
{
    RenderBegin();
    RenderEnd();
    RenderPresent();
}

void ElaraFree()
{
    BufferFree(&GS.VertexBuffer);
}