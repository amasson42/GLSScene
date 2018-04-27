
CC := clang++

NAME := GLScene

INCS := includes/
SRCS := sources/
OBJS := objs

SHELL := /bin/bash

INCLUDES := -I$(INCS) -I$(INCS)/GLScene

LIBS := -L/usr/lib \
		-framework OpenGL

CPP_FILES := $(shell find $(SRCS) -type f -name "*.cpp")
CPP_DIRS := $(shell find $(SRCS) -depth -type d)
CPP_FLAGS += -Wall -Wextra -Werror -O2

O_DIRS := $(addprefix $(OBJS)/,$(CPP_DIRS))
O_FILES_TMP := $(CPP_FILES:%.cpp=$(OBJS)/%.o)
O_FILES := $(O_FILES_TMP:%.s=$(OBJS)/%.o)

DEPS := $(O_FILES:.o=.d)

$(shell mkdir -p $(O_DIRS) $(OBJS))

all:
	make $(NAME)

$(NAME): $(O_FILES)
	$(CC) -dynamiclib $(CPP_FLAGS) $(INCLUDES) $(LIBS) -o $@.dylib $(O_FILES)

$(OBJS)/%.o: %.cpp
	clang $(CPP_FLAGS) $(INCLUDES) -o $@ -c $<

clean:
	rm -fr $(O_FILES) $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
.SILENT:
