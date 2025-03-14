#pragma once
#include "model_b.h"

class ModelC : public ModelB {
protected:
    void actioncmdQS(const std::string& content);

public:
    void registerCommands() override;
};