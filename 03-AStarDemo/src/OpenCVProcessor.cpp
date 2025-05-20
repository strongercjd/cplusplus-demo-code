#include "OpenCVProcessor.hpp"
/**
 * @brief 绘制保存按钮
 * 
 * @param img 要绘制按钮的图像
 */
void OpenCVProcessor::drawSaveButton(cv::Mat &img)
{
    // 在图像右下角绘制按钮
    cv::Rect buttonRect(img.cols - 120, 10, 110, 30);
    cv::rectangle(img, buttonRect, cv::Scalar(200, 200, 250), cv::FILLED);
    cv::putText(img, "Save Map", cv::Point(img.cols - 110, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 0), 2);
}
/**
 * @brief 处理地图点击事件
 * 
 * @param event 鼠标事件
 * @param x 鼠标点击位置的x坐标
 * @param y 鼠标点击位置的y坐标
 * @param flags 鼠标事件标志
 * @param userdata 用户数据指针
 */
void OpenCVProcessor::mapClickHandler(int event, int x, int y, int flags, void *userdata)
{
    auto &ctx = *static_cast<MouseContext *>(userdata); // 获取完整上下文
    auto &map_grid = *ctx.mapData;

    if (event == cv::EVENT_LBUTTONDOWN)
    {
        // 转换坐标到网格系统
        int cell_size = ctx.cell_size;
        int grid_x = x / cell_size;
        int grid_y = (map_grid.height - 1) - (y / cell_size); // 转换Y轴方向

        // 边界检查
        if (grid_x >= 0 && grid_x < map_grid.width &&
            grid_y >= 0 && grid_y < map_grid.height)
        {
            int index = grid_y * map_grid.width + grid_x;
            map_grid.data[index] = 1 - map_grid.data[index]; // 切换状态
        }
    }
}
/**
 * @brief 处理保存按钮点击事件
 *
 * @param event 鼠标事件
 * @param x 鼠标点击位置的x坐标
 * @param y 鼠标点击位置的y坐标
 * @param flags 鼠标事件标志
 * @param userdata 用户数据指针
 */
void OpenCVProcessor::saveButtonHandler(int event, int x, int y, int, void *userdata)
{
    cv::Mat *img_ptr = static_cast<cv::Mat *>(userdata);
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        // 检查是否点击按钮区域（右下角110x30区域）
        if (x > (*img_ptr).cols - 120 && x < (*img_ptr).cols - 10 &&
            y > 10 && y < 40)
        {
            cv::imwrite("map.png", *img_ptr);
            std::cout << "Map saved to map.png" << std::endl;
        }
    }
}

void OpenCVProcessor::redrawMap(cv::Mat &map_img, MapInfo &map_grid,
                                const std::vector<std::pair<int, int>> &path,
                                int cell_size)
{
    map_img.setTo(cv::Scalar(255, 255, 255)); // 重置为白色

    // 原有绘制障碍物、路径、网格线的代码
    for (int y = 0; y < map_grid.height; ++y)
    {
        for (int x = 0; x < map_grid.width; ++x)
        {
            int index = (map_grid.height - 1 - y) * map_grid.width + x;
            if (map_grid.data[index] == 1)
            {
                cv::rectangle(map_img,
                              cv::Point(x * cell_size, y * cell_size),
                              cv::Point((x + 1) * cell_size - 1, (y + 1) * cell_size - 1),
                              cv::Scalar(0, 0, 0), cv::FILLED);
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
}

// OpenCV绘图函数封装
void OpenCVProcessor::drawMapWithOpenCV(MapInfo &map_grid,
                                        const std::vector<std::pair<int, int>> &path,
                                        int cell_size)
{
    // 创建彩色图像（每个格子50x50像素） 地图
    cv::Mat map_img(map_grid.height * cell_size,
                    map_grid.width * cell_size,
                    CV_8UC3,
                    cv::Scalar(255, 255, 255)); // 初始化为白色
    // 创建UI图像
    cv::Mat ui_img(map_grid.height * cell_size,
                   map_grid.width * cell_size,
                   CV_8UC3,
                   cv::Scalar(255, 255, 255)); // 初始化为白色

    this->cell_size = cell_size; // 设置格子大小


    cv::namedWindow("A* Path Planning");
    cv::setMouseCallback("A* Path Planning", [](int event, int x, int y, int flags, void *userdata)
                         {
                             auto ctx = static_cast<MouseContext *>(userdata);
                             saveButtonHandler(event, x, y, flags, ctx->mapImage); // 处理保存按钮点击事件
                             mapClickHandler(event, x, y, flags, ctx);    // 处理地图点击事件
                         },
                         new MouseContext{&map_grid, &map_img,cell_size});

    // 修改显示循环
    while (true)
    {
        redrawMap(map_img, map_grid, path, cell_size);
        ui_img = map_img.clone();
        drawSaveButton(ui_img);

        cv::imshow("A* Path Planning", ui_img);
        int key = cv::waitKey(30);
        if (key == 27)
            break;
    }
    cv::destroyAllWindows();
    cv::waitKey(0);
}