CC = c++
CPPFLAGS =  -fsanitize=address -g 

# Automatically find all .cpp files in the repository (exclude .git and root/webserv placeholders)
SERVER = $(shell find ./server -name '*.cpp' | sort)
PARSE = $(shell find ./parsing -name '*.cpp' | sort)
REQERSP = $(shell find ./req-res -name '*.cpp' | sort)

SRCS := $(PARSE) $(SERVER) $(REQERSP) main.cpp
# SRCS := $(SERVER) main.cpp
# SRCS := $(PARSE) main.cpp
OBJ := $(SRCS:.cpp=.o)

NAME = webserv

all: $(NAME) Makefile

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