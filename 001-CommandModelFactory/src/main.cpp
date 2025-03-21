#include <iostream>
#include <cctype>
#include "model_factory.h"

#include "testclass.h"

class MainClass
{
public:
    MainClass(){}
    ~MainClass(){}

    TestClass test_code;

    void processCommands();

};


void MainClass::processCommands() 
{
    std::string input;
    while (true) {
        
        auto&& supported_types = ModelFactory::instance().getRegisteredTypes();
            std::cout << "support modes: ";
            for (const auto& type : supported_types) {
                std::cout << type << " ";
        }
        std::cout << std::endl;

        std::cout << "Enter command (@[A-D][AA/WS/QS/ER][00-99]*): ";
        std::cin >> input;
        
        if (input == "0") break;

        // 格式验证
        if (input.size() < 5 || input[0] != '@' || input.back() != '*') {
            std::cout << "Invalid frame format\n";
            continue;
        }

        // 解析命令
        const char model_char = toupper(input[1]);
        const std::string model_type(1, model_char);
        const std::string cmd = input.substr(2, 2);
        const size_t star_pos = input.find('*', 3);
        const std::string content = (star_pos != std::string::npos) ? 
            input.substr(4, star_pos - 4) : "";

        // 创建对应型号
        auto model = ModelFactory::instance().create(model_type,test_code);
        if (!model) {
            std::cout << "Invalid model: " << model_char << "\n";
            continue;
        }

        // 处理命令
        model->handleCommand(cmd, content);
    }
}

int main() {
    MainClass test;
    test.processCommands();
    return 0;
}