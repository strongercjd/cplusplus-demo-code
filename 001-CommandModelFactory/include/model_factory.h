#pragma once
#include <memory>
#include <functional>
#include "model_a.h"


class ModelFactory {
public:
    using Creator = std::function<std::unique_ptr<ModelA>()>;
    
    static ModelFactory& instance();
    void registerModel(const std::string& type, Creator creator);
    std::unique_ptr<ModelA> create(const std::string& type);

private:
    std::unordered_map<std::string, Creator> registry_;
};

#define DEVICE_REGISTER(type, model) \
namespace { \
struct AutoRegister##model { \
    AutoRegister##model() { \
        ModelFactory::instance().registerModel(type, []{ return std::make_unique<model>(); }); \
    } \
} static_registrar_##model; \
}
