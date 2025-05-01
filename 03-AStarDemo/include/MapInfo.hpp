#pragma once

#include <vector>

// 对于map访问，修改为左下角为原点，向右为X正方向，向上为Y的正方向，坐标从0开始
class MapInfo
{

public:
    MapInfo() : width(0), height(0) {}
    int width;
    int height;
    std::vector<int> data; // 0表示可通行，1～100表示障碍物 -1表示未知
};