NAME = gomoku

SRC = main.cpp gomoku.cpp Logic.cpp

OBJ = $(SRC:.cpp=.o)
CXX = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++20 -O3
LDFLAGS  = -lsfml-graphics -lsfml-window -lsfml-system
RM = rm -f


all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CPPFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)