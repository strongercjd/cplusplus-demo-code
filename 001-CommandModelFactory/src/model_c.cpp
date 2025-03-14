#include "model_c.h"
#include "model_factory.h"
#include <iostream>

void ModelC::actioncmdQS(const std::string& content) {
    std::cout << "[ModelC] QS Action Content: " << content << "\n";
}

void ModelC::registerCommands() {
    ModelB::registerCommands();
    std::cout << "Registering ModelC QS handler\n";
    registerCommand("QS", [this](auto&& content) { 
        std::cout << "DEBUG - Handling QS in ModelC\n";
        actioncmdQS(content); 
    });
    // ...其他注册逻辑
}


DEVICE_REGISTER("C", ModelC);