#include "AStar.hpp"
#include "OpenCVProcessor.hpp"

// 测试代码
int main()
{
    MapInfo map_grid;
    map_grid.width = 100;
    map_grid.height = 100;
    map_grid.data.resize(map_grid.width * map_grid.height);
    std::fill(map_grid.data.begin(), map_grid.data.end(), 0);

    AStar AStarPath;
    OpenCVProcessor imageProcessor;

    std::vector<std::pair<int, int>> path = AStarPath.findPath(map_grid, 2, 2, 20, 50);
    // 打印地图，路径上的点用*表示
    if (path.empty() == false)
    {
        imageProcessor.drawMapWithOpenCV(map_grid, path,5);
    }

    std::cout << "Path: ";
    for (auto p : path)
    {
        std::cout << "(" << p.first << "," << p.second << ") ";
    }
    std::cout << std::endl;

    return 0;
}