#include "model_d.h"
#include "model_factory.h"
#include <iostream>

ModelD::ModelD() {
    registerCommands();
    std::cout << "[ModelD] ModelD Created\n";
}

void ModelD::actioncmdWS(const std::string& content) {
    std::cout << "[ModelD] WS Action Content: " << content << "\n";
}

void ModelD::registerCommands() {
    registerCommand("WS", [this](auto&& content) { actioncmdWS(content); });
}

DEVICE_REGISTER("D", ModelD);