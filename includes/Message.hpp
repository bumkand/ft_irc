#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <iostream>
# include <string>
#include <vector>

class Message
{
	public:
		Message();
		Message(const Message &copy);
		~Message();
		Message & operator=(const Message &assign);

		const std::string& getPrefix() const;
		const std::string& getCommand() const;
		const std::string& getParam(size_t i) const;
		size_t		getParamSize() const;

		void setPrefix(const std::string &prefix);
		void setCommand(const std::string &command);
		void pushBackParam(const std::string &param);
		
	private:
		std::string					_prefix;
		std::string					_command;
		std::vector<std::string>	_params;
		
};

#endif