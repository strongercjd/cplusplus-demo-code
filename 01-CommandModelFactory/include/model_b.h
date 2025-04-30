#pragma once
#include "model_a.h"

class ModelB : public ModelA {
protected:
    void actioncmdER(const std::string& content);

public:
    ModelB(TestClass& test_code);
    void registerCommands() ;
};