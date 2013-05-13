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
#include	"sh_list.h"
#include	"alias.h"

void		display_alias(var_t *alias) {
  printf("alias %s='%s'\n", alias->name, alias->value);
}

int		show_alias(char *name) {
  var_t		*alias;

  if ((alias = get_alias(name)) != NULL) {
    display_alias(alias);
    return 0;
  }
  shell_warn("alias: '%s': Not found.\n", name);
  return -1;
}

int		list_aliases() {
  var_t		*it;

  for (it = sh.alias; it != NULL; it = it->next) {
    display_alias(it);
  }

  return 1;
}

var_t		*get_alias(char *name) {
  var_t		*it;

  for (it = sh.alias; it != NULL; it = it->next) {
    if (strcmp(it->name, name) == 0)
      return it;
  }
  return NULL;
}

void		set_alias(char *name, char *value) {
  var_t		*alias;

  if ((alias = get_alias(name)) != NULL) {
    if (alias->value)
      free(alias->value);
    alias->value = strdup(value);
  }
  else {
    alias = xmalloc(sizeof(*alias));
    memset(alias, 0, sizeof(*alias));
    alias->name = strdup(name);
    alias->value = strdup(value);
    push_in_list((list_t**)&sh.alias, (list_t*)alias);
  }
}

int		builtin_alias(char **argv) {
  int		i;
  int		j;
  bool		set;
  int		final_ret;
  int		ret;

  final_ret = 0;
  if (argv[1] == NULL)
    return list_aliases();
  for (i = 1; argv[i] != NULL; i++) {
    set = 0;
    for (j = 0; argv[i][j] != '\0'; j++) {
      if (argv[i][j] == '=') {
	argv[i][j] = '\0';
	set_alias(argv[i], &argv[i][j + 1]);
	set = 1;
	break;
      }
    }

    if (!set) {
      ret = show_alias(argv[i]);
      if (final_ret != -1)
	final_ret = ret;
    }
  }

  return final_ret;
}
