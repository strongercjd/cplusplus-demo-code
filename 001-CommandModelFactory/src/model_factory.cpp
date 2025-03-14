#include "model_factory.h"

ModelFactory& ModelFactory::instance() {
    static ModelFactory factory;
    return factory;
}

void ModelFactory::registerModel(const std::string& type, Creator creator) {
    registry_[type] = creator;
}

std::unique_ptr<ModelA> ModelFactory::create(const std::string& type) {
    if (auto it = registry_.find(type); it != registry_.end()) {
        auto model = it->second();
        model->registerCommands();
        return model;
    }
    return nullptr;
}