#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <set>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;

// 对于map访问，修改为左下角为原点，向右为X正方向，向上为Y的正方向，坐标从0开始
struct MapInfo
{
    int width;
    int height;
    vector<int> data; // 0表示可通行，1～100表示障碍物 -1表示未知
};

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

vector<pair<int, int>> AStar(MapInfo &grid,
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
    priority_queue<Node *, vector<Node *>, CompareNode> openList;
    set<pair<int, int>> closedList;

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
            vector<pair<int, int>> path;
            while (current != nullptr)
            {
                path.emplace_back(current->x, current->y);
                current = current->parent;
            }
            reverse(path.begin(), path.end());
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

// OpenCV绘图函数封装
void drawMapWithOpenCV(const MapInfo &map_grid,
                       const vector<pair<int, int>> &path,
                       int cell_size = 50)
{
    // 创建彩色图像（每个格子50x50像素）
    cv::Mat map_img(map_grid.height * cell_size,
                    map_grid.width * cell_size,
                    CV_8UC3,
                    cv::Scalar(255, 255, 255)); // 初始化为白色

    // 绘制网格和障碍物
    for (int y = 0; y < map_grid.height; ++y)
    {
        for (int x = 0; x < map_grid.width; ++x)
        {
            int index = (map_grid.height - 1 - y) * map_grid.width + x; // 保持坐标系统一致

            // 绘制障碍物（黑色）
            if (map_grid.data[index] == 1)
            {
                cv::rectangle(map_img,
                              cv::Point(x * cell_size, y * cell_size),
                              cv::Point((x + 1) * cell_size - 1, (y + 1) * cell_size - 1),
                              cv::Scalar(0, 0, 0), // BGR颜色
                              cv::FILLED);
            }
        }
    }

    // 绘制路径（绿色）
    for (auto &p : path)
    {
        int img_y = map_grid.height - 1 - p.second; // 转换为图像坐标
        cv::rectangle(map_img,
                      cv::Point(p.first * cell_size, img_y * cell_size),
                      cv::Point((p.first + 1) * cell_size - 1, (img_y + 1) * cell_size - 1),
                      cv::Scalar(0, 255, 0),
                      cv::FILLED);
    }

    // 添加网格线
    for (int i = 0; i <= map_grid.width; ++i)
    {
        cv::line(map_img,
                 cv::Point(i * cell_size, 0),
                 cv::Point(i * cell_size, map_img.rows),
                 cv::Scalar(200, 200, 200));
    }
    for (int i = 0; i <= map_grid.height; ++i)
    {
        cv::line(map_img,
                 cv::Point(0, i * cell_size),
                 cv::Point(map_img.cols, i * cell_size),
                 cv::Scalar(200, 200, 200));
    }

    // 显示图像
    cv::imshow("A* Path Planning", map_img);
    cv::waitKey(0);
}

// 测试代码
int main()
{

    MapInfo map_grid;
    map_grid.width = 5;
    map_grid.height = 5;
    map_grid.data = {0, 0, 0, 0, 0,
                     0, 1, 1, 1, 0,
                     0, 0, 0, 1, 0,
                     0, 1, 1, 0, 0,
                     0, 0, 0, 1, 0};

    vector<pair<int, int>> path = AStar(map_grid, 2, 2, 3, 1);
    // 打印地图，路径上的点用*表示
    if (path.empty() == false)
    {
        drawMapWithOpenCV(map_grid, path);
    }

    cout << "Path: ";
    for (auto p : path)
    {
        cout << "(" << p.first << "," << p.second << ") ";
    }
    cout << endl;

    return 0;
}