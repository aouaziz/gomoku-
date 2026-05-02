NAME = gomoku
TEST_NAME = run_tests

# Main game files
SRC = gomoku.cpp Logic.cpp main.cpp
OBJ = $(SRC:.cpp=.o)

# Test files (replaces main.cpp with test.cpp)
TEST_SRC = gomoku.cpp Logic.cpp test.cpp
TEST_OBJ = $(TEST_SRC:.cpp=.o)

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -O3
LDFLAGS  = -lsfml-graphics -lsfml-window -lsfml-system
RM = rm -f

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

# The test target
test: $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $(TEST_OBJ) -o $(TEST_NAME) $(LDFLAGS)
	./$(TEST_NAME)

clean:
	$(RM) $(OBJ) test.o

fclean: clean
	$(RM) $(NAME) $(TEST_NAME)

re: fclean all

.PHONY: all clean fclean re test