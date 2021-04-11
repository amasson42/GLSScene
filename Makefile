
UNAME_S := $(shell uname -s)

NAME = game

CC = g++

INCS = includes/
SRCS = sources/
OBJS = objs


SHELL := /bin/bash

INCLUDES =	-I $(INCS) \
			-I $(INCS)/CustomSceneControllers/ \
			-I $(INCS)/CustomSceneControllers/VoxelProcedural/ \
		-I GLScene/includes \
		-I GLScene/CLDevice/includes \
		-I libs/nanogui/include \
		-I libs/nlohmannjson \
		\
		-I libs/nanogui/ext/eigen \
		-I libs/nanogui/ext/nanovg/src/ \
		\


LIBS = -lm \
		`pkg-config --static --libs glfw3` \
		`pkg-config --static --libs glm` \
		`pkg-config --static --libs assimp` \
		`pkg-config glfw3 --cflags-only-I` \
		`pkg-config glm --cflags-only-I` \
		`pkg-config assimp --cflags-only-I` \
		-L GLScene -lGLScene \
		-L libs/nanogui/build -lnanogui \

ifeq ($(UNAME_S),Linux)
	LIBS += -L/usr/lib \
			-lOpenCL \
			-lOpenGL \

endif
ifeq ($(UNAME_S),Darwin)
	LIBS += -L/usr/lib \
			-framework OpenCL \
			-framework OpenGL \

endif

CPP_FILES = $(shell find $(SRCS) -type f -name *.cpp)
CPP_DIRS = $(shell find $(SRCS) -depth -type d)
CPP_FLAGS += -std=c++17 -Wall -Wextra -Werror

ifeq ($(release), )
	CPP_FLAGS += -g -DDEBUG
else
	CPP_FLAGS += -O3 -DNDEBUG
endif

O_DIRS = $(addprefix $(OBJS)/,$(CPP_DIRS))
O_FILES_TMP = $(CPP_FILES:%.cpp=$(OBJS)/%.o)
O_FILES = $(O_FILES_TMP:%.s=$(OBJS)/%.o)

DEPS = $(O_FILES:.o=.d)

$(shell mkdir -p $(O_DIRS) $(OBJS))

# Print
ACTUAL = $(words $(COUNT))
TOTAL = $(words $(filter %.cpp,$(CPP_FILES)))
INCREMENT = $(eval COUNT+=1)

PRINTINC = $(INCREMENT) $(PRINT)
PRINT = printf "\r \033[31;1m%3s%%\033[0m\t -->>\t\033[31;1m%-65s\033[0m\r" "$(shell echo $(ACTUAL)\*100\/$(TOTAL) | bc)" "$^"
PRINTDONE = printf "\r \033[32;1m%3s%%\033[0m\t -->>\t\033[32;1m%-65s\033[0m\r" "$(shell echo $(ACTUAL)\*100\/$(TOTAL) | bc)" "$@"

all: $(NAME)

run: all
	./$(NAME)

install:
	brew update
	brew install pkg-config || brew upgrade pkg-config
	brew install glfw || brew upgrade glfw
	brew install glm || brew upgrade glm
	brew install assimp || brew upgrade assimp

GLScene:
	@make -C GLScene

$(NAME): GLScene libs/nanogui/build/libnanogui.a libs/nlohmannjson $(O_FILES)
	@$(CC) $(CPP_FLAGS) \
		$(INCLUDES) \
		$(LIBS) \
		-o $@ $(O_FILES) && $(PRINTDONE) && echo || exit

$(OBJS)/%.o: %.cpp
	@$(CC) $(CPP_FLAGS) \
		$(INCLUDES) \
		-o $@ -c $< && $(PRINTINC) || exit

clean:
	rm -fr $(O_FILES) $(OBJS)
	make -C GLScene clean

fclean: clean
	make -C GLScene fclean
	rm -f $(NAME)
	rm -fr $(NAME).app
	rm -fr build
	rm -fr libs

re: fclean all

app: all
	mkdir -p $(NAME).app/Contents/MacOS
	mkdir -p $(NAME).app/Contents/Resources
	cp $(NAME) $(NAME).app/Contents/MacOS/game 2> /dev/null
	cp Info.plist $(NAME).app/Contents
	cp -r assets $(NAME).app/Contents/Resources

libs:
	@mkdir libs || :

libs/nanogui: libs
	@if [ ! -d $@ ]; then \
		git clone --recursive https://github.com/wjakob/nanogui.git libs/nanogui ; \
	fi
	@touch $@

libs/nanogui/build: libs/nanogui
	@mkdir libs/nanogui/build 2> /dev/null || :

libs/nanogui/build/Makefile: libs/nanogui/build
	@if [ ! -f $@ ]; then \
		cd libs/nanogui/build && cmake -DNANOGUI_BUILD_SHARED=OFF -DNANOGUI_BUILD_PYTHON=OFF -DNANOGUI_BUILD_EXAMPLE=OFF .. ; \
	fi
	@touch $@

libs/nanogui/build/libnanogui.a: libs/nanogui/build/Makefile
	@if [ ! -f $@ ]; then \
		make -j4 -C libs/nanogui/build ; \
	fi
	@touch $@

libs/nlohmannjson: libs
	@if [ ! -d $@ ]; then \
		cd libs && \
			wget https://github.com/nlohmann/json/releases/download/v3.7.0/include.zip && \
			unzip -q include.zip -d . && \
			mv include nlohmannjson && \
			rm include.zip ; \
		fi
	@touch $@

-include $(DEPS)

.PHONY: all GLScene install clean fclean re app
