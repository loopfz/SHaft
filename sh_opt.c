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
#include	<string.h>
#include	<fcntl.h>
#include	"main.h"
#include	"sh_opt.h"

void		read_cmd_from_file(char *path) {
  if ((sh.read_cmd_fd = open(path, O_RDONLY)) == -1) {
    shell_die("No such file: '%s'\n", path);
  }
  sh.args[0] = path;
}

void		capture_args(char **argv, int *idx) {
  int		j;

  for (j = 1; argv[*idx] != NULL; (*idx)++) {
    sh.args[j++] = argv[*idx];
  }
  (*idx)--;
}

void		parse_options(char **argv) {
  int		i;
  int		j;
  struct {
    char *opt;
    void (*handler)(char **argv, int *idx);
  } known_opts[] = {
    { "-c", cmd_string_handler },
    { "-s", keep_args_handler },
    { NULL, NULL }
  };

  sh.args[0] = argv[0];
  for (i = 1; argv[i] != NULL; i++) {
    for (j = 0; known_opts[j].opt != NULL; j++) {
      if (strcmp(argv[i], known_opts[j].opt) == 0) {
	known_opts[j].handler(argv, &i);
	break;
      }
    }
    if (known_opts[j].opt == NULL) {
      if (i == 1)
	read_cmd_from_file(argv[i]);
      else
	capture_args(argv, &i);
    }
  }
}

void		cmd_string_handler(char **argv, int *idx) {
  int		pfd[2];

  (*idx)++;
  if (argv[*idx] == NULL) {
    shell_die("-c: Missing parameter.\n");
  }
  if (pipe(pfd) == -1) {
    shell_die("-c: pipe() failed.\n");
  }
  write(pfd[1], argv[*idx], strlen(argv[*idx]));
  close(pfd[1]);
  sh.read_cmd_fd = pfd[0];
  if (argv[*idx + 1] != NULL) {
    (*idx)++;
    capture_args(argv, idx);
  }
}

void		keep_args_handler(char **argv, int *idx) {
  (*idx)++;
  capture_args(argv, idx);
}
