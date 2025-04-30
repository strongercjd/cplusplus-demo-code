#include "model_b.h"
#include "model_factory.h"
#include <iostream>

#include "testclass.h"


ModelB::ModelB(TestClass& test_code):ModelA(test_code)
{
    registerCommands();
    std::cout << "[ModelB] Created\n";
    testclass.printfself();
}
void ModelB::actioncmdER(const std::string& content) { 
    std::cout << "[ModelB] ER Action Content: " << content << "\n";
}

void ModelB::registerCommands() {
    registerCommand("ER", [this](auto&& content) { actioncmdER(content); });
}

DEVICE_REGISTER("B", ModelB);