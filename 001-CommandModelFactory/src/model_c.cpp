#include "model_c.h"
#include "model_factory.h"
#include <iostream>


ModelC::ModelC() {
    registerCommands();
    std::cout << "[ModelC] ModelC Created\n";
}
void ModelC::actioncmdQS(const std::string& content) {
    std::cout << "[ModelC] QS Action Content: " << content << "\n";
}

void ModelC::registerCommands() {
    registerCommand("QS", [this](auto&& content) {actioncmdQS(content);});
}


DEVICE_REGISTER("C", ModelC);