#include "EnvironmentVariablesManager.h"

std::unordered_map<std::string, std::string> EnvironmentVariablesManager::environmentVariables;

void EnvironmentVariablesManager::setEnvironmentVariable(const std::string& name, const std::string& value) {
    environmentVariables[name] = value;
}

std::string EnvironmentVariablesManager::getEnvironmentVariable(const std::string& name) {
    auto it = environmentVariables.find(name);
    if (it != environmentVariables.end()) {
        return it->second;
    }
    return "";
}

void EnvironmentVariablesManager::removeEnvironmentVariable(const std::string& name) {
    auto it = environmentVariables.find(name);
    if (it != environmentVariables.end()) {
        environmentVariables.erase(it);
    }
}

const std::unordered_map<std::string, std::string>& EnvironmentVariablesManager::getEnvironmentVariables() {
    return environmentVariables;
}
