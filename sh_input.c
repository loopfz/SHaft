/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Thomas Schaffer <thomas.schaffer@epitech.eu>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include	"main.h"
#include	"sh_input.h"
#include	"sh_input_line_edit.h"
#include	"sh_tree.h"
#include	"sh_token_handlers.h"

char		*next_input_line() {
  static char	sbuf[INPUT_BUFSIZE];
  static int	sidx;
  static int	ret;
  char		dest[INPUT_BUFSIZE + 1];
  int		i;

  i = 0;
  while (i < INPUT_BUFSIZE) {
    while (sidx < ret) {
      if (sbuf[sidx] == '\n') {
	dest[i++] = sbuf[sidx++];
	dest[i] = '\0';
	return strdup(dest);
      }
      dest[i++] = sbuf[sidx++];
    }
    dest[i] = '\0';
    ret = xread(sh.read_cmd_fd, sbuf, INPUT_BUFSIZE);
    sidx = 0;
    if (ret == 0) {
      if (i == 0) {
	sh.interrupt = 1;
	return NULL;
      }
      else
	return strdup(dest);
    }
  }
  dest[i] = '\0';
  return strdup(dest);
}

void		print_stack(state_t *stack, char *buf) {
  if (stack->next != NULL) {
    print_stack(stack->next, buf);
    strcat(buf, " ");
    strcat(buf, stack->str);
  }
  else
    strcpy(buf, stack->str);
}

char		*get_input(char *s, state_t *stack) {
  char		*ret;
  char		*prompt;
  char		buf[1024];

  if (sh.interactive) {
    if (stack != NULL) {
      print_stack(stack, buf);
      strcat(buf, ADDITIONAL_INPUT_REQ_PROMPT);
      prompt = buf;
    }
    else
      prompt = sh.prompt;
    ret = line_edit_input(prompt);
  }
  else
    ret = next_input_line();
  return concat_and_free(s, ret);
}
