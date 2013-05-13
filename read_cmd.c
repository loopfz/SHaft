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
#include	<fcntl.h>
#include	"main.h"
#include	"env.h"
#include	"sh_interactive.h"
#include	"sh_tree.h"
#include	"parse_input.h"

void		read_cmd_loop() {
  exec_node_t*	exec_tree;
  struct {
    int in;
    int out;
  } fd_backup;

  check_interactive_sh();
  fd_backup.in = dup(0);
  fd_backup.out = dup(1);

  for (;;) {
    exec_tree = parse_input();

    if (exec_tree != NULL) {
      exec_tree->exec_handler(exec_tree);
      dup2(fd_backup.in, 0);
      dup2(fd_backup.out, 1);

      exec_node_deep_free(exec_tree);
    }
    else if (sh.interrupt) {
      if (sh.interactive)
	printf("exit\n");
      break;
    }
  }
}
