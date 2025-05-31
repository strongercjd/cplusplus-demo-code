#include "AStar.hpp"
#include "OpenCVProcessor.hpp"
#include <opencv2/opencv.hpp>

#define CELLSIZE 5

int main()
{
    MapInfo map_grid;
    /* 
    opencv 坐标系特性
    1. **原点位置**  
    原点位于 **图像左上角**（与屏幕坐标系一致）

    2. **坐标轴方向**  
    - X 轴：从左到右递增（列方向）
    - Y 轴：从上到下递增（行方向）
    */
    cv::Mat map_img = cv::imread("map.png");

    if (!map_img.empty()) {
        // 从图片加载地图数据
        map_grid.width = map_img.cols/CELLSIZE;
        map_grid.height = map_img.rows/CELLSIZE;

        map_grid.data.resize(map_grid.width * map_grid.height);

        // 转换像素数据（考虑 Y 轴翻转）
        int x,y;
        for (int height = 0; height < map_grid.height; height++)
        {
            y = (map_grid.height - height - 1) * CELLSIZE + CELLSIZE / 2;
            for (int width = 0; width < map_grid.width; width++)
            {
                x = width * CELLSIZE  + CELLSIZE / 2;

                // 黑色像素（0）视为障碍物，其他为可通行
                cv::Vec3b pixel = map_img.at<cv::Vec3b>(y, x);
                map_grid.data[map_grid.width * height + width] = 0;
                if ((static_cast<int>(pixel[0]) == 0) && (static_cast<int>(pixel[1]) == 0) && static_cast<int>(pixel[2]) == 0)
                {
                    map_grid.data[map_grid.width * height + width] = 1;
                }
            }
        }
    } else {
        // 默认初始化
        map_grid.width = 100;
        map_grid.height = 100;
        map_grid.data.resize(map_grid.width * map_grid.height);
        std::fill(map_grid.data.begin(), map_grid.data.end(), 0);
    }

    AStar AStarPath;
    OpenCVProcessor imageProcessor;

    std::vector<std::pair<int, int>> path = AStarPath.findPath(map_grid, 2, 15, 60, 80);
    // 打印地图，路径上的点用*表示
    if (path.empty() == false)
    {
        imageProcessor.drawMapWithOpenCV(map_grid, path,CELLSIZE);
    }

    std::cout << "Path: ";
    for (auto p : path)
    {
        std::cout << "(" << p.first << "," << p.second << ") ";
    }
    std::cout << std::endl;

    return 0;
}