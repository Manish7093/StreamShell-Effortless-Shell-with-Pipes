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
	while (true)
	{
		// Waiting to exit child process
		int flag;
		pid_t processid = wait(&flag);

		if (int(processid) <= 0)
		{
			// All children exited
			return;
		}
	      	if (WIFSIGNALED(flag))
		{
			int flag = WTERMSIG(flag);
		
		}
	}
}


void executeShells(vector<string> Input)
{
	//  Non pipe single commands
	if (int(Input.size()) == 1)
	{
		// Single command
		Shell Input_cmd = Shell(Input.front());
		Input_cmd.run();

		// Waiting to exit single child process 
		ChildWait();
		return;
	}

	// pipe Commands execution

	// array to track pipes
	int InputPipe[2];
	int OutputPipe[2];

	// first command pipe setup
	if (pipe(OutputPipe) == -1)
	{
		Logger::error("Pipe creation failed");
	};

	// first command with single output pipe
	Shell FirstCmd = Shell(Input[0], NULL, OutputPipe);
	FirstCmd.run();

	// Reroute the pipes for the next command
	InputPipe[0] = OutputPipe[0];
	InputPipe[1] = OutputPipe[1];

	// use of default STDIN/STDOUT
	for (size_t i = 1; i < Input.size() - 1; i++)
	{
		// pipe setups
		if (pipe(OutputPipe) == -1)
		{
			Logger::error("Pipe creation failed");
		};
		// Run the command
		Shell input = Shell(Input[i], InputPipe, OutputPipe);
		input.run();

		// Change pipes 
		InputPipe[0] = OutputPipe[0];
		InputPipe[1] = OutputPipe[1];
	}

	// End command
	Shell EndCmd = Shell(Input[Input.size() - 1], InputPipe, NULL);
	EndCmd.run();

	// Waiting to complete child after all commands
	ChildWait();
}

int main(int argc, char *argv[])
{
    // Set environment variables
    extern char** environ; // Access the array of environment variables
    for (char** env = environ; *env != nullptr; ++env) 
    {
        std::string envVariable(*env);
        size_t equalSignPos = envVariable.find('=');

        if (equalSignPos != std::string::npos) 
        {
            std::string envName = envVariable.substr(0, equalSignPos);
            std::string envValue = envVariable.substr(equalSignPos + 1);

            EnvironmentVariablesManager::setEnvironmentVariable(envName, envValue);
        }
    }
    
    Logger::log("Shell started");

    while (true)
    {
        // Display prompt and wait for input
        std::cout << "StreamShell$ ";
        std::string rawMultiShell;

        // Handle End Of File
        if(!getline(std::cin, rawMultiShell))
        {
            std::cout<<"\nExiting shell\n";
            break;
        }

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
