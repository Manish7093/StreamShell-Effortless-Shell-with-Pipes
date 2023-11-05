#include <string>
#include <vector>

using namespace std;

namespace Parser
{
    constexpr char DOUBLE_QUOTE = '"';
    constexpr char SINGLE_QUOTE = '\'';
    constexpr char ESCAPE_CHAR = '\\';

	string trimStr(string str);
	vector<string> lex(string rawMultiCommand, char delimiter);
	std::string replaceEnvironmentVariables(const std::string& input);
}