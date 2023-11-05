#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <ctime>

class Logger
{
public:
    static void log(const std::string &message)
    {
        std::ofstream logFile("shell_log.txt", std::ios::app); // Appending to log file
        if (logFile.is_open())
        {
            logFile << getCurrentTimestamp() << " [INFO] " << message << std::endl;
            std::cout << message << std::endl; // Print to console as well
            logFile.close();
        }
        else
        {
            std::cerr << "Error: Unable to open log file." << std::endl;
        }
    }

    static void error(const std::string &message)
    {
        std::ofstream logFile("shell_log.txt", std::ios::app); // Appending to log file
        if (logFile.is_open())
        {
            logFile << getCurrentTimestamp() << " [ERROR] " << message << std::endl;
            std::cerr << message << std::endl; // Print to standard error as well
            logFile.close();
        }
        else
        {
            std::cerr << "Error: Unable to open log file." << std::endl;
        }
    }

private:
    static std::string getCurrentTimestamp()
    {
        std::time_t now = std::time(0);
        char timestamp[100];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(timestamp);
    }
};

#endif // LOGGER_H
