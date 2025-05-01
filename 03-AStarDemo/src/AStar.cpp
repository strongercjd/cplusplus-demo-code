#include "AStar.hpp"
#include <cmath>
#include <set>
#include <algorithm>  // 包含 reverse 函数

std::vector<std::pair<int, int>> AStar::findPath(MapInfo &grid,
                                       int startX, int startY,
                                       int endX, int endY)
{
    int rows = grid.height; // 行数，即Y方向的长度
    int cols = grid.width;  // 列数，即X方向的长度

    // 创建起点和终点
    Node *start = new Node(startX, startY);
    Node *end = new Node(endX, endY);

    // 初始化启发值
    start->h = heuristic(start, end);

    // 开放列表和闭合列表
    std::priority_queue<Node *, std::vector<Node *>, CompareNode> openList;
    std::set<std::pair<int, int>> closedList;

    // 四个方向：上下左右
    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};

    openList.push(start);

    while (!openList.empty())
    {
        Node *current = openList.top();
        openList.pop();

        // 将当前节点加入闭合列表
        closedList.insert({current->x, current->y});

        // 如果到达目标点
        if (current->x == endX && current->y == endY)
        {
            std::vector<std::pair<int, int>> path;
            while (current != nullptr)
            {
                path.emplace_back(current->x, current->y);
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        // 遍历四个方向
        for (int i = 0; i < 4; ++i)
        {
            int newX = current->x + dx[i];
            int newY = current->y + dy[i];
            int index = (rows - 1 - newY) * cols + newX;

            // 检查边界条件和障碍物
            if (newX >= 0 && newX < cols && // X范围是[0,cols-1]
                newY >= 0 && newY < rows && // Y范围是[0,rows-1]
                grid.data[index] == 0)
            {

                Node *neighbor = new Node(newX, newY);
                neighbor->parent = current;
                neighbor->g = current->g + 1;
                neighbor->h = heuristic(neighbor, end);

                // 如果邻居在闭合列表中跳过
                if (closedList.find({newX, newY}) != closedList.end())
                {
                    delete neighbor;
                    continue;
                }

                // 添加到开放列表
                openList.push(neighbor);
            }
        }
    }

    // 没有找到路径
    return {};
}