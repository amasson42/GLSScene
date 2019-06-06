
CC := clang++

NAME := GLScene

INCS := includes/
SRCS := sources/
OBJS := objs

SHELL := /bin/bash

INCLUDES := -I$(INCS) -I$(INCS)/GLScene \
			`pkg-config glm --cflags-only-I` \
			`pkg-config assimp --cflags-only-I`

LIBS := -L/usr/lib \
		-framework OpenCL \
		-framework OpenGL \
		`pkg-config --static --libs glm` \
		`pkg-config --static --libs assimp`

CPP_FILES := $(shell find $(SRCS) -type f -name "*.cpp")
CPP_DIRS := $(shell find $(SRCS) -depth -type d)
CPP_FLAGS += -std=c++11 -Wall -Wextra -Werror -g

O_DIRS := $(addprefix $(OBJS)/,$(CPP_DIRS))
O_FILES_TMP := $(CPP_FILES:%.cpp=$(OBJS)/%.o)
O_FILES := $(O_FILES_TMP:%.s=$(OBJS)/%.o)

DEPS := $(O_FILES:.o=.d)

$(shell mkdir -p $(O_DIRS) $(OBJS))

all:
	make $(NAME).a
	make $(NAME).dylib

$(NAME).dylib: $(O_FILES)
	$(CC) -dynamiclib $(CPP_FLAGS) $(INCLUDES) $(LIBS) -o $@ $(O_FILES)

$(NAME).a: $(O_FILES)
	ar rcs $@ $^

$(OBJS)/%.o: %.cpp
	clang $(CPP_FLAGS) $(INCLUDES) -o $@ -c $<

clean:
	rm -fr $(O_FILES) $(OBJS)

fclean: clean
	rm -f $(NAME).dylib
	rm -f $(NAME).a

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
.SILENT:
