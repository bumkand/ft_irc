#include "Ircserv.hpp"
#include "ClientData.hpp"

std::string my_tolower(const std::string &str)
{
	std::string result;
	result.resize(str.length());

	for (size_t j = 0; j < str.length(); j++)
	{
		if (str[j] >= 'A'&& str[j] <= '^')
		{
			result[j] = str[j] + ('a' - 'A');
		}
		else
		{
			result[j] = str[j];
		}
	}
	return result;
}

std::string my_toupper(const std::string &str)
{
	std::string result;
	result.resize(str.length());

	for (size_t j = 0; j < str.length(); j++)
	{
		if (str[j] >= 'a'&& str[j] <= '~')
		{
			result[j] = str[j] - ('a' - 'A');
		}
		else
		{
			result[j] = str[j];
		}
	}
	return result;
}

std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> tokens;
    size_t start = 0;
	size_t pos = s.find(delim, start);
    while (pos != std::string::npos)
    {
        tokens.push_back(s.substr(start, pos - start));
        start = pos + 1;
		pos = s.find(delim, start);
    }
    tokens.push_back(s.substr(start));
    return tokens;
}
