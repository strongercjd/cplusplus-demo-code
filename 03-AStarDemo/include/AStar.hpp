#pragma once
#include <vector>
#include <queue>
#include <unordered_map>
#include "MapInfo.hpp"

class AStar
{
public:
    // 定义节点结构体
    struct Node
    {
        int x, y;
        int g, h;     // g是从起点到当前节点的实际代价，h是从当前节点到目标节点的估计代价
        Node *parent; // 父节点指针

        Node(int _x, int _y) : x(_x), y(_y), g(0), h(0), parent(nullptr) {}
    };

    // 计算启发函数（曼哈顿距离）
    int heuristic(Node *a, Node *b)
    {
        return abs(a->x - b->x) + abs(a->y - b->y);
    }

    // 重载<运算符用于优先队列比较
    struct CompareNode
    {
        bool operator()(const Node *a, const Node *b) const
        {
            return (a->g + a->h) > (b->g + b->h);
        }
    };

    std::vector<std::pair<int, int>> findPath(MapInfo &grid,
                                    int startX, int startY,
                                    int endX, int endY);
};