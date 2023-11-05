#ifndef ENVIRONMENT_VARIABLES_MANAGER_H
#define ENVIRONMENT_VARIABLES_MANAGER_H

#include <string>
#include <unordered_map>

class EnvironmentVariablesManager {
public:
    static void setEnvironmentVariable(const std::string& name, const std::string& value);
    static std::string getEnvironmentVariable(const std::string& name);
    static void removeEnvironmentVariable(const std::string& name);
private:
    static std::unordered_map<std::string, std::string> environmentVariables;
};

#endif // ENVIRONMENT_VARIABLES_MANAGER_H

