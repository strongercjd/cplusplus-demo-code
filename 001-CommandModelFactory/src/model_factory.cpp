#include "model_factory.h"

ModelFactory& ModelFactory::instance() {
    static ModelFactory factory;
    return factory;
}

void ModelFactory::registerModel(const std::string& type, Creator creator) {
    registry_[type] = creator;
}

std::unique_ptr<ModelA> ModelFactory::create(const std::string& type,TestClass& test_code) 
{
    if (auto it = registry_.find(type); it != registry_.end()) {
        auto model = it->second(test_code);
        return model;
    }
    return nullptr;
}