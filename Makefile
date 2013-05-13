## The MIT License (MIT)
##
## Copyright (c) 2013 Thomas Schaffer <thomas.schaffer@epitech.eu>
##
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
##
## The above copyright notice and this permission notice shall be included in
## all copies or substantial portions of the Software.
##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
## THE SOFTWARE.

NAME = SHaft

SRC = main.c			\
      xfunc.c			\
      sh_error.c		\
      sh_opt.c			\
      sh_init.c			\
      sh_get_set_env.c		\
      sh_env_init.c		\
      sh_interactive.c		\
      read_cmd.c		\
      sh_input.c		\
      sh_input_line_edit.c	\
      sh_exec_handlers.c	\
      parse_input.c		\
      sh_redirs_handlers.c	\
      sh_token_handlers.c	\
      sh_list.c			\
      sh_local_vars.c		\
      sh_func.c			\
      deep_copy_free.c		\
      built_ins.c		\
      alias.c			\
      export.c			\
      unset.c			\
      exit.c			\
      change_dir.c		\
      echo.c

OBJ = $(SRC:.c=.o)

CC = gcc

CFLAGS = -W -Wall -g

LDFLAGS =

RM = rm -f

all: $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
