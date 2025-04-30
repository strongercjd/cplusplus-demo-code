#pragma once
#include "model_a.h"

class ModelD : public ModelA {
protected:
    void actioncmdWS(const std::string& content);

public:
    ModelD(TestClass& test_code);
    void registerCommands() ;
};