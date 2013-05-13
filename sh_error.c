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

#include	<stdarg.h>
#include	<stdio.h>
#include	<unistd.h>
#include	"main.h"

void		sh_print_error(char *fmt, va_list ap);

void		sh_print_error(char *fmt, va_list ap) {
  vfprintf(stderr, fmt, ap);
}

void		shell_die(char *fmt, ...) {
  va_list	ap;

  va_start(ap, fmt);

  sh_print_error(fmt, ap);

  if (sh.read_cmd_fd > 0)
    close(sh.read_cmd_fd);

  va_end(ap);

  exit(-1);
}

void		shell_warn(char *fmt, ...) {
  va_list	ap;

  va_start(ap, fmt);

  sh_print_error(fmt, ap);

  va_end(ap);
}
