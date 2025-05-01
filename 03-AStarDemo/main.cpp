#include "AStar.hpp"
#include "OpenCVProcessor.hpp"



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

    AStar AStarPath;
    OpenCVProcessor imageProcessor;

    std::vector<std::pair<int, int>> path = AStarPath.findPath(map_grid, 2, 2, 3, 1);
    // 打印地图，路径上的点用*表示
    if (path.empty() == false)
    {
        imageProcessor.drawMapWithOpenCV(map_grid, path);
    }

    std::cout << "Path: ";
    for (auto p : path)
    {
        std::cout << "(" << p.first << "," << p.second << ") ";
    }
    std::cout << std::endl;

    return 0;
}