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
    cv::putText(img, "save", cv::Point(img.cols - 110, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 0), 2);
}

void OpenCVProcessor::processGridClick(int x, int y, MouseContext &ctx, MapInfo &map_grid,int flg)
{
    int cell_size = ctx.cell_size;
    int grid_x = x / cell_size;
    int grid_y = (map_grid.height - 1) - (y / cell_size);

    if (grid_x >= 0 && grid_x < map_grid.width &&
        grid_y >= 0 && grid_y < map_grid.height)
    {
        int index = grid_y * map_grid.width + grid_x;
        if (flg== 1)//左键
        {
            map_grid.data[index] = 1;
        }
        if (flg== 2)//右键
        {
            map_grid.data[index] = 0;
        }
    }
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
    auto &ctx = *static_cast<MouseContext *>(userdata);
    auto &map_grid = *ctx.mapData;

    // 处理鼠标左键按下事件
    if ((event == cv::EVENT_LBUTTONDOWN)||(event == cv::EVENT_RBUTTONDOWN))
    {
        if (event == cv::EVENT_LBUTTONDOWN)
        {
            ctx.isDragging = 1; // 设置拖动标志
        }
        else
        {
            ctx.isDragging = 2; // 设置拖动标志
        }
        processGridClick(x, y, ctx, map_grid,ctx.isDragging);
    }
    // 处理鼠标左键释放事件
    else if ((event == cv::EVENT_LBUTTONUP)||(event == cv::EVENT_RBUTTONUP))
    {
        ctx.isDragging = 0; // 清除拖动标志
    }
    // 处理鼠标移动事件
    else if (event == cv::EVENT_MOUSEMOVE)
    {
        if (ctx.isDragging) // 只有在拖动状态下处理
        {
            processGridClick(x, y, ctx, map_grid,ctx.isDragging);
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
    auto &ctx = *static_cast<MouseContext *>(userdata);
    auto mapimg_ptr = ctx.mapImage;
    auto disimg_ptr = ctx.displayImage;
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        // 检查是否点击按钮区域（右下角110x30区域）
        if (x > (*disimg_ptr).cols - 120 && x < (*disimg_ptr).cols - 10 &&
            y > 10 && y < 40)
        {
            cv::imwrite("map.png", *mapimg_ptr);
            std::cout << "Map saved to map.png" << std::endl;
        }
    }
}

void OpenCVProcessor::drawMap(cv::Mat &map_img, MapInfo &map_grid, int cell_size)
{
    map_img.setTo(cv::Scalar(255, 255, 255));
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
}
void OpenCVProcessor::drawGridMap(cv::Mat &map_img, MapInfo &map_grid,int cell_size)
{
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
void OpenCVProcessor::drawPathMap(cv::Mat &path_img, MapInfo &map_grid,
                                const std::vector<std::pair<int, int>> &path,
                                int cell_size)
{
    // 绘制路径（绿色）
    for (auto &p : path)
    {
        int img_y = map_grid.height - 1 - p.second; // 转换为图像坐标
        cv::rectangle(path_img,
                      cv::Point(p.first * cell_size, img_y * cell_size),
                      cv::Point((p.first + 1) * cell_size - 1, (img_y + 1) * cell_size - 1),
                      cv::Scalar(0, 255, 0),
                      cv::FILLED);
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

    // 显示时扩展画布（右侧增加50像素）
    cv::Mat display_img;
    cv::copyMakeBorder(map_img, display_img,
                       0, 0,  // 上下不扩展
                       0, 100, // 右侧扩展100像素
                       cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
    // 创建UI图像
    cv::Mat grid_img = map_img.clone();
    cv::Mat ui_img = map_img.clone();
    cv::Mat path_img = map_img.clone();

    this->cell_size = cell_size; // 设置格子大小

    cv::namedWindow("A* Path Planning");
    cv::setMouseCallback("A* Path Planning", [](int event, int x, int y, int flags, void *userdata)
                         {
                             auto ctx = static_cast<MouseContext *>(userdata);
                             saveButtonHandler(event, x, y, flags, ctx); // 处理保存按钮点击事件
                             mapClickHandler(event, x, y, flags, ctx);             // 处理地图点击事件
                         },
                         new MouseContext{&map_grid, &map_img, &display_img, cell_size});
    map_img.setTo(cv::Scalar(255, 255, 255)); // 重置为白色
    // 修改显示循环
    while (true)
    {
        drawMap(map_img, map_grid, cell_size);

        grid_img = map_img.clone();
        drawGridMap(grid_img, map_grid, cell_size);

        path_img = grid_img.clone();
        drawPathMap(path_img, map_grid, path, cell_size);

        // 将路径图像复制到显示图像的左侧区域
        path_img.copyTo(display_img(cv::Rect(0, 0, path_img.cols, path_img.rows)));
        drawSaveButton(display_img); // 在显示图像上绘制按钮

        cv::imshow("A* Path Planning", display_img);
        int key = cv::waitKey(30);
        if (key == 27)
            break;
    }
    cv::destroyAllWindows();
    cv::waitKey(0);
}