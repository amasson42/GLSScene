
NAME = game

CC = clang++

SRCS = sources
OBJS = objs
INCS = includes

FRAMEWORKS = -lm \
		-framework OpenGL \
		-framework OpenCL \
		`pkg-config --static --libs glfw3` \
		`pkg-config --static --libs glm` \
		`pkg-config --static --libs assimp` \
		`pkg-config glfw3 --cflags-only-I` \
		`pkg-config glm --cflags-only-I` \
		`pkg-config assimp --cflags-only-I` \
		-L GLScene -lGLScene

INCLUDES =	-I $(INCS) \
		-I GLScene/includes \
		-I GLScene/CLDevice/includes \

FLAGS = -std=c++11 -Wall -Werror -Wextra

CPP_FILES = $(shell find $(SRCS) -type f -name *.cpp)
CPP_DIRS = $(shell find $(SRCS) -depth -type d)

O_DIRS = $(addprefix $(OBJS)/,$(CPP_DIRS))
O_FILES_TMP = $(CPP_FILES:%.cpp=$(OBJS)/%.o)
O_FILES = $(O_FILES_TMP:%.s=$(OBJS)/%.o)

DEPS = $(O_FILES:.o=.d)

# Print
ACTUAL = $(words $(COUNT))
TOTAL = $(words $(filter %.cpp,$(CPP_FILES)))
INCREMENT = $(eval COUNT+=1)

PRINTINC = $(INCREMENT) $(PRINT)
PRINT = printf "\r \033[31;1m%3s%%\033[0m\t -->>\t\033[31;1m%-65s\033[0m\r" "$(shell echo $(ACTUAL)\*100\/$(TOTAL) | bc)" "$^"
PRINTDONE = printf "\r \033[32;1m%3s%%\033[0m\t -->>\t\033[32;1m%-65s\033[0m\r" "$(shell echo $(ACTUAL)\*100\/$(TOTAL) | bc)" "$@"

all:
	mkdir -p $(O_DIRS) $(OBJS)
	make $(NAME)

install:
	brew update
	brew install pkg-config || brew upgrade pkg-config
	brew install glfw || brew upgrade glfw
	brew install glm || brew upgrade glm
	brew install assimp || brew upgrade assimp

GLScene:
	make -C GLScene

$(NAME): GLScene $(O_FILES)
	$(CC) $(FLAGS) \
		$(INCLUDES) \
		$(FRAMEWORKS) \
		-o $@ $(O_FILES) && $(PRINTDONE) && echo || exit

$(OBJS)/%.o: %.cpp
	$(CC) $(FLAGS) \
		$(INCLUDES) \
		-o $@ -c $< && $(PRINTINC) || exit

clean:
	rm -fr $(O_FILES) $(OBJS)

fclean: clean
	make -C GLScene fclean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all GLScene $(NAME) install clean fclean re
.SILENT:
