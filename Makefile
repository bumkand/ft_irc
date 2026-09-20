CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

# source files
SRCS = main.cpp Ircserv.cpp ClientData.cpp
OBJS = $(SRCS:.cpp=.o)
INCLUDES_DIR = includes/

NAME = ircserv

all: $(NAME)

# compile files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I $(INCLUDES_DIR) -c $< -o $@

# create the executable
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re