
NAME = game

CC = clang++
FLAGS = -std=c++11 -Wall -Werror -Wextra -g
SRCS = *.cpp

all:
	make -C GLScene
	make $(NAME)

install:
	brew update
	brew install pkg-config || brew upgrade pkg-config
	brew install glfw || brew upgrade glfw
	brew install glm || brew upgrade glm
	brew install assimp || brew upgrade assimp

$(NAME):
	$(CC) $(FLAGS) \
		-lm -framework OpenGL \
		`pkg-config --static --libs glfw3` \
		`pkg-config --static --libs glm` \
		`pkg-config --static --libs assimp` \
		`pkg-config glfw3 --cflags-only-I` \
		`pkg-config glm --cflags-only-I` \
		`pkg-config assimp --cflags-only-I` \
		-I GLScene/includes \
		$(SRCS) -o game \
		GLScene/GLScene.a

clean:
	rm -fr $(NAME).dSYM

fclean: clean
	make -C GLScene fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all $(NAME) install clean fclean re
