
UNAME_S := $(shell uname -s)

CC := g++

NAME := CLDevice
NAME_STATIC := lib$(NAME).a
NAME_DYNAMIC := $(NAME).dylib

INCS := includes/
SRCS := sources/
OBJS := objs
EXAMPLES := $(shell find examples/ -type d -execdir echo '{}' ';')

SHELL := /bin/bash

INCLUDES := -I$(INCS)

ifeq ($(UNAME_S),Linux)
	LIBS := -L/usr/lib \
			-lOpenCL \
			-lOpenGL
endif
ifeq ($(UNAME_S),Darwin)
	LIBS := -L/usr/lib \
			-framework OpenCL \
			-framework OpenGL
endif

CPP_FILES := $(shell find $(SRCS) -type f -name "*.cpp")
CPP_DIRS := $(shell find $(SRCS) -depth -type d)
CPP_FLAGS = -std=c++11 -Wall -Wextra -Werror

ifeq ($(release), )
	CPP_FLAGS += -g -DDEBUG
else
	CPP_FLAGS += -O3 -DNDEBUG
endif

O_DIRS := $(addprefix $(OBJS)/,$(CPP_DIRS))
O_FILES_TMP := $(CPP_FILES:%.cpp=$(OBJS)/%.o)
O_FILES := $(O_FILES_TMP:%.s=$(OBJS)/%.o)

DEPS := $(O_FILES:.o=.d)

$(shell mkdir -p $(O_DIRS) $(OBJS))

# Print
ACTUAL = $(words $(COUNT))
TOTAL = $(words $(filter %.cpp,$(CPP_FILES)))
INCREMENT = $(eval COUNT+=1)

PRINTINC = $(INCREMENT) $(PRINT)
PRINT = printf "\r \033[31;1m%3s%%\033[0m\t -->>\t\033[31;1m%-65s\033[0m\r" "$(shell echo $(ACTUAL)\*100\/$(TOTAL) | bc)" "$^"
PRINTDONE = printf "\r \033[32;1m%3s%%\033[0m\t -->>\t\033[32;1m%-65s\033[0m\r" "$(shell echo $(ACTUAL)\*100\/$(TOTAL) | bc)" "$@"

all: $(NAME_STATIC)

$(EXAMPLES): $(NAME_STATIC)
	$(CC) examples/$@/*.cpp $(INCLUDES) -L. -l$(NAME) $(LIBS) -l$(NAME) -o $@

$(NAME_DYNAMIC): $(O_FILES)
	@$(CC) -dynamiclib $(CPP_FLAGS) $(INCLUDES) $(LIBS) -o $@ $(O_FILES)

$(NAME_STATIC): $(O_FILES)
	@ar rcs $@ $^
	@$(PRINTDONE) && echo || exit

$(OBJS)/%.o: %.cpp
	@$(CC) $(CPP_FLAGS) $(INCLUDES) -o $@ -c $< && $(PRINTINC) || exit

clean:
	rm -fr $(OBJS)
	rm -fr openlib

fclean: clean
	rm -f $(NAME_DYNAMIC)
	rm -f $(NAME_STATIC)
	rm -f $(EXAMPLES)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
