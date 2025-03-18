#include "model_b.h"
#include "model_factory.h"
#include <iostream>


ModelB::ModelB() 
{
    registerCommands();
    std::cout << "[ModelB] Created\n";
}
void ModelB::actioncmdER(const std::string& content) { 
    std::cout << "[ModelB] ER Action Content: " << content << "\n";
}

void ModelB::registerCommands() {
    registerCommand("ER", [this](auto&& content) { actioncmdER(content); });
}

DEVICE_REGISTER("B", ModelB);