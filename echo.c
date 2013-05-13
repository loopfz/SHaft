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

#include	<unistd.h>
#include	"main.h"
#include	"echo.h"

static struct {
  char *seq;
  char repl;
} replacements[] = {
  { "\\t", '\t' },
  { "\\n", '\n' },
  { "\\a", '\a' },
  { "\\b", '\b' },
  { "\\e", '\e' },
  { "\\f", '\f' },
  { "\\r", '\r' },
  { "\\v", '\v' },
  { NULL, '\0' }
};

void		echo_getopt(char **argv, int *i, bool *nl, bool *expand) {
  int		j;
  bool		match;

  *nl = 0;
  *expand = 0;
  for (*i = 1; argv[*i] != NULL && argv[*i][0] == '-'; (*i)++) {
    match = 0;
    for (j = 1; argv[*i][j] != '\0'; j++) {
      match = 1;
      if (argv[*i][j] == 'e')
	*expand = 1;
      else if (argv[*i][j] == 'n')
	*nl = 1;
      else {
	match = 0;
	break;
      }
    }
    if (!match)
      return;
  }
}

void		echo_expand_arg(char *arg) {
  int		i;
  int		j;

  i = 0;
  while(arg[i] != '\0') {
    for (j = 0; replacements[j].seq != NULL; j++) {
      if (strncmp(replacements[j].seq, &arg[i], strlen(replacements[j].seq)) == 0) {
	arg[i] = replacements[j].repl;
	strcpy(&arg[i + 1], &arg[i + strlen(replacements[j].seq)]);
	break;
      }
    }
    i++;
  }
}

int		builtin_echo(char **argv) {
  bool		nl;
  bool		expand;
  int		i;

  echo_getopt(argv, &i, &nl, &expand);
  while (argv[i] != NULL) {
    if (expand)
      echo_expand_arg(argv[i]);
    write(1, argv[i], strlen(argv[i]));
    if (argv[i + 1] != NULL)
      write(1, " ", 1);
    i++;
  }
  if (!nl)
    write(1, "\n", 1);
  return 0;
}
