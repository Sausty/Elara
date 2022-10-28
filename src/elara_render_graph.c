/*
** EPITECH PROJECT, 2022
** elara_render_graph.c
** File description:
** Render graph implementation
*/

#include "elara_render_graph.h"

void AddNodesRecursively(render_graph_node* GraphNode, render_graph* Graph)
{
    if (GraphNode)
    {
        Graph->Nodes[Graph->NodeCount] = GraphNode;
        Graph->NodeCount++;
        
        for (u32 Iterator = 0; Iterator < GraphNode->InputCount; Iterator++)
        {
            render_graph_node* Owner = GraphNode->Inputs[Iterator].Owner;
            if (!Owner)
                break;
            AddNodesRecursively(Owner, Graph);
        }
    }
}

void RenderGraphInit(render_graph* Graph)
{
    memset(Graph, 0, sizeof(render_graph));
    
    // TODO(milo): render_graph_execute setup
}

void RenderGraphConnectNodes(render_graph* Graph, u32 Src, u32 Dst, render_graph_node* SrcNode, render_graph_node* DstNode)
{
    assert(SrcNode);
    assert(DstNode);
    assert(!IS_NODE_INPUT(Src));
    assert(IS_NODE_INPUT(Dst));
    
    DstNode->Inputs[GET_NODE_PORT_INDEX(Dst)].Owner = SrcNode;
    DstNode->Inputs[GET_NODE_PORT_INDEX(Dst)].Index = GET_NODE_PORT_INDEX(Src);
    DstNode->InputCount++;
}

void RenderGraphBake(render_graph* Graph, render_graph_node* LastNode)
{
    render_graph Temp;
    Temp.NodeCount = 0;
    AddNodesRecursively(LastNode, &Temp);
    
    for (u32 NodeIterator = Temp.NodeCount - 1; NodeIterator >= 0; NodeIterator--)
    {
        render_graph_node* GraphNode = Temp.Nodes[NodeIterator];
        b8 AlreadyInGraph = false;
        
        for (u32 Check = 0; Check < Graph->NodeCount; Check++)
        {
            if (Graph->Nodes[NodeIterator] == GraphNode)
            {
                AlreadyInGraph = true;
                break;
            }
        }
        
        if (!AlreadyInGraph)
        {
            Graph->Nodes[Graph->NodeCount] = GraphNode;
            Graph->NodeCount++;
        }
    }
    
    for (u32 NodeIterator = 0; NodeIterator < Graph->NodeCount; NodeIterator++)
    {
        render_graph_node* GraphNode = Graph->Nodes[NodeIterator];
        GraphNode->Init(GraphNode);
    }
}

void RenderGraphFree(render_graph* Graph)
{
    for (u32 NodeIterator = 0; NodeIterator < Graph->NodeCount; NodeIterator++)
    {
        render_graph_node* GraphNode = Graph->Nodes[NodeIterator];
        GraphNode->Free(GraphNode);
    }
}

void RenderGraphResize(render_graph* Graph)
{
    for (u32 NodeIterator = 0; NodeIterator < Graph->NodeCount; NodeIterator++)
    {
        render_graph_node* GraphNode = Graph->Nodes[NodeIterator];
        GraphNode->Resize(GraphNode);
    }
}

void RenderGraphUpdate(render_graph* Graph)
{
    for (u32 NodeIterator = 0; NodeIterator < Graph->NodeCount; NodeIterator++)
    {
        render_graph_node* GraphNode = Graph->Nodes[NodeIterator];
        GraphNode->Update(GraphNode);
    }
}

gpu_image* RenderGraphGetImageFromInput(render_graph_node_input* Input)
{
    assert(Input->Owner);
    return &Input->Owner->Output[Input->Index];
}