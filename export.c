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
#include	"sh_local_vars.h"
#include	"env.h"
#include	"export.h"

void		export_and_set(char *name, char *value) {
  unset_local_var(name);
  sh_setenv(name, value);
}

void		export_local_var(char *name) {
  var_t		*var;

  if ((var = get_local_var(name)) != NULL) {
    sh_setenv(name, var->value);
    unset_local_var(name);
  }
}

int		show_env() {
  int		i;

  if (sh.environment == NULL)
    return 0;
  for (i = 0; sh.environment[i] != NULL; i++) {
    printf("%s\n", sh.environment[i]);
  }
  return 0;
}

int		builtin_export(char **argv) {
  int		i;
  int		j;
  bool		set;

  if (argv[1] == NULL) {
    return show_env();
  }
  for (i = 1; argv[i] != NULL; i++) {
    set = 0;
    for (j = 0; argv[i][j] != '\0'; j++) {
      if (argv[i][j] == '=') {
	argv[i][j] = '\0';
	export_and_set(argv[i], &argv[i][j + 1]);
	set = 1;
	break;
      }
    }
    if (!set)
      export_local_var(argv[i]);
  }
  return 0;
}
