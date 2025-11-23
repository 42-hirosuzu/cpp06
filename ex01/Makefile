NAME := ex01
SRC_DIR := src
OBJ_DIR := build
LOG_DIR := log

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -pedantic
# CXXFLAGS += -fsanitize=address
CPPFLAGS := -I$(SRC_DIR)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

log: $(LOG_DIR)/merge_file.sh
	cd $(LOG_DIR) && bash ./merge_file.sh

.PHONY: all clean fclean re log
