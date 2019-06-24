
CC := clang++

NAME := GLScene

INCS := includes/
SRCS := sources/
OBJS := objs

SHELL := /bin/bash

INCLUDES := -I$(INCS) -I$(INCS)/GLScene \
			`pkg-config glm --cflags-only-I` \
			`pkg-config assimp --cflags-only-I` \
			-ICLDevice/includes \

LIBS := -L/usr/lib \
		-framework OpenCL \
		-framework OpenGL \
		`pkg-config --static --libs glm` \
		`pkg-config --static --libs assimp` \
		-L CLDevice -lCLDevice

CPP_FILES := $(shell find $(SRCS) -type f -name "*.cpp")
CPP_DIRS := $(shell find $(SRCS) -depth -type d)
CPP_FLAGS += -std=c++11 -Wall -Wextra -Werror

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

all:
	make -C CLDevice
	make lib$(NAME).a
	make $(NAME).dylib

$(NAME).dylib: $(O_FILES)
	@$(CC) -dynamiclib $(CPP_FLAGS) $(INCLUDES) $(LIBS) -o $@ $(O_FILES)

lib$(NAME).a: $(O_FILES)
	ar rcs tmp_$@ $^
	mkdir tmp_libmerge
	cd tmp_libmerge \
		&& ar -x ../tmp_$@ \
		&& ar -x ../CLDevice/libCLDevice.a \
		&& ar rcs ../$@ *.o
	rm -fr tmp_libmerge
	rm -fr tmp_$@
	$(PRINTDONE) && echo || exit

$(OBJS)/%.o: %.cpp
	clang $(CPP_FLAGS) $(INCLUDES) -o $@ -c $< && $(PRINTINC) || exit

clean:
	rm -fr $(OBJS)
	make -C CLDevice clean
	rm -fr tmp_*

fclean: clean
	rm -f $(NAME).dylib
	rm -f lib$(NAME).a
	make -C CLDevice fclean

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re lib$(NAME).a
