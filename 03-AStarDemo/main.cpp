#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <set>
#include <algorithm>

using namespace std;

// 定义节点结构体
struct Node {
    int x, y;
    int g, h;// g是从起点到当前节点的实际代价，h是从当前节点到目标节点的估计代价
    Node* parent;// 父节点指针
    
    Node(int _x, int _y) : x(_x), y(_y), g(0), h(0), parent(nullptr) {}
};

// 计算启发函数（曼哈顿距离）
int heuristic(Node* a, Node* b) {
    return abs(a->x - b->x) + abs(a->y - b->y);
}

// 重载<运算符用于优先队列比较
struct CompareNode {
    bool operator()(const Node* a, const Node* b) const {
        return (a->g + a->h) > (b->g + b->h);
    }
};

vector<pair<int, int>> AStar(vector<vector<int>>& grid, 
                            int startX, int startY, 
                            int endX, int endY) {
    int rows = grid.size();
    int cols = grid[0].size();
    
    // 创建起点和终点
    Node* start = new Node(startX, startY);
    Node* end = new Node(endX, endY);
    
    // 初始化启发值
    start->h = heuristic(start, end);
    
    // 开放列表和闭合列表
    priority_queue<Node*, vector<Node*>, CompareNode> openList;
    set<pair<int, int>> closedList;
    
    // 四个方向：上下左右
    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1}; 
    
    openList.push(start);
    
    while (!openList.empty()) {
        Node* current = openList.top();
        openList.pop();
        
        // 将当前节点加入闭合列表
        closedList.insert({current->x, current->y});
        
        // 如果到达目标点
        if (current->x == endX && current->y == endY) {
            vector<pair<int, int>> path;
            while (current != nullptr) {
                path.emplace_back(current->x, current->y);
                current = current->parent;
            }
            reverse(path.begin(), path.end());
            return path;
        }
        
        // 遍历四个方向
        for (int i = 0; i < 4; ++i) {
            int newX = current->x + dx[i];
            int newY = current->y + dy[i];
            
            // 检查边界条件和障碍物
            if (newX >= 0 && newX < cols &&   // X范围是[0,cols-1]
                newY >= 0 && newY < rows &&   // Y范围是[0,rows-1]
                grid[rows - 1 - newY][newX] == 0)  {
                
                Node* neighbor = new Node(newX, newY);
                neighbor->parent = current;
                neighbor->g = current->g + 1;
                neighbor->h = heuristic(neighbor, end);
                
                // 如果邻居在闭合列表中跳过
                if (closedList.find({newX, newY}) != closedList.end()) {
                    delete neighbor;
                    continue;
                }
                
                // 添加到开放列表
                openList.push(neighbor);
            }
        }
    }
    
    // 没有找到路径
    return {};
}

// 测试代码
int main() {
    // 0表示可通行，1表示障碍物
    //对于grid访问，修改为左下角为原点，向右为X正方向，向上为Y的正方向
    // 例如，grid[1][2]表示地图上的(2,3)点
    vector<vector<int>> grid = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 0, 1, 0}
    };
    
    vector<pair<int, int>> path = AStar(grid, 2, 2, 3, 1);
    //打印地图，路径上的点用*表示
    if(path.empty() == false)
    {
        cout << "Map:" << endl;
        for (int i = 0; i < grid.size(); i++) {
            for (int j = 0; j < grid[0].size(); j++) {
                int x = j;
                int y = grid.size() - 1 - i;
                if (grid[i][j] == 1) {
                    cout << "1 ";
                }   
                else if (find(path.begin(), path.end(), make_pair(x, y)) != path.end()) {
                    cout << "* ";
                }   
                else {
                    cout << "0 ";
                }
            }
            cout << endl;
        }
        cout << endl;
    }

    cout << "Path: ";
    for (auto p : path) {
        cout << "(" << p.first << "," << p.second << ") ";
    }
    cout << endl;
    
    return 0;
}