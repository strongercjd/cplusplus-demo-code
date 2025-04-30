#pragma once
#include "model_b.h"

class ModelC : public ModelB {
protected:
    void actioncmdQS(const std::string& content);

public:
    ModelC(TestClass& test_code);
    void registerCommands() ;
};