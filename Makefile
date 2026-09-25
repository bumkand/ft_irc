CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC_DIR = src
OBJ_DIR = obj
INCLUDES_DIR = includes

# source files
SRCS = main.cpp Ircserv.cpp ClientData.cpp \
		parse.cpp Message.cpp handleCmds.cpp \
		handleNICK.cpp

OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

NAME = ircserv

all: $(NAME)

# compile files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
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