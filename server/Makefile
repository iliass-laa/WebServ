CC = c++
CPPFLAGS = -Werror -Wextra -Werror -std=c++98

SRC =  Socket.cpp webserv.cpp EventLoop.cpp Core.cpp Client.cpp
HDRS =  Socket.hpp EventLoop.hpp Core.hpp Client.hpp
OBJ = $(SRC:.cpp=.o)

# SRCCLIN = client.cpp 
# CLIENT_OBJ = $(SRCCLIN:.cpp=.o)

SERVER = webserv
# CLIENT = client

all : $(SERVER)

$(SERVER) : $(OBJ)
	$(CC) $(CPPFLAGS) -o $@ $^

%.o: %.cpp $(HDRS)
	$(CC) $(CPPFLAGS) $< -c -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(SERVER)

re: fclean all

# .PHONY: all clean fclean re