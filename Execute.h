#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include "logger.h"
using namespace std;

class Shell
{
public:
	static const char DELIMITER = ' ';

	Shell(string rawShell);	// constructor
	Shell(string rawShell, int *Input, int *Output); // constructor
	void run();

private:
	string rawShell;
	int *Input;
	int *Output;

	vector<string> tokenize(string rawShell); //function that parses Shell line into strings
	void Implement(vector<string> args);
	void ImplementChild(vector<string> args);
	void setInput(int *input);
	void setOutput(int *output);
    

	// Static helper function
	static char *const *stringVectorToCharArray(vector<string> toConvert);
};

