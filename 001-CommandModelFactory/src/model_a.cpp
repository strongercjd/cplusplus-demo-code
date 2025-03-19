#include "model_a.h"
#include <iostream>
#include "model_factory.h"

#include "testclass.h"

void ModelA::registerCommand(const std::string& cmd, std::function<void(const std::string&)> handler) {
    command_handlers[cmd] = handler;
}

void ModelA::actioncmdAA(const std::string& content) { 
    std::cout << "[ModelA] AA Action Content: " << content << "\n"; 
}

void ModelA::actioncmdWS(const std::string& content) {
    std::cout << "[ModelA] WS Action Content: " << content << "\n";
}

void ModelA::actioncmdQS(const std::string& content) {
    std::cout << "[ModelA] QS Action Content: " << content << "\n";
}

void ModelA::actioncmdER(const std::string& content) {
    std::cout << "[ModelA] ER Action Content: " << content << "\n";
}

ModelA::ModelA(TestClass& test_code):testclass(test_code)
{
    registerCommands(); 
    std::cout << "[ModelA] created\n";
    testclass.printfself();
}


void ModelA::registerCommands() {
    registerCommand("AA", [this](auto&& content) { actioncmdAA(content); });
    registerCommand("WS", [this](auto&& content) { actioncmdWS(content); });
    registerCommand("QS", [this](auto&& content) { actioncmdQS(content); });
    registerCommand("ER", [this](auto&& content) { actioncmdER(content); });
}

void ModelA::handleCommand(const std::string& cmd, const std::string& content) {
    if (auto it = command_handlers.find(cmd); it != command_handlers.end()) {
        it->second(content);
    } else {
        std::cout << "Unknown command: " << cmd << "\n";
    }
}

DEVICE_REGISTER("A", ModelA);
