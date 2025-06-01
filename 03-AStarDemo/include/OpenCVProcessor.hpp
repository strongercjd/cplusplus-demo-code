#pragma once
#include <opencv2/opencv.hpp>
#include "MapInfo.hpp"
#include <utility> // 包含 pair 的头文件

class OpenCVProcessor
{
public:
    class MouseContext
    {
    public:
        MapInfo *mapData;        // 地图网格数据指针
        cv::Mat *mapImage;       // 可交互地图图像
        int cell_size;           // 每个格子的大小
        bool isDragging = false; // 拖动状态标志
        MouseContext(MapInfo *data, cv::Mat *img, int size)
            : mapData(data), mapImage(img), cell_size(size) {}
    };
    void drawMapWithOpenCV(MapInfo &map_grid,
                           const std::vector<std::pair<int, int>> &path,
                           int cell_size = 50);
    void drawSaveButton(cv::Mat &img);
    static void saveButtonHandler(int event, int x, int y, int flags, void *userdata);
    static void mapClickHandler(int event, int x, int y, int flags,void *userdata);

    void drawMap(cv::Mat &map_img, MapInfo &map_grid,
        int cell_size);
    void drawGridMap(cv::Mat &map_img, MapInfo &map_grid,
                     int cell_size);
    void drawPathMap(cv::Mat &path_img, MapInfo &map_grid,
                     const std::vector<std::pair<int, int>> &path,
                     int cell_size);

    static void processGridClick(int x, int y, MouseContext &ctx, MapInfo &map_grid);

private:
    int cell_size = 50; // 每个格子的大小
};