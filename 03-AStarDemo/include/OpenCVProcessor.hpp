#pragma once
#include <opencv2/opencv.hpp>
#include "MapInfo.hpp"
#include <utility> // 包含 pair 的头文件

class OpenCVProcessor
{
public: 
    void drawMapWithOpenCV(const MapInfo &map_grid,
                           const std::vector<std::pair<int, int>> &path,
                           int cell_size = 50);
};