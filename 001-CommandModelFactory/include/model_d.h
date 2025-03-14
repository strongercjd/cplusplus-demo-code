#pragma once
#include "model_a.h"

class ModelD : public ModelA {
protected:
    void actioncmdWS(const std::string& content);

public:
    void registerCommands() override;
};