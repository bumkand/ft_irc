# test build for my channel part only. run from here:  make && ./test_channel
# -I . comes first so the MOCK ClientData.hpp wins over ../includes/ClientData.hpp
NAME	= test_channel
CXX		= c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I . -I ../includes

SRCS	= mock_main.cpp ../src/Channel.cpp ../src/ChannelManager.cpp \
		  ../src/ChannelJOIN.cpp ../src/ChannelPART.cpp \
		  ../src/ChannelPRIVMSG.cpp ../src/ChannelQUIT.cpp \
		  ../src/ChannelTOPIC.cpp ../src/ChannelKICK.cpp \
		  ../src/ChannelINVITE.cpp ../src/ChannelMODE.cpp ../src/ChannelMODEflags.cpp ../src/ChannelWHO.cpp

all: $(NAME)

$(NAME): $(SRCS) ClientData.hpp ../includes/Channel.hpp ../includes/Replies.hpp \
		../includes/ChannelManager.hpp
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(NAME)

clean:
	rm -f $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
