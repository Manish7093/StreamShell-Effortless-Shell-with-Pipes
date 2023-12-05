#include "Execute.h"
#include "Parser.h"
#include "logger.h"
#include "EnvironmentVariablesManager.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <vector>

const char Pipe = '|';

bool shouldStop = false; // Global flag to control the loop execution
void ChildWait()
{
    while (!shouldStop)
    {
        int flag;
        pid_t pid;

        // Wait for any child process to exit without blocking
        pid = waitpid(-1, &flag, WNOHANG);

        if (pid > 0)
        {
            if (flag == 0)
            {
                // Child process exited normally; do not print a message
                return;
            }
            else
            {
                int exitStatus = flag >> 8; // Extract exit status from the status variable
                int signal = flag & 0x7F;    // Extract termination signal from the status variable

                if (signal)
                {
                    Logger::log("Process " + std::to_string(pid) + " terminated by signal: " + std::to_string(signal));
                    return;
                }
                else
                {
                    return;
                }
            }
        }
        else if (pid == 0)
        {
            // No child process has exited
            usleep(10); // Sleep for a short duration to avoid busy waiting
        }
        else if (pid == -1 && errno != ECHILD)
        {
            Logger::error("Error in waitpid");
        }
    }
}

void executeShells(std::vector<std::string> Shells)
{
    // Handle single Shell (no pipes)
    if (Shells.size() == 1)
    {
        // Run the single Shell
        Shell Shell(Shells.front());
        Shell.run();

        // Wait for the single child process to exit
        ChildWait();
        return;
    }

    int pPipe[2] = {-1, -1};
    int cPipe[2] = {-1, -1};

    for (size_t i = 0; i < Shells.size(); ++i)
    {
        // Set up pipes for all Shells except the last one
        if (i < Shells.size() - 1)
        {
            if (pipe(cPipe) == -1)
            {
                Logger::error("Pipe creation failed");
                return;
            }
        }

        pid_t pid = fork();

        if (pid == -1)
        {
            Logger::error("Fork failed");
            return;
        }

        if (pid == 0)
        {
            // Child process

            // Redirect input from the previous Shell's output
            if (pPipe[0] != -1)
            {
                dup2(pPipe[0], STDIN_FILENO);
                close(pPipe[0]);
                close(pPipe[1]);
            }

            // Redirect output to the next Shell's input, except for the last Shell
            if (i < Shells.size() - 1)
            {
                dup2(cPipe[1], STDOUT_FILENO);
                close(cPipe[0]);
                close(cPipe[1]);
            }

            // Close all other pipes in child processes
            for (size_t j = 0; j < Shells.size() - 1; ++j)
            {
                if (j != i)
                {
                    close(pPipe[0]);
                    close(pPipe[1]);
                }
            }

            // Run the Shell
            Shell Shell(Shells[i]);
            Shell.run();

            // Exit child process after Shell execution
            exit(0);
        }
        else
        {
            // Parent process

            // Close previous pipe after it's no longer needed
            if (pPipe[0] != -1)
            {
                close(pPipe[0]);
                close(pPipe[1]);
            }

            // Save current pipe as previous pipe for the next iteration
            if (i < Shells.size() - 1)
            {
                pPipe[0] = cPipe[0];
                pPipe[1] = cPipe[1];
            }

            // Wait for child process to complete
            if (i == Shells.size() - 1)
            {
                // For the last Shell, wait for all children
                ChildWait();
            }
        }
    }
}


int main(int argc, char *argv[])
{
    // Set environment variables
    EnvironmentVariablesManager::setEnvironmentVariable("USER", "Manish");
    EnvironmentVariablesManager::setEnvironmentVariable("HOME", "/home/Manish");

    Logger::log("Shell started");

    while (true)
    {
        // Display prompt and wait for input
        std::cout << "StreamShell$ ";
        std::string rawMultiShell;
        getline(std::cin, rawMultiShell);
        // Replace environment variables in the user input
        rawMultiShell = Parser::replaceEnvironmentVariables(rawMultiShell);

        if (rawMultiShell.empty())
        {
            continue; // Empty input, continue to the next iteration
        }
                // Check for the exit Shell
        if (rawMultiShell == "exit" || rawMultiShell == "quit")
        {
            Logger::log("Exiting shell");
            break; // Exit the loop and close the shell
        }

        try
        {
            // Parse the multi-Shell into a vector of raw Shells (strings)
            std::vector<std::string> Shells = Parser::lex(rawMultiShell, Pipe);
            
            // Execute the parsed Shells
            executeShells(Shells);
        }
        catch (const std::exception &e)
        {
            Logger::error("Exception occurred: " + std::string(e.what()));
        }
        catch (...)
        {
            Logger::error("An unknown error occurred");
        }
    }
     
    Logger::log("Shell exiting");
    return 0;
}
