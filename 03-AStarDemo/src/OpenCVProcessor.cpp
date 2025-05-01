#include "OpenCVProcessor.hpp"

// OpenCV绘图函数封装
void OpenCVProcessor::drawMapWithOpenCV(const MapInfo &map_grid,
                                        const std::vector<std::pair<int, int>> &path,
                                        int cell_size)
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