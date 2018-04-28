
NAME = game
FLAGS = -Wall -Werror -Wextra -O2

all:
	make -C GLScene
	make $(NAME)

install:
	brew update
	brew install pkg-config
	brew install glfw
	brew install glm

$(NAME):
	clang++ \
		$(FLAGS) \
		-lm -framework OpenGL `pkg-config --static --libs glfw3` \
		`pkg-config --static --libs glm` \
		`pkg-config glfw3 --cflags-only-I` \
		`pkg-config glm --cflags-only-I` \
		-I GLScene/includes \
		main.cpp -o game \
		GLScene/GLScene.a

clean:

fclean: clean
	make -C GLScene fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all install clean fclean re
