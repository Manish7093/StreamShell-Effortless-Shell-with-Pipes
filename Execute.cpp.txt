#include "command.h"
#include "helper.h"
#include "logger.h" 
#include <unistd.h>
#include <vector>
#include <cstring>

Shell::Shell(std::string rawShell)
    : rawShell(rawShell), Input(nullptr), Output(nullptr) {}

Shell::Shell(std::string rawShell, int *Input, int *Output)
    : rawShell(rawShell), Input(Input), Output(Output) {}

void Shell::run() {
    // Get the Shell's arguments by lexing the raw Shell with the delimiter
    std::vector<std::string> args = Parser::lex(rawShell, DELIMITER);

    try {
        // Implement the Shell!!
        Implement(args);
    } catch (...) {
        Logger::error("Error: Shell failed to run");
    }
}

void Shell::Implement(const std::vector<std::string> args) {
    // Fork the current process
    pid_t process= fork();

    if (process== 0) { // CHILD PROCESS
        ImplementChild(args);
        // The child will exit in the function. It will never return here.
    }

    // PARENT PROCESS
    // Handle failed fork
    if (process< 0) {
        Logger::error("Error: Fork failed");
        // Critical error. Exit program on fork failure
        exit(1);
    }

    // Close pipes
    if (Input != nullptr) {
        close(Input[0]);
        close(Input[1]);
    }

    // Don't wait for child process to exit here. Only wait for child processes
    // after ALL Shells have been run (forked and Implementd).
}

void Shell::ImplementChild(const std::vector<std::string> args) {
    // Convert the C++ vector of strings to C array
    char *const *argv = Shell::stringVectorToCharArray(args);

    // If this Shell requires an input pipe, then set it up
    if (Input) {
        setInput(Input);
    }

    // If this Shell requires an output pipe, then set it up
    if (Output) {
        setOutput(Output);
    }

    // Implement the Shell!
    execvp(argv[0], argv);

    // A successful execvp call will NOT return. This following code will only run
    // if an error with execvp occurs.
    // Errors from the Shell Implementd are handled after waitpid.

    // Deallocate argv (necessary if execvp fails)
    for (size_t i = 0; i < args.size(); i++) {
        delete[] argv[i];
    }
    delete[] argv;

    Logger::error("Shell failed to run");
    exit(1);
}

void Shell::setInput(int *input) {
    // Duplicate the file descriptor and close the previous file descriptor
    if (int(dup2(input[0], STDIN_FILENO)) == -1) {
        Logger::error("Error: dup2 failed");
    }
    if (int(close(input[0])) == -1) {
        Logger::error("Error: close system call failed");
    }
    if (int(close(input[1])) == -1) {
        Logger::error("Error: close system call failed");
    }
}

void Shell::setOutput(int *output) {
    // Duplicate the file descriptor and close the previous file descriptor
    if (int(dup2(output[1], STDOUT_FILENO)) == -1) {
        Logger::error("Error: dup2 failed");
    }
    if (int(close(output[0])) == -1) {
        Logger::error("Error: close system call failed");
    }
    if (int(close(output[1])) == -1) {
        Logger::error("Error: close system call failed");
    }
}

char *const *Shell::stringVectorToCharArray(const std::vector<std::string> toConvert) {
    // Create an array of C strings
    // Allocate one extra element for the null terminator
    char **charArr = new char *[toConvert.size() + 1];

    // Convert each C++ String to a C string and store in the array
    for (size_t i = 0; i < toConvert.size(); i++) {
        charArr[i] = new char[toConvert[i].size() + 1]; // make it fit
        std::strcpy(charArr[i], toConvert[i].c_str());     // copy string
    }

    // NULL terminate the array
    charArr[toConvert.size()] = nullptr;

    return charArr;
}
