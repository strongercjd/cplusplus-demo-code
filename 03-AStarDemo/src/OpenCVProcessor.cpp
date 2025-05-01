#include "OpenCVProcessor.hpp"

void OpenCVProcessor::drawSaveButton(cv::Mat &img)
{
    // 在图像右下角绘制按钮
    cv::Rect buttonRect(img.cols - 120, 10, 110, 30);
    cv::rectangle(img, buttonRect, cv::Scalar(200, 200, 250), cv::FILLED);
    cv::putText(img, "Save Map", cv::Point(img.cols - 110, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 0), 2);
}
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
    cv::Mat ui_img(map_grid.height * cell_size,
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

    ui_img = map_img.clone();// 克隆图像以便绘制按钮

    drawSaveButton(ui_img);

    // 显示图像
    cv::imshow("A* Path Planning", ui_img);

    cv::namedWindow("A* Path Planning");
    cv::setMouseCallback("A* Path Planning", saveButtonHandler, &map_img);

    // 修改显示循环
    while(true) {
        cv::imshow("A* Path Planning", ui_img);
        int key = cv::waitKey(30);
        if(key == 27) { // ESC键退出
            break;
        }
    }
    cv::destroyAllWindows();
    cv::waitKey(0);
}