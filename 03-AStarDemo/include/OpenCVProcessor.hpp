#pragma once
#include <opencv2/opencv.hpp>
#include "MapInfo.hpp"
#include <utility> // 包含 pair 的头文件

class OpenCVProcessor
{
public:
    struct MouseContext
    {
        MapInfo *mapData;  // 地图网格数据指针
        cv::Mat *mapImage; // 可交互地图图像
        int cell_size;     // 每个格子的大小
    };
    void drawMapWithOpenCV(MapInfo &map_grid,
                           const std::vector<std::pair<int, int>> &path,
                           int cell_size = 50);
    void drawSaveButton(cv::Mat &img);
    static void saveButtonHandler(int event, int x, int y, int flags, void *userdata);
    static void mapClickHandler(int event, int x, int y, int flags,void *userdata);

    void redrawMap(cv::Mat &map_img, MapInfo &map_grid,
                   const std::vector<std::pair<int, int>> &path,
                   int cell_size);

private:
    int cell_size = 50; // 每个格子的大小
};