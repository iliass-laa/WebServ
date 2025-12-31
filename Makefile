CC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98  
# CPPFLAGS =  -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g

# Automatically find all .cpp files in the repository (exclude .git and root/webserv placeholders)
SERVER = $(shell find ./server -name '*.cpp')
PARSE = $(shell find ./parsing -name '*.cpp')
REQERSP = $(shell find ./req-res -name '*.cpp')
SESSION = $(shell find ./session -name '*.cpp')

HEADERS = $(shell find ./server -name '*.hpp') \
		 $(shell find ./session -name '*.hpp') \
		 $(shell find ./req-res -name '*.hpp') \
		 $(shell find ./parsing -name '*.hpp')

SRCS := $(PARSE) $(SERVER) $(REQERSP)  $(SESSION) main.cpp
OBJ := $(SRCS:.cpp=.o)

NAME = webserv

all: $(NAME) Makefile

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) -o $@ $(OBJ)

%.o: %.cpp ${HEADERS}
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re