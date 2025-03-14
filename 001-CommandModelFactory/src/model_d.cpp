#include "model_d.h"
#include "model_factory.h"
#include <iostream>

void ModelD::actioncmdWS(const std::string& content) {
    std::cout << "[ModelD] WS Action Content: " << content << "\n";
}

void ModelD::registerCommands() {
    ModelA::registerCommands();
    registerCommand("WS", [this](auto&& content) { actioncmdWS(content); });
}

DEVICE_REGISTER("D", ModelD);