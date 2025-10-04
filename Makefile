
CC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++11 -I. -Iheaders -Iincludes -Iparsing -Iparsing/configParse -Iparsing/configTools -Ireq-res -Iserver

# Automatically find all .cpp files in the repository (exclude .git and root/webserv placeholders)
SRCS := $(shell find . -name '*.cpp' -not -path './.git/*' -not -path './webserv/*' | sort)
OBJ := $(SRCS:.cpp=.o)

NAME = webserv

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
