
NAME = game
FLAGS = -Wall -Werror -Wextra -O2

all: $(NAME)

install:
	brew update
	brew install pkg-config
	brew install glfw

$(NAME):
	clang++ \
		$(FLAGS) \
		-lm -framework OpenGL `pkg-config --static --libs glfw3` \
		`pkg-config glfw3 --cflags-only-I` \
		-I GLScene/includes \
		main.cpp -o game

clean:

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all install clean fclean re
