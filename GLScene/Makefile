# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: igomez <igomez@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2014/12/25 12:40:20 by igomez            #+#    #+#              #
#    Updated: 2018/03/21 12:52:50 by arthur           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC := clang++

NAME := GLScene

INCS := includes
SRCS := sources
OBJS := objs

SHELL := /bin/bash

INCLUDES := -I$(INCS)\
			`pkg-config glfw3 --cflags-only-I` \
			`pkg-config glew --cflags-only-I`

LIBS := -L/usr/lib \
		-framework OpenGL \
		`pkg-config --static --libs glfw3` \
		`pkg-config --static --libs glew` \
		-lreadline

# Search source files
C_FILES := $(shell find $(SRCS) -type f -name *.cpp)
C_DIRS := $(shell find $(SRCS) -depth -type d)
C_FLAGS += -MMD -std=c99 -g -Wall -Wextra -Werror -O2 -std=c++11 -Wc++11-extensions

# Build .o list
O_DIRS := $(addprefix $(OBJS)/,$(C_DIRS))
O_FILES_TMP := $(C_FILES:%.cpp=$(OBJS)/%.o)
O_FILES := $(O_FILES_TMP:%.s=$(OBJS)/%.o)

DEPS := $(O_FILES:.o=.d)

# Create OBJS and childs
$(shell mkdir -p $(O_DIRS) $(OBJS))

# Print
ACTUAL = $(words $(COUNT))
TOTAL = $(words $(filter %.c,$(C_FILES)))
INCREMENT = $(eval COUNT+=1)

PRINTINC = $(INCREMENT) $(PRINT)
PRINT = printf "\r \033[31;1m%3s%%\033[0m\t -->>\t\033[31;1m%-65s\033[0m\r" "$(shell echo $(ACTUAL)\*100\/$(TOTAL) | bc)" "$^"
PRINTDONE = printf "\r \033[32;1m%3s%%\033[0m\t -->>\t\033[32;1m%-65s\033[0m\r" "$(shell echo $(ACTUAL)\*100\/$(TOTAL) | bc)" "$@"

all:
	@make $(NAME)

$(NAME): $(O_FILES)
	@make -C $(LIB)
	@make -C $(LIG)
	@make -C $(LIX)
	@$(CC) $(C_FLAGS) $(INCLUDES) $(LIBS) -o $@ $(O_FILES) $(FMLX) && $(PRINTDONE) && echo || exit

$(OBJS)/%.o: %.c
	@$(CC) $(C_FLAGS) $(INCLUDES) -o $@ -c $< && $(PRINTINC) || exit

clean:
	@rm -rf $(O_FILES) $(OBJS)

fclean: clean
	@make -C $(LIB) fclean
	@make -C $(LIG) fclean
	@make -C $(LIX) clean
	@rm -f $(NAME)

re: fclean all

test: all
	./$(NAME)

-include $(DEPS)

.PHONY: all clean fclean re
.SILENT:
