#pragma once
#include <opencv2/opencv.hpp>
#include "MapInfo.hpp"
#include <utility> // 包含 pair 的头文件

class OpenCVProcessor
{
public:
    void drawMapWithOpenCV(MapInfo &map_grid,
                           const std::vector<std::pair<int, int>> &path,
                           int cell_size = 50);
    void drawSaveButton(cv::Mat &img);
    static void saveButtonHandler(int event, int x, int y, int flags, void* userdata);
    static void mapClickHandler(int event, int x, int y, int flags, void* userdata);

    void redrawMap(cv::Mat& map_img, MapInfo& map_grid, 
        const std::vector<std::pair<int, int>>& path,
        int cell_size);
};