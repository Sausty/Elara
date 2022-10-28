/*
** EPITECH PROJECT, 2022
** elara_render_graph.h
** File description:
** Header file with all the render graph related functions
*/

#ifndef ELARA_RENDER_GRAPH_H_
#define ELARA_RENDER_GRAPH_H_

#include "elara_vulkan.h"

#define DECLARE_NODE_OUTPUT(index) ((~(1u << 31u)) & index)
#define DECLARE_NODE_INPUT(index) ((1u << 31u) | index)
#define IS_NODE_INPUT(id) (((1u << 31u) & id) > 0)
#define GET_NODE_PORT_INDEX(id) (((1u << 31u) - 1u) & id)

typedef struct render_graph_node render_graph_node;

typedef struct render_graph_node_input {
    render_graph_node* Owner;
    u32 Index;
} render_graph_node_input;

struct render_graph_node {
    void* Reserved;
    
    void (*Init)(render_graph_node* GraphNode);
    void (*Free)(render_graph_node* GraphNode);
    void (*Update)(render_graph_node* GraphNode);
    void (*Resize)(render_graph_node* GraphNode);
    
    gpu_image Output[32];
    u32 OutputCount;
    
    render_graph_node_input Inputs[32];
    u32 InputCount;
};

typedef struct render_graph {
    render_graph_node* Nodes[32];
    u32 NodeCount;
} render_graph;

void RenderGraphInit(render_graph* Graph);
void RenderGraphConnectNodes(render_graph* Graph, u32 Src, u32 Dst, render_graph_node* SrcNode, render_graph_node* DstNode);
void RenderGraphBake(render_graph* Graph, render_graph_node* LastNode);
void RenderGraphFree(render_graph* Graph);
void RenderGraphResize(render_graph* Graph);
void RenderGraphUpdate(render_graph* Graph);

gpu_image* RenderGraphGetImageFromInput(render_graph_node_input* Input);

#endif
